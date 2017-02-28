# file 'models/ant_eye_9080.dm' 
p name pheidole-h
p ppmm 126
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

v 194.655 329.108 s S0
v 227.549 307.841 s S0
v 249.97 288.189 s S0
v 272.138 265.068 s S0
v 290.163 232.807 s S0
v 294.816 222.921 s S0
v 314.928 161.049 s S0
v 307.694 123.501 s S0
v 288.732 105.698 s S0
v 256.699 100.379 s S0
v 224.434 108.286 s S0
v 186.813 119.006 s S0
v 148.062 144.807 s S0
v 124.814 176.989 s S0
v 109.713 204.093 s S0
v 103.075 248.269 s S0
v 95.5755 276.442 s S0
v 92.9094 295.977 s S0
v 153.93 339.059 s S0
v 183.537 260.773 s sm
v 210.278 211.836 s sm
v 238.801 184.045 s sm
v 270.214 157.205 s sm
v 146.387 211.225 s sm
v 139.047 287.356 s sm

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
