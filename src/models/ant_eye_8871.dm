# file 'models/ant_eye_8871.dm' 
p name pheidole-h
p ppmm 239
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
s weichZeichner g 5.5

v 415.369 472.087 s S0
v 331.775 529.13 s S0
v 367.826 371.718 s S0
v 443.932 313.13 s S0
v 499.156 413.728 s S0
v 387.478 511.752 s S0
v 262.227 522.562 s S0
v 260.237 377.895 s S0

e 2 1 rl 101.202
