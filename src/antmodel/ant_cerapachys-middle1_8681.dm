# file 'antmodel/ant_cerapachys-middle1_8681.dm' 
p name cerapachys-middle1
p ppmm 220
p dirnode 5
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 531.542 132.843 s S0
v 479.192 139.012 s S0
v 461.306 180.442 s S0
v 565.93 141.681 s S0
v 591.97 182.493 s S0
v 584.877 269.437 s S0
v 493.525 243.059 s sm
v 523.896 180.366 s sm

e 1 2 sc 19.1827 rl 52.3684 es f 4
e 2 3 sc 24.0762 rl 48.9495 es f 3
e 4 1 rl 35.9475 es f 3
e 5 4 sc 44.0358 rl 49.9775 es f 3
e 6 5 rl 87.4103 es f 9
e 7 3 rl 72.4318
e 7 6 sc 34.5916 rl 97.606
e 7 5 rl 113.979
e 8 5 rl 67.1369
e 8 7 rl 70.0627 es f 6
e 8 2 rl 59.9259
e 8 3 rl 61.3024
e 8 1 sc 42.1356 rl 47.0615
e 8 4 rl 58.6236
