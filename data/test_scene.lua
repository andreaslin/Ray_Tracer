-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0.0, 0.2)
mat5 = gr.material({0.3, 0.1, 0.5}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat6 = gr.material({0.9, 0.9, 0.9}, {0.5, 0.7, 0.5}, 25, 1.7, 0.2)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, -400}, 100)
scene_root:add_child(s1)
s1:set_material(mat1)

s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
scene_root:add_child(s2)
s2:set_material(mat1)
s2:set_texture('square.png')

s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
scene_root:add_child(s3)
s3:set_material(mat2)

b1 = gr.nh_box('b1', {-200, -125, 0}, 100)
scene_root:add_child(b1)
b1:set_material(mat4)
b1:set_texture('pattern01.png')

s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
scene_root:add_child(s4)
s4:set_material(mat3)
s4:set_texture('pattern02.png')

s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
scene_root:add_child(s5)
s5:set_material(mat1)

s6 = gr.cylinder('s6')
s6:translate( 200, 300, -100 )
s6:scale( 75, 75, 75 )
s6:rotate( 'x', -90 )
scene_root:add_child(s6)
s6:set_material(mat3)

s7 = gr.nh_sphere('s7', {0, 0, 500}, 50 )
scene_root:add_child(s7)
s7:set_material(mat6)

-- Try CSG
s8 = gr.nh_sphere('s8', {0, 300, -100}, 75 )
--scene_root:add_child(s8)
s8:set_material(mat2)

s9 = gr.nh_sphere('s9', {0, 250, -100}, 75 )
--scene_root:add_child(s9)
s9:set_material(mat3)

union1 = gr.union_node('union1')
union1:union( s8, s9 )
scene_root:add_child(union1)

--inter1 = gr.intersect_node('inter1')
--inter1:intersect( s8, s9 )
--scene_root:add_child(inter1)

--diff1 = gr.difference_node('diff1')
--diff1:difference( s8, s9 )
--scene_root:add_child(diff1)

-- A small stellated dodecahedron.

require('smstdodeca')

steldodec:set_material(mat3)
scene_root:add_child(steldodec)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'test_scene.png', 500, 500,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
