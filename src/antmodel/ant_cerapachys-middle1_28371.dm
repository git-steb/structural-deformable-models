# file 'antmodel/ant_cerapachys-middle1_28371.dm' 
p name cerapachys-middle1
p ppmm 290
p dirnode 5
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 580.051 106.113 s S0
v 536.72 125.176 s S0
v 527.092 161.448 s S0
v 627.033 131.919 s S0
v 637.641 166.633 s S0
v 590.851 240.187 s S0
v 537.922 213.092 s sm
v 574.704 162.23 s sm

e 1 2 sc 18.733 rl 50.8845 es f 4
e 2 3 sc 14.9094 rl 38.7083 es f 3
e 4 1 rl 52.7047 es f 3
e 5 4 rl 36.1519 es f 3
e 6 5 rl 88.3696 es f 9
e 7 3 rl 52.9005
e 7 6 rl 60.0638
e 7 5 rl 109.614
e 8 5 rl 63.5059
e 8 7 rl 63.5315 es f 6
e 8 2 rl 52.8793
e 8 3 rl 48.6634
e 8 1 rl 58.6128
e 8 4 rl 61.2696
