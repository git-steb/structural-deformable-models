# file 'antmodel/pheidole-m1.dm' 
p name pheidole-m1
p ppmm 239
p dirnode 5
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

s cr r
s sm source cr g 6
s S0 source sm d

v 548.752 154.182 s S0
v 573.339 161.414 s S0
v 589.542 158.81 s S0
v 601.957 161.21 s S0
v 609.826 187.562 s S0
v 628.343 218.133 s S0
v 567.921 208.851 s sm

e 6 5 sc 11.0792 rl 36.5653 es f 6
e 5 4 sc 3.83322 rl 26.5704 es f 5
e 4 3 sc 4.6756 rl 15.2632 es f 2
e 3 2 sc 4.13999 rl 19.4643 es f 3
e 2 1 sc 16.0813 rl 26.1573 es f 4
e 7 1 rl 57.0423
e 7 2 sc 34.6616 rl 47.8455
e 7 3 sc 38.2565 rl 54.8532
e 7 4 rl 58.8206
e 7 5 sc 23.0074 rl 47.3124
e 7 6 rl 60.1054
