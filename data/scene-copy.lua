-- My ray tracing scene

---------------------------------------------------------

-- MATERIALS
wall_colour = gr.material({0.89, 0.87, 0.71}, {0.5, 0.7, 0.5}, 25, 0.0, 0.0)
table_colour = gr.material({0.52, 0.37, 0.26}, {0.5, 0.7, 0.5}, 25, 0.0, 0.0)
monitor_colour = gr.material({0.1, 0.1, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
alarm_colour = gr.material({0.56, 0.9, 0.9}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)
piggy_colour = gr.material({0.93, 0.64, 0.68}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2)   
glass_material = gr.material({0.93, 0.64, 0.68}, {0.5, 0.7, 0.5}, 25, 1.52, 0.2) 
copper = gr.material({0.72, 0.45, 0.20}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2) 
silver = gr.material({0.75, 0.75, 0.75}, {0.5, 0.7, 0.5}, 25, 0.0, 0.2) 

----------------------------------------------------------

-- ##############################################
-- the scene
-- ##############################################

scene = gr.node('scene')

-- The walls
walls = gr.node('walls')

back_wall = gr.cube('back_wall')
back_wall:translate( -400.0, -50.0, -500.0 )
back_wall:scale( 800.0, 800.0, 1.0 )
back_wall:set_material(wall_colour)
walls:add_child(back_wall)

left_wall = gr.cube('left_wall')
left_wall:translate( -400.0, -50.0, -500.0 )
left_wall:scale( 1.0, 800.0, 800.0 )
left_wall:set_material(wall_colour)
walls:add_child(left_wall)

right_wall = gr.cube('right_wall')
right_wall:translate( 400.0, -50.0, -500.0 )
right_wall:scale( 1.0, 800.0, 800.0 )
right_wall:set_material(wall_colour)
walls:add_child(right_wall)

ceiling = gr.cube('ceiling')
ceiling:translate( -400, 750.0, -500.0 )
ceiling:scale( 800.0, 1.0, 800.0 )
ceiling:set_material(wall_colour)
walls:add_child(ceiling)

floor = gr.cube('floor')
floor:translate( -400, -50.0, -500.0 )
floor:scale( 800.0, 1.0, 800.0 )
floor:set_material(wall_colour)
walls:add_child(floor)

-- The workspace
workspace = gr.node('workspace')
workspace:translate( -300.0, 200.0, -500.0 )


-- The table
table = gr.node('table')

table1 = gr.cube('table1')
table1:scale( 400.0, 20.0, 300.0 )
table1:set_material(table_colour)
table:add_child(table1)

leg1 = gr.cube('leg1')
leg1:scale( 30.0, -300.0, 30.0 )
leg1:set_material(table_colour)
table:add_child(leg1)

leg2 = gr.cube('leg2')
leg2:translate( 0.0, 0.0, 270.0 )
leg2:scale( 30.0, -300.0, 30.0 )
leg2:set_material(table_colour)
table:add_child(leg2)

leg3 = gr.cube('leg3')
leg3:translate( 370.0, 0.0, 270.0 )
leg3:scale( 30.0, -300.0, 30.0 )
leg3:set_material(table_colour)
table:add_child(leg3)

leg4 = gr.cube('leg4')
leg4:translate( 370.0, 0.0, 0.0 )
leg4:scale( 30.0, -300.0, 30.0 )
leg4:set_material(table_colour)
table:add_child(leg4)

-- The monitor
monitor = gr.node('monitor')

base = gr.cylinder('base')
base:translate( 200.0, 0.0, 35.0 )
base:rotate('x', -90.0)
base:scale( 35.0, 35.0, 10.0 )
base:set_material(monitor_colour)
monitor:add_child(base)

stand = gr.cylinder('stand')
stand:translate( 200.0, 0.0, 35.0 )
stand:rotate('x', -90.0)
stand:scale( 10.0, 10.0, 50.0 )
stand:set_material(monitor_colour)
monitor:add_child(stand)

screen = gr.node('screen')

screen1 = gr.difference_node('screen1')
outer_screen = gr.cube('outer_screen')
outer_screen:scale( 200.0, 130.0, 20.0 )
outer_screen:set_material( monitor_colour )

inner_screen = gr.cube('inner_screen')
inner_screen:translate( 20.0, 20.0, 10.0 )
inner_screen:scale( 160.0, 90.0, 10.0 )
inner_screen:set_material( monitor_colour )

screen1:difference( inner_screen, outer_screen )

actual_screen = gr.cube('actual_screen')
actual_screen:translate( 20.0, 20.0, 10.0 )
actual_screen:scale( 160.0, 90.0, 10.0 )
actual_screen:set_material( monitor_colour )
actual_screen:set_texture('Screenshot.png')

screen:add_child( screen1 )
screen:add_child( actual_screen )
screen:translate( 80.0, 50.0, 35.0 )
monitor:add_child(screen)


-- The glass
glass = gr.difference_node('glass')

outer_glass = gr.cylinder('outer_glass')
outer_glass:scale( 20.0, 20.0, 30.0 )
outer_glass:set_material(glass_material)

inner_glass = gr.cylinder('inner_glass')
inner_glass:scale( 18.0, 18.0, 27.0 )
inner_glass:set_material(glass_material)

glass:difference( inner_glass, outer_glass )
glass:translate( 400.0, 50.0, 200.0 )
glass:rotate( 'x',-90.0 )
--table:add_child(glass)

-- The coaster
coaster = gr.cylinder('coaster')
coaster:translate( 400.0, 20.0, 200.0 )
coaster:scale( 30.0, 30.0, 5.0 )
coaster:rotate('x', -90.0)
coaster:set_material(piggy_colour) 
--table:add_child(coaster)


-- The globe
globe = gr.nh_sphere('globe_itself', {600.0, 400.0, 400.0}, 50)
globe:set_material(wall_colour)
globe:set_texture('flatearth-2.png')
--workspace:add_child(globe)


-- The lamp
lamp = gr.node('lamp')

lamp_base = gr.cylinder('lamp_base')
lamp_base:translate( 500.0, -200.0, 200.0 )
lamp_base:scale( 50.0, 50.0, 5.0 )
lamp_base:rotate('x', -90.0)
lamp_base:set_material(silver)
lamp:add_child(lamp_base)

workspace:add_child(lamp)

monitor:translate( 0.0, 20.0, 20.0 )
table:add_child(monitor)
workspace:add_child(table)



-- Adding things together
scene:add_child(walls)
scene:add_child(workspace)

--scene:rotate('y', -30.0)


-- Lights
white_light1 = gr.light({-100.0, 400.0, 500.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene,
	  'scene.png', 1024, 1024,
	  {50, 400.0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_light1})
