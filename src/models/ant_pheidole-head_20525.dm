# file 'models/ant_eye_20525.dm' 
p name pheidole-h
p ppmm 131
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

v 225.747 356.587 s S0
v 242.08 334.459 s S0
v 252.363 315.033 s S0
v 261.929 285.268 s S0
v 271.148 239.092 s S0
v 276.981 208.999 s S0
v 264.486 141.157 s S0
v 242.184 121.064 s S0
v 223.771 116.226 s S0
v 176.991 127.115 s S0
v 155.947 154.804 s S0
v 139.014 187.497 s S0
v 115.187 227.903 s S0
v 99.968 272.27 s S0
v 98.9319 310.823 s S0
v 106.447 348.437 s S0
v 110.401 356.281 s S0
v 117 364.912 s S0
v 207.128 376.307 s S0
v 183.478 311.97 s sm
v 195.006 250.858 s sm
v 215.016 211.378 s sm
v 223.261 174.07 s sm
v 128.113 289.226 s sm
v 156.64 364.526 s sm

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
