#include "a5.hpp"

Vector3D randomDirection() {
	double ran = (double)std::rand() / ( (double)RAND_MAX / 2.0 ) - 1;
	double x, y, z;
	Vector3D direction;
	do {
		direction = Vector3D(0.0, 0.0, 0.0);
		direction[0] += ran;
		x = pow( direction[0], 2 );
		ran = (double)std::rand() / ( (double)RAND_MAX / 2.0 ) - 1;
		direction[1] += ran;
		y = pow( direction[1], 2 );
		ran = (double)std::rand() / ( (double)RAND_MAX / 2.0 ) - 1;
		direction[2] += ran;	
		z = pow( direction[2], 2 );
	} while( x + y + z > 1 );
	direction.normalize();
	return direction;
}

bool compare_lights( const Light* l1, const Light* l2 ) {
	return ( l1->intensity < l2->intensity );
}

double getDistance( const Point3D &p1, const Point3D &p2 ) {
	return sqrt( pow( p1[0] - p2[0], 2 ) + pow( p1[1] - p2[1], 2 ) + pow( p1[2] - p2[2], 2 ) );
}

void* start( void *p ) {
	Parameters *para = (Parameters *)p;
	unsigned int id = para->id;
	Renderer *r = para->renderer;
	if ( PHOTON_MAPPING ) r->mapping( id );
	r->render( id );
	return NULL;
}

void a5_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               )
{
	std::cerr << "Stub: a5_render(" << root << ",\n     "
	          << filename << ", " << width << ", " << height << ",\n     "
	          << eye << ", " << view << ", " << up << ", " << fov << ",\n     "
	          << ambient << ",\n     {";

	for (std::list<Light*>::const_iterator I = lights.begin(); I != lights.end(); ++I) {
		if (I != lights.begin()) std::cerr << ", ";
		std::cerr << **I;
	}
	std::cerr << "});" << std::endl;

	// Fill in raytracing code here.
	Renderer *renderer;
	renderer = new Renderer( root, filename, width, height, eye, view, up , fov, ambient, lights );

	pthread_t threads[NUM_THREADS];
	Parameters para[NUM_THREADS];
	// Create threads
	for( unsigned int i = 0; i < NUM_THREADS; i += 1 ) {
		para[i] = Parameters( i, renderer );
		pthread_create( &threads[i], NULL, &start, (void*)(para+i) );
	}

	// Wait for threads to finish
	for( unsigned int i = 0; i < NUM_THREADS; i += 1 ) {
		pthread_join( threads[i], NULL );
	}

	std::cout << std::endl;
}

Renderer::Renderer( // What to render
                   SceneNode* root,
                   // Where to output the image
                   const std::string& filename,
                   // Image size
                   int width, int height,
                   // Viewing parameters
                   const Point3D& eye, const Vector3D& view,
                   const Vector3D& up, double fov,
                   // Lighting parameters
                   const Colour& ambient,
                   const std::list<Light*>& lights ) :
	root( root ), filename( filename ), width( width ), height( height ),
	eye( eye ), view( view ), up( up ), fov( fov ), ambient( ambient ), lights( lights ) {
	img = new Image(width, height, 3);
 
	// Orthonormal basis, notice the order of cross product matter
	// which I don't really know why but I've discovered it by try and error...
	w = -1 * view;
	w.normalize();
	u = up.cross( w );
	u.normalize();
	v = u.cross( w );
	v.normalize();

	m_fov = fov * TO_RADIAN;
	focal = height / ( 2 * tan( m_fov / 4 ) );
	m_width = (double)width;
	m_height = (double)height;

	total_pixel = (unsigned int)(width * height);
	proceed_pixel = 0;
	percent = 0;

	height_offset = m_height / NUM_THREADS;

	// Photon mapping
	photon_offset = NUM_PHOTONS / NUM_THREADS;

	// Calculate the intensity of all the lights
	for( std::list<Light*>::const_iterator it = lights.begin(); it != lights.end(); it++ ) {
		// Intensity is simply defined as the sum of attenuation
		Light *l = (*it);
		double intensity = l->falloff[0] + l->falloff[1] + l->falloff[2];
		total_intensity += intensity;
		(*it)->intensity = intensity;
	}

	// Sort the light sources
	this->lights.sort( compare_lights );

#ifdef KDTREE
	// Initialize photon map
	photon_map = kd_create(3);
#endif

	// Initialize seed using time
	std::srand( time(NULL) );
}

Renderer::~Renderer() {
#ifdef KDTREE
	kd_free( photon_map );
#endif
}

