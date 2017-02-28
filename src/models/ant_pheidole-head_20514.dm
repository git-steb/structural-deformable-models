# file 'models/ant_eye_20514.dm' 
p name pheidole-h
p ppmm 132
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

v 198.135 222.897 s S0
v 222.798 227.038 s S0
v 245.069 225.929 s S0
v 268.327 224.465 s S0
v 304.078 215.003 s S0
v 311.179 211.96 s S0
v 340.059 188.369 s S0
v 349.126 173.072 s S0
v 350.507 152.006 s S0
v 333.679 126.414 s S0
v 312.949 117.92 s S0
v 281.952 105.986 s S0
v 251.563 104.155 s S0
v 222.527 112.115 s S0
v 200.08 119.102 s S0
v 173.957 139.799 s S0
v 159.974 151.793 s S0
v 151.085 163.043 s S0
v 171.665 212.99 s S0
v 222.014 182.274 s sm
v 261.763 168.988 s sm
v 286.521 167.916 s sm
v 314.491 168.026 s sm
v 221.406 141.321 s sm
v 183.048 176.715 s sm

e 1 2
e 2 3
e 3 4
e 4 5
e 5 6
e 6 7
e 7 8
e 8 9
e 9 10
e 10 11
e 11 12
e 12 13
e 13 14
e 14 15
e 15 16
e 16 17
e 17 18
e 18 19
e 19 1
e 3 25
e 23 7
e 8 23
e 23 9
e 10 23
e 23 11
e 11 22
e 22 23
e 22 21
e 21 20
e 20 25
e 25 18
e 25 17
e 16 25
e 25 24
e 24 20
e 24 21
e 21 12
e 12 22
e 22 6
e 6 21
e 21 5
e 4 21
e 21 3
e 3 20
e 2 20
e 1 20
e 1 25
e 25 19
e 24 16
e 15 24
e 24 14
e 13 24
e 21 13
e 22 7
