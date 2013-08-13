mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)

scene_root = gr.node('root')

s1 = gr.nh_box('s1', {-325, 180, -270}, 150)
scene_root:add_child(s1)
s1:set_material(mat2)

s2 = gr.nh_sphere('s2', {-250, 260, -200}, 100)
scene_root:add_child(s2)
s2:set_material(mat3)

union1 = gr.union_node('union1')
union1:union( s1, s2 )
scene_root:add_child(union1)

inter1 = gr.intersect_node('inter1')
inter1:intersect( s1, s2 )
inter1:translate( 250, 0, 0 )
scene_root:add_child(inter1)

diff1 = gr.difference_node('diff1')
diff1:difference( s2, s1 )
diff1:translate( 500, 0, 0 )
scene_root:add_child(diff1)

s3 = gr.cube('s3')
s3:translate( -325, -200, -270 )
s3:scale(150, 150, 150)
scene_root:add_child(s1)
s3:set_material(mat2)
s3:set_texture('pattern01.png')

s4 = gr.nh_sphere('s4', {-250, -120, -200}, 100)
scene_root:add_child(s2)
s4:set_material(mat3)
s4:set_texture('pattern02.png')

union2 = gr.union_node('union2')
union2:union( s3, s4 )
scene_root:add_child(union2)

--inter2 = gr.intersect_node('inter2')
--inter2:intersect( s3, s4 )
--inter2:translate( 250, 0, 0 )
--scene_root:add_child(inter2)

diff2 = gr.difference_node('diff2')
diff2:difference( s4, s3 )
diff2:translate( 500, 0, 0 )
scene_root:add_child(diff2)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'csg01.png', 500, 500,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})