void Renderer::render( unsigned int id ) {	
	int y_min = id * height_offset;
	int y_max = ( id == NUM_THREADS - 1 ? height : ( id + 1 ) * height_offset );

	// Iterating through each pixel
	for (int y = y_min; y < y_max; y++) {
		for (int x = 0; x < width; x++) {
			// Colour of the pixel / background, can be modified to a better looking background
			Colour background_colour( 0.0, 0.0, 1 - (double)y / m_height );

			// Next: Determine the propagation direction of the primary ray
			// Convert pixel to screen coordinates
			double x_cor = -m_width + 2 * ( x + 0.5 );
			double y_cor = -m_height + 2 * ( y + 0.5 );
			Vector3D direction = x_cor * u + y_cor * v - focal * w;
			direction.normalize();

			// Primary ray
			Ray ray( eye, direction );
			double offset = 1.0 / ((double)AA_value + 1.0);
			Colour colour_sum( 0.0 );
			double num_subpixel = pow( AA_value + 1, 2 );
				
			// Supersampling Anti-aliasing:
			// Here I've used the naive grid algorithm where I split the pixel evenly into 
			// (AA_value+1)^2 subpixels.
			/*			for( int j = 0; j < AA_value + 1; j += 1 ) {
				double AA_x_cor = -m_width + 2 * ( ( (double)x + (double)j * offset ) + 0.5 );
				for ( int k = 0; k < AA_value + 1; k += 1 ) {
					double AA_y_cor = -m_height + 2 * ( ( (double)y + (double)k * offset ) + 0.5 );
					Vector3D dir = AA_x_cor * u + AA_y_cor * v - focal * w;
					dir.normalize();
					Ray r( eye, dir );
					Colour c( 0.0 );
					c = rayTrace( r, background_colour, 0 );
					colour_sum = colour_sum + c;
				}
				}*/

			// Stochastic Sampling Anti-aliasing
			/* Using the AA_value, each pixel is divided into (AA_value+1)^2 cells and
			   randomly choose a location in each cell to trace. Here I do not explicitly
			   divide pixel into cells but rather randomly determine the position of each ray
			   directly. Weight is given by its distance from the center of the pixel. */
			std::vector<Weight> weights;
			double distance_sum = 0.0;
			double distance_square_sum = 0.0;
			double distance = 0.0;
			double temp = 0.0;
			for( int j = 0; j < AA_value + 1; j += 1 ) {
				// 0.0 - 0.99
				if ( AA_value > 0 ) {
					temp = (double)( ( std::rand() % STOCHASTIC_OFFSET ) ) / STOCHASTIC_OFFSET;
					temp += j;
				} else {
					temp = j;
				}
				double AA_x_cor = -m_width + 2 * ( ( (double)x + temp * offset ) + 0.5 );
				for ( int k = 0; k < AA_value + 1; k += 1 ) {
					// 0.0 - 0.99
					if ( AA_value > 0 ) {
						temp = (double)( ( std::rand() % STOCHASTIC_OFFSET ) ) / STOCHASTIC_OFFSET;
						temp += k;
					} else {
						temp = k;
					}
					double AA_y_cor = -m_height + 2 * ( ( (double)y + temp * offset ) + 0.5 );
					Vector3D dir = AA_x_cor * u + AA_y_cor * v - focal * w;
					dir.normalize();
					Ray r( eye, dir );
					Colour c( 0.0 );
					Intersection i;
					c = rayTrace( r, background_colour, 0, i, false );
					distance = sqrt( pow( x_cor - AA_x_cor, 2 ) + pow( y_cor - AA_y_cor, 2 ) );
					distance_sum += distance;
					distance_square_sum += pow( distance, 2 );
					Weight w( distance, c );
					weights.push_back( w );
				}
			}

			// Calculate the colour of the pixel
			double weight;
			for( std::vector<Weight>::iterator it = weights.begin(); it != weights.end(); it++ ) {
				weight = (*it).distance / distance_sum;
				if ( AA_value > 0 ) {
					colour_sum = colour_sum + (*it).colour * weight;							
				} else {
					colour_sum = colour_sum + (*it).colour;
				}
			} 

			// NOTE: The followoing is an attempt of using Gaussian Function to assign weight. Need
			//       modification
			// Gaussian Function
			/*			double mean = distance_sum / (double)weights.size();
			double standard_dev = sqrt( distance_square_sum / (double)weights.size() - mean );
			double A = 1.0 / ( standard_dev * SQUARE_ROOT_TWO_PI );
			double B = mean;
			double C = pow( standard_dev, 2 );
			double weight;
			for( std::vector<Weight>::iterator it = weights.begin(); it != weights.end(); it++ ) {
				weight = A * pow( CONSTANT_E, -( pow( (*it).distance - B, 2 ) / ( 2 * C ) ) ) ;
				weight *= 0.5;
				if ( AA_value > 0 ) {
					colour_sum = colour_sum + (*it).colour * weight;							
				} else {
					colour_sum = colour_sum + (*it).colour;
				}
				}*/


			// update the picutre with average of all the colour of the subpixel
			(*img)(x, y, 0) = colour_sum.R(); // num_subpixel;
			(*img)(x, y, 1) = colour_sum.G(); // num_subpixel;
			(*img)(x, y, 2) = colour_sum.B(); // num_subpixel;

			// For percent done
			proceed_pixel += 1;
			percentDone();
		}
	}

	// Save the image
	img->savePng(filename);	
}

