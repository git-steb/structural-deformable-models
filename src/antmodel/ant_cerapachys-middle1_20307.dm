# file 'antmodel/ant_cerapachys-middle1_20307.dm' 
p name cerapachys-middle1
p ppmm 285
p dirnode 5
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 517.257 101.992 s S0
v 474.015 123.46 s S0
v 462.993 157.774 s S0
v 561.045 128.789 s S0
v 573.674 163.633 s S0
v 523.798 232.957 s S0
v 473.346 208.18 s sm
v 511.125 159.806 s sm

e 1 2 sc 18.733 rl 50.0072 es f 4
e 2 3 sc 14.9094 rl 38.0409 es f 3
e 4 1 rl 51.796 es f 3
e 5 4 rl 35.5286 es f 3
e 6 5 rl 86.846 es f 9
e 7 3 rl 51.9884
e 7 6 rl 59.0282
e 7 5 rl 107.724
e 8 5 rl 62.411
e 8 7 rl 62.4361 es f 6
e 8 2 rl 51.9676
e 8 3 rl 47.8244
e 8 1 rl 57.6022
e 8 4 rl 60.2132
