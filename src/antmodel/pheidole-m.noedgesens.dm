# file 'antmodel/pheidole-m.dm' 
p name pheidole-m
p ppmm 239
p dirnode 4
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

v 330.183 169.301 s S0
v 359.689 122.587 s S0
v 408.407 114.193 s S0
v 469.827 122.751 s S0
v 503.782 155.788 s S0
v 391.405 181.398 s sm
v 341.136 146.331 s S0
v 386.812 117.511 s S0
v 441.486 112.304 s S0
v 489.772 140.072 s S0

e 6 1 sc 28.0425 rl 62.275
e 6 2 sc 47.7167 rl 66.9503
e 6 3 sc 47.3426 rl 69.3284
e 6 4 rl 97.9324
e 6 5 rl 115.27
e 7 1 sc 19.2537 rl 25.3479
e 7 2 sc 24.0454 rl 30.102
e 8 2 sc 21.1031 rl 27.4014
e 8 3 sc 17.1173 rl 21.6162
e 9 3 sc 31.3213 rl 33.1005
e 9 4 sc 28.6543 rl 30.172
e 10 4 sc 33.7911 rl 26.4121
e 10 5 sc 18.1358 rl 21.0679