void Renderer::percentDone() {
	unsigned int temp = ( proceed_pixel * 100 / total_pixel );
	if ( temp == percent ) return;
	percent = temp;
	if ( percent % 10 == 0 ) {
		std::cout << percent << "%";
	} else {
		std::cout << "-";
	}
	std::cout << std::flush;
}

void Renderer::checkLighting( Ray &ray, Intersection &intersection, Colour &colour ) {
	// Refer to the figure on CS488 notes P104, we need the following vectors
	Vector3D L;		// Incoming vector from light source
	Vector3D R;		// Perfect reflection
	Vector3D V;		// Vector to viewer
	Vector3D N;		// Normal vector

	// Two other lights
	Colour diffuse(0.0, 0.0, 0.0);
	Colour specular(0.0, 0.0, 0.0);

	Material *m = intersection.material;
	if ( intersection.primitive->has_texture() ) return;
	for ( std::list<Light*>::const_iterator it = lights.begin(); it != lights.end(); it++ ) {
		N = intersection.normal;
		N.normalize();
		L = (*it)->position - intersection.intersect;
		L.normalize();
		R = -L + 2 * ( L.dot(N) ) * N;
		R.normalize();
		V = -ray.direction;
		V.normalize();
		
		Ray check_ray( intersection.intersect, L );
		Intersection inter2 = intersection;
		// Check for shadow
		if ( !root->intersect( check_ray, inter2 ) ) {
			// check if the light is not at opposite direction
			double dot1 = L.dot(N);
			double dot2 = R.dot(V);
			if ( dot1 >= 0 )
				diffuse = diffuse + dot1 * m->getDiffuse() * (*it)->colour;
			if ( dot2 >= 0 )
				specular = specular + pow( dot2, m->getShininess() ) * m->getSpecular() * (*it)->colour;	
		}
	}

	if ( PHOTON_MAPPING ) {
		// Photon map
		double close_dist = 40.0;

		// Naive Algorithm
#ifndef KDTREE
		std::list<Photon> nearest;
		// Find some photons around the map
		for( std::list<Photon>::iterator it = photon_map.begin(); it != photon_map.end(); it++ ) {
			double dist = getDistance( intersection.intersect, (*it).final_pos );
			if ( dist < close_dist ) {
				nearest.push_back((*it));
			}
		}

		for( std::list<Photon>::iterator it = nearest.begin(); it != nearest.end(); it++ ) {
			Photon p = (*it);
			N = intersection.normal;
			N.normalize();
			L = p.final_pos - intersection.intersect;
			L.normalize();
			R = -L + 2 * ( L.dot(N) ) * N;
			R.normalize();
			V = -p.incoming_dir;
			V.normalize();
		
			// check if the light is not at opposite direction
			double dot1 = L.dot(N);
			double dot2 = R.dot(V);

			diffuse = diffuse + dot1 * m->getDiffuse() * p.colour;
			specular = specular + dot2 * pow( dot2, m->getShininess() ) * m->getSpecular() * p.colour;
		}
#endif

		// Kd-tree algorithm
#ifdef KDTREE
		kdres *result;
		Point3D intersect = intersection.intersect;
		double pos[3];
		Photon *p;
		pos[0] = intersect[0];
		pos[1] = intersect[1];
		pos[2] = intersect[2];
		
		result = kd_nearest_range3( photon_map, intersect[0], intersect[1], intersect[2], close_dist );
		unsigned int count = 0;
		while( !kd_res_end( result ) ) {
			p = (Photon*)kd_res_item( result, pos );
			N = intersection.normal;
			N.normalize();
			L = p->final_pos - intersection.intersect;
			L.normalize();
			R = -L + 2 * ( L.dot(N) ) * N;
			R.normalize();
			V = -p->incoming_dir;
			V.normalize();

			// check if the light is not at opposite direction
			double dot1 = L.dot(N);
			double dot2 = R.dot(V);

			//			if ( dot1 >= 0 ) 
			//				diffuse = diffuse + dot1 * m->getDiffuse() * p->colour;
			//			if ( dot2 >= 0 )
			//				specular = specular + dot2 * pow( dot2, m->getShininess() ) * m->getSpecular() * p->colour;
			diffuse = diffuse + p->colour;
			specular = specular + p->colour;
						
			count += 1;
			/* go to the next entry */
			kd_res_next( result );
		}
		if ( count > 0 ) {
			diffuse = Colour( diffuse.R() / count, diffuse.G() / count, diffuse.B() / count );
			specular = Colour( specular.R() / count, specular.G() / count, specular.B() / count );
		}
#endif
	}

	// Add all lights up to get the final RGB
	colour = ambient * m->getDiffuse() + diffuse + specular; 
} 

