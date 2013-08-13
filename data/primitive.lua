-- For testing new primitives

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)

scene_root = gr.node('root')

s1 = gr.cylinder('s1')
s1:translate( -300, -200, -600 )
s1:rotate( 'x', 45 )
s1:rotate( 'y', 45 )
s1:scale( 75, 75, 200 )
scene_root:add_child(s1)
s1:set_material(mat3)

s2 = gr.cylinder('s2')
s2:translate( 200, -200, -600 )
s2:rotate( 'x', 45 )
s2:rotate( 'y', 45 )
s2:scale( 75, 75, 200 )
scene_root:add_child(s2)
s2:set_material(mat1)
s2:set_texture('square.png')

s3 = gr.cone('s3')
s3:translate( 100, 400, -600 )
s3:rotate( 'x', 45 )
s3:rotate( 'y', 45 )
s3:scale( 75, 75, 300 )
scene_root:add_child(s3)
s3:set_material(mat2)

s4 = gr.cone('s4')
s4:translate( -400, 400, -600 )
s4:rotate( 'x', 45 )
s4:rotate( 'y', 45 )
s4:scale( 75, 75, 300 )
scene_root:add_child(s4)
s4:set_material(mat2)
s4:set_texture('square.png')

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'primitive.png', 500, 500,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})