# file 'models/ant_eye_20523.dm' 
p name pheidole-h
p ppmm 122
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

v 221.015 423.232 s S0
v 244.998 390.901 s S0
v 257.969 361.056 s S0
v 268.168 328.004 s S0
v 281.272 281.932 s S0
v 285.723 260.03 s S0
v 273.828 186.917 s S0
v 251.189 156.054 s S0
v 221.984 146.95 s S0
v 173.056 158.044 s S0
v 148.934 188.766 s S0
v 129.692 225.013 s S0
v 109.464 272.423 s S0
v 93.7857 319.991 s S0
v 76.9633 358.159 s S0
v 78.4497 404.03 s S0
v 87.8814 422.013 s S0
v 97.5536 435.078 s S0
v 195.525 445.477 s S0
v 178.063 364.966 s sm
v 192.051 296.293 s sm
v 212.324 254.331 s sm
v 221.362 212.248 s sm
v 115.076 340.015 s sm
v 143.468 419.592 s sm

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