Colour Renderer::rayTrace( Ray &ray, Colour colour, unsigned int level_count, Intersection &intersection, bool is_photon ) {
	Colour ret_colour = colour;	// Remember to set to back ground colour if no primitive hit
	if ( !is_photon ) {
		if ( root->intersect( ray, intersection ) && intersection.nearest != NULL ) {
			// Check if the primitive has a texutre
			if ( intersection.primitive != NULL ) {
				Point3D in_point = intersection.intersect;
				intersection.primitive->getColour( in_point, ret_colour );
			}

			// If we hit some primitives, time to calculate the lighting and shading
			checkLighting( ray, intersection, ret_colour );
		
			// First check if the ray reaches the maximum level
			if ( level_count < MAX_RECURSIVE_LEVEL ) {
				// Get material of primitive
				Material *mat = intersection.material;
				Vector3D normal = intersection.normal;
				Vector3D ray_dir = ray.direction;
				Intersection i;

				// NOTE: The following is the simple implementation of path tracing one reflected ray only
				//       at each iteration
				// Refraction
				if ( mat->getRefractIndex() > 0 ) {
					// Original medium always have refraction index of 1
					double n = 1.0 / mat->getRefractIndex();

					// Check if the ray is casting from inside?
					if ( ray_dir.dot( normal ) >= 0 ) {
						n = mat->getRefractIndex();
						normal = -normal;
					}

					double c1 = -ray_dir.dot(normal);
					double a1 = pow( n, 2 ) * ( 1.0 - pow( c1, 2 ) );
					// Check so c2 will not square root a negative number
					if ( a1 <= 1.0 ) {
						double c2 = sqrt( 1.0 - a1 );
						Vector3D refract_dir = ( n * ray_dir ) + ( n * c1 - c2 ) * normal;
						refract_dir.normalize();
						Point3D refract_start_point = intersection.intersect;
						Ray refract_ray( refract_start_point, refract_dir );
						// HOHO, weird 0.1 coefficent appears which I don't know why ( while of course by try and error... )
						ret_colour = 0.1 * ret_colour + rayTrace( refract_ray, colour, level_count + 1, i, is_photon );
					}
				}

				// Check if primitiev is reflective
				if ( mat->getReflectIndex() > 0 ) {

					Vector3D reflect_dir = ray_dir + 2 * -ray_dir.dot(normal) * normal;
					reflect_dir.normalize();
					Point3D reflect_start_point = intersection.intersect;
					Ray reflect_ray( reflect_start_point, reflect_dir );
					ret_colour = ret_colour + mat->getReflectIndex() * rayTrace( reflect_ray, colour, level_count + 1, i, is_photon );
				}
			}
		}
	} else {
		// Photon Scattering
		if ( root->intersect( ray, intersection ) && intersection.nearest != NULL ) {
			// Use Russian roulette to determine whether to reflect, transmitt or absorb the photon
			double epsilon = (double)std::rand() / (double)RAND_MAX;
			Material *mat = intersection.material;
			Colour diffuse = mat->getDiffuse();
			Colour specular = mat->getSpecular();
			double pd_avg = ( diffuse.R() + diffuse.G() + diffuse.B() ) / 3.0;
			double ps_avg = ( specular.R() + specular.G() + specular.B() ) / 3.0;

			double pdps = pd_avg + ps_avg;
			
			Vector3D normal = intersection.normal;
			Vector3D ray_dir = ray.direction;
			
			Vector3D reflect_dir;
			Intersection i;
			
			if ( level_count < MAX_RECURSIVE_LEVEL ) {
				if ( epsilon > 0 && epsilon <= pd_avg ) {
					// Diffuse Reflection
					// Pick a random direction
					reflect_dir = randomDirection();
					Ray reflect_ray( intersection.intersect, reflect_dir );
					ret_colour = Colour( ret_colour.R() * diffuse.R() * pd_avg,
					                     ret_colour.G() * diffuse.G() * pd_avg,
					                     ret_colour.B() * diffuse.B() * pd_avg );
					ret_colour = rayTrace( reflect_ray, ret_colour, level_count + 1, intersection, is_photon );
				} else if ( epsilon > pd_avg && epsilon <= pdps ) {
					// Specular Reflection
					reflect_dir = ray_dir + 2 * -ray_dir.dot(normal) * normal;
					reflect_dir.normalize();
					Ray reflect_ray( intersection.intersect, reflect_dir );
					ret_colour = Colour( ret_colour.R() * specular.R() * ps_avg,
					                     ret_colour.G() * specular.G() * ps_avg,
					                     ret_colour.B() * specular.B() * ps_avg );
					ret_colour = rayTrace( reflect_ray, ret_colour, level_count + 1, intersection, is_photon );
				} else if ( epsilon > pdps && epsilon <= 1 ) {
					// Absorption
					// Just return the colour
					// intersection.primitive = NULL;
				}
			}
		}
	}
	
	return ret_colour;
}


