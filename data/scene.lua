--- Materials

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat2 = gr.material({0.9, 0.0, 0.0}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat3 = gr.material({0.0, 0.0, 0.9}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0.0, 0.2)
table_colour = gr.material({0.52, 0.37, 0.26}, {0.5, 0.7, 0.5}, 25, 0.0, 0.0)
wall_colour = gr.material({0.89, 0.87, 0.71}, {0.5, 0.7, 0.5}, 25, 0.0, 0.0)
copper = gr.material({0.72, 0.45, 0.20}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2) 
monitor_colour = gr.material({0.1, 0.1, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat6 = gr.material({0.9, 0.9, 0.9}, {0.5, 0.7, 0.5}, 25, 1.52, 0.2)

scene_root = gr.node('root')

-- Setup
table = gr.nh_box('table', {-500, -1200, -500}, 1000)
scene_root:add_child(table)
table:set_material(table_colour)

wall = gr.cube('wall')
wall:translate( -1000, -1000, -1200 )
wall:scale( 2000, 2000, 10 )
scene_root:add_child(wall)
wall:set_material(wall_colour)

-- The globe
globe = gr.nh_sphere('globe', {200, -50.0, 0}, 100)
globe:set_material(mat1)
globe:set_texture('flatearth-2.png')

globe_stand = gr.cylinder('globe_stand')
globe_stand:translate(200, -200, 0)
globe_stand:scale( 75.0, 15.0, 15.0 )
globe_stand:rotate( 'x', -90.0 )
globe_stand:set_material( copper )
scene_root:add_child(globe_stand)

globe_stand2 = gr.cylinder('globe_stand2')
globe_stand2:translate(200, -200, 0)
globe_stand2:scale( 15.0, 15.0, 75.0 )
globe_stand2:rotate( 'x', -90.0 )
globe_stand2:set_material( copper )
scene_root:add_child(globe_stand2)

globe_rod = gr.cylinder('globe_rod')
globe_rod:translate(200.0, -200.0, 0.0)
globe_rod:scale( 10.0, 270.0, 10.0 )
globe_rod:rotate( 'x', -90 )
globe_rod:set_material( copper )

globe_union = gr.union_node('globe_union')
globe_union:union( globe, globe_rod )
scene_root:add_child(globe_union)

-- Crystal
crystal = gr.nh_sphere('crystal', {0.0, -100, 400.0}, 50)
scene_root:add_child(crystal)
crystal:set_material(mat6)

-- Small rocket
s3 = gr.cone('s3')
s3:translate( -150, 200.0, -100.0 )
s3:rotate( 'x', 90 )
s3:scale( 70, 70, 300 )
scene_root:add_child(s3)
s3:set_material(mat2)

s4 = gr.cylinder('s4')
s4:translate( -150, 80.0, -100 )
s4:rotate( 'x', 90 )
s4:scale( 50, 50, 250 )
scene_root:add_child(s4)
s4:set_material(mat3)

union2 = gr.union_node('union2')
union2:union( s3, s4 )
scene_root:add_child(union2)

-- Rocket stand
monitor_base = gr.cylinder('monitor_base')
monitor_base:translate( -150, -200.0, -100.0 )
monitor_base:scale(50.0, 10.0, 50.0)
monitor_base:rotate('x', -90.0)
monitor_base:set_material(monitor_colour)
scene_root:add_child(monitor_base)

monitor_stand = gr.cylinder('monitor_stand')
monitor_stand:translate( -150, -200.0, -100.0 )
monitor_stand:scale(20.0, 120.0, 20.0)
monitor_stand:rotate('x', -90.0)
monitor_stand:set_material(monitor_colour)
scene_root:add_child(monitor_stand)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, 'scene.png', 1000, 1000,
	  {0, -100, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
