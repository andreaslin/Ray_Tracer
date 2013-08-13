mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)

scene_root = gr.node('root')

s1 = gr.cylinder('s1')
s1:translate( -250, 260, -300 )
s1:scale( 50, 50, 300 )
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
diff1:difference( s1, s2 )
diff1:translate( 500, 0, 0 )
scene_root:add_child(diff1)

s3 = gr.cone('s3')
s3:translate( -250, 0, -200 )
s3:rotate( 'x', 90 )
s3:scale( 70, 70, 300 )
scene_root:add_child(s3)
s3:set_material(mat2)

s4 = gr.cylinder('s4')
s4:translate( -250, -120, -200 )
s4:rotate( 'x', 90 )
s4:scale( 50, 50, 250 )
scene_root:add_child(s4)
s4:set_material(mat3)

union2 = gr.union_node('union2')
union2:union( s3, s4 )
scene_root:add_child(union2)

inter2 = gr.intersect_node('inter2')
inter2:intersect( s3, s4 )
inter2:translate( 250, 0, 0 )
scene_root:add_child(inter2)

diff2 = gr.difference_node('diff2')
diff2:difference( s4, s3 )
diff2:translate( 500, 0, 0 )
scene_root:add_child(diff2)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'csg02.png', 500, 500,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})