# file 'antmodel/pheidole-m1.dm' 
p name pheidole-m1
p ppmm 248
p dirnode 5
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 547.254 153.263 s S0
v 572.767 160.767 s S0
v 589.58 158.065 s S0
v 602.463 160.555 s S0
v 610.628 187.9 s S0
v 629.842 219.622 s S0
v 567.145 209.99 s sm

e 6 5 sc 11.0792 rl 37.9422 es f 6
e 5 4 sc 3.83322 rl 27.571 es f 5
e 4 3 sc 4.6756 rl 15.838 es f 2
e 3 2 sc 4.13999 rl 20.1973 es f 3
e 2 1 sc 16.0813 rl 27.1423 es f 4
e 7 1 rl 59.1903 es f 6
e 7 2 sc 34.6616 rl 49.6472 es f 6
e 7 3 sc 38.2565 rl 56.9188 es f 6
e 7 4 rl 61.0356 es f 6
e 7 5 sc 23.0074 rl 49.094 es f 6
e 7 6 rl 62.3688 es f 6
