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

v 551.021 158.913 s S0
v 573.803 161.138 s S0
v 588.994 159.654 s S0
v 600.638 161.843 s S0
v 610.94 186.767 s S0
v 622.632 217.375 s S0
v 571.652 204.471 s sm

e 6 5 sc 12.4347 rl 32.4937
e 5 4 sc 4.4806 rl 24.5134
e 4 3 sc 5.42709 rl 13.5304
e 3 2 sc 4.50732 rl 17.013
e 2 1 sc 11.2162 rl 23.0282
e 7 1 sc 44.5643 rl 50.3623
e 7 2 sc 31.9927 rl 42.2445
e 7 3 sc 30.9971 rl 48.5451
e 7 4 rl 52.1032
e 7 5 sc 24.8218 rl 41.7062
e 7 6 sc 31.9644 rl 53.0678
