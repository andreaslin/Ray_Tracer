-- For testing photon mapping

red = gr.material({0.9, 0.0, 0.0}, {0.2, 0.2, 0.2}, 25, 0.0, 0.0)
green = gr.material({0.0, 0.9, 0.0}, {0.2, 0.2, 0.2}, 25, 0.0, 0.0)
white = gr.material({0.9, 0.9, 0.9}, {0.2, 0.2, 0.2}, 25, 0.0, 0.0)
blue = gr.material({0.0, 0.0, 0.9}, {0.2, 0.2, 0.2}, 25, 0.0, 0.0)

scene_root = gr.node('root')

-- Cornell Box
-- Data is from http://www.graphics.cornell.edu/online/box/data.html
red_wall = gr.cube( 'red_wall' )
red_wall:translate( 549.6, 0.0, 0.0 )
red_wall:scale( 1.0, 548.8, 559.2 )
red_wall:set_material(red)
scene_root:add_child(red_wall)

green_wall = gr.cube( 'green_wall' )
green_wall:scale( 1.0, 548.8, 559.2 )
green_wall:set_material(green)
scene_root:add_child(green_wall)

white_floor = gr.cube( 'white_floor' )
white_floor:translate( 0.0, 0.0, 0.0 )
white_floor:scale( 559.2, 1.0, 559.2 )
white_floor:set_material(white)
scene_root:add_child(white_floor)

white_ceiling = gr.cube( 'white_ceiling' )
white_ceiling:translate( 0.0, 548.8, 0.0 )
white_ceiling:scale( 559.2, 1.0, 559.2 )
white_ceiling:set_material(white)
scene_root:add_child(white_ceiling)

white_back = gr.cube( 'white_back' )
white_back:translate( 0.0, 0.0, 559.2 )
white_back:scale( 556.0, 548.8, 1.0 )
white_back:set_material(white)
scene_root:add_child(white_back)

short_box = gr.cube( 'short_box' )
short_box:translate( 140.0, 0.0, 65.0 )
short_box:rotate('y', -15.0)
short_box:scale( 180.0, 180.0, 180.0 )
short_box:set_material(white)
--scene_root:add_child( short_box )

tall_box = gr.cube( 'tall_box' )
tall_box:translate( 265, 0.0, 247.0 )
tall_box:rotate('y', 15.0 )
tall_box:scale( 180, 330, 180 )
tall_box:set_material(white)
--scene_root:add_child( tall_box )

white_light = gr.light({278.0, 500.0, 278.0}, {0.9, 0.9, 0.9}, {1, 0, 0} )

gr.render(scene_root, 'photon_mapping.png', 500, 500,
	  {278, 273, -800}, {0, 0, 1}, {0, 1, 0}, 38,
	  {0.3, 0.3, 0.3}, {white_light})