// The algorithm here follows the book "Realistic Image Synthesis Using Photon Mapping" by Henrik Wann Jensen
// In chapter 5.1 using diffuse point light

// First step: Building the photon map
void Renderer::mapping( unsigned int id ) {
	// Divide the total number of photon processes
	unsigned int photon_min = id * photon_offset;
	unsigned int photon_max = ( id == NUM_THREADS - 1 ? NUM_PHOTONS : ( id + 1 ) * photon_offset );
	unsigned int count = photon_min;

	// Shoot some photons
	while(true) {
		if ( count == photon_max ) break; 
		// Choose some light source with probability
		unsigned int photon_id = std::rand() % lights.size();
		unsigned int photon = 0;
		
		for( std::list<Light*>::iterator it = lights.begin(); it != lights.end(); it++ ) {
			if ( photon == photon_id ) {
				// Use a random direction
				Vector3D direction = randomDirection();
				
				Ray r( (*it)->position, direction );
				Colour c = (*it)->colour;
				Colour ret_colour(0.0);
				Intersection i;
				ret_colour = rayTrace( r, c, 0, i, true );
				// If the photon hits some primitive, record final position, incoming direction and colour
				if ( i.primitive != NULL ) {
#ifndef KDTREE
					Photon p;
					p.final_pos = i.intersect;
					p.incoming_dir = i.incoming;
					p.colour = ret_colour;
					photon_map.push_back( p );
#endif
#ifdef KDTREE
					Photon *p = new Photon();
					p->final_pos = i.intersect;
					p->incoming_dir = i.incoming;
					p->colour = ret_colour;
					kd_insert3( photon_map, p->final_pos[0], p->final_pos[1], p->final_pos[2], p );
#endif
					count += 1; 
				}
			}
			photon += 1;
		}		
	}

	// Try looking at the photon map
	// NOTE: Please do this only under 1 thread since multi-threading is not handled
#ifdef DEBUG1
	double closest = 1.0;
	for( unsigned int x = 0; x < height; x += 1 ) {
		for ( unsigned int y = 0; y < width; y += 1 ) {
			Colour colour_sum(0.0, 0.0, 0.0);
			double x_cor = -m_width + 2 * ( x + 0.5 );
			double y_cor = -m_height + 2 * ( y + 0.5 );
			for( std::list<Photon>::iterator it = photon_map.begin(); it != photon_map.end(); it++ ) {
				double dist = getDistance( Point3D( x_cor, y_cor, (*it).final_pos[2] ), (*it).final_pos );
				if ( dist <= closest ) {
					colour_sum = (*it).colour;
				}
			}
			
			(*img)(x, y, 0) = colour_sum.R();
			(*img)(x, y, 1) = colour_sum.G();
			(*img)(x, y, 2) = colour_sum.B();
		}
	}

	// Save the image
	img->savePng(filename);	
#endif

#ifdef DEBUG1
	for (std::list<Photon>::iterator it = photon_map.begin(); it != photon_map.end(); it++ ) {
		std::cout << (*it).final_pos << " " << (*it).colour << " " << (*it).incoming_dir << std::endl;
	}
#endif
}
