# file 'antmodel/ant_anochetus-middle_34799.dm' 
p name anochetus-m
p ppmm 183
p dirnode 6
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 322.173 149.063 s S0
v 351.236 117.758 s S0
v 426.142 91.4384 s S0
v 511.021 97.9797 s S0
v 576.653 92.1367 s S0
v 649.084 92.8975 s S0
v 696.132 128.058 s S0
v 386.637 202.372 s S0
v 440.742 138.92 s sm
v 491.181 165.108 s sm
v 598.96 157.041 s sm
v 377.247 158.246 s sm

e 1 2 rl 43.0265 es f 2
e 2 3 rl 74.9125 es f 4
e 3 4 rl 82.3229 es f 4
e 4 5 rl 65.3446 es f 3
e 5 6 rl 72.1992 es f 3
e 6 7 sc 38.8473 rl 58.95 es f 3
e 9 8 rl 83.8431
e 10 9 rl 56.2193 es f 2
e 9 3 sc 31.9677 rl 49.4174
e 10 4 sc 40.9617 rl 69.7793
e 11 4 rl 105.498
e 11 10 rl 109.281 es f 6
e 11 5 rl 67.8348
e 11 6 rl 82.5044
e 11 7 rl 101.415
e 10 8 rl 112.803
e 12 1 rl 55.0232
e 12 8 sc 27.8803 rl 45.3141
e 12 9 sc 48.4458 rl 63.0683 es f 3
e 12 2 rl 47.3614
e 9 4 rl 82.6968
e 12 3 rl 83.8003
