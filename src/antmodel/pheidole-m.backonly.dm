# file 'antmodel/pheidole-m.dm' 
p name pheidole-m
p ppmm 423
#p springconst 0.1
#p damping     0.1
#p mass        0.05
#p grav        0
#p viscousdrag 0.1
#p imageforce  1
#p torque      5


s cr r
s sm source cr g 6
s S0 source sm d

v 336.729 143.939 s S0
v 372.028 120.995 s S0
v 416.957 119.974 s S0
v 459.653 132.935 s S0
v 479.145 155.915 s S0
v 389.478 166.768 s sm
v 351.027 132.252 s S0
v 395.296 117.915 s S0
v 444.165 123.831 s S0
v 469.218 140.824 s S0

e 6 1 rl 61.1987
e 6 2 rl 52.7439
e 6 3 rl 54.6174
e 6 4 rl 77.1519
e 6 5 rl 90.8108
e 7 1 rl 17.5277
e 7 2 rl 17.4929
e 8 2 rl 21.587
e 8 3 rl 17.0294
e 9 3 rl 26.0768
e 9 4 rl 23.7697
e 10 4 rl 20.8076
e 10 5 rl 16.5974
