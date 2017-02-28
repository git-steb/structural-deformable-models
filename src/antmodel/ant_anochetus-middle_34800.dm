# file 'antmodel/ant_anochetus-middle_34800.dm' 
p name anochetus-m
p ppmm 167
p dirnode 6
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 235.03 142.029 s S0
v 272.212 112.017 s S0
v 352.635 89.0585 s S0
v 444.704 101.027 s S0
v 517.584 111.079 s S0
v 597.684 123.8 s S0
v 633.074 155.948 s S0
v 297.341 211.799 s S0
v 360.01 140.897 s sm
v 414.514 171.289 s sm
v 531.717 185.016 s sm
v 292.161 161.595 s sm

e 1 2 rl 47.9475 es f 2
e 2 3 rl 83.4803 es f 4
e 3 4 rl 91.7383 es f 4
e 4 5 rl 72.8182 es f 3
e 5 6 rl 80.4567 es f 3
e 6 7 sc 41.593 rl 47.0843 es f 3
e 9 8 rl 93.4323
e 10 9 rl 62.6492 es f 2
e 9 3 sc 34.1588 rl 55.0693
e 10 4 sc 43.9051 rl 77.76
e 11 4 rl 117.564
e 11 10 rl 121.779 es f 6
e 11 5 rl 75.5932
e 11 6 rl 91.9405
e 11 7 rl 105.518
e 10 8 rl 125.704
e 12 1 rl 61.3162
e 12 8 sc 29.897 rl 50.4968
e 12 9 sc 43.7033 rl 70.2815 es f 3
e 12 2 rl 52.7782
e 9 4 rl 92.155
e 12 3 rl 93.3846
