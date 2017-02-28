# file 'models/ant_eye_20524.dm' 
p name pheidole-h
p ppmm 124
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

v 204.451 420.013 s S0
v 226.123 396 s S0
v 240 369.145 s S0
v 254.138 341.657 s S0
v 265.829 302.829 s S0
v 273.057 280.38 s S0
v 252.966 185.405 s S0
v 224.285 172.422 s S0
v 206.985 170.891 s S0
v 151.116 194.089 s S0
v 136.605 216.827 s S0
v 121.086 243.885 s S0
v 101.907 283.227 s S0
v 81.0368 340.909 s S0
v 67.815 376.594 s S0
v 65.8842 399.936 s S0
v 66.7065 401.728 s S0
v 71.9277 415.633 s S0
v 185.431 438.462 s S0
v 170.616 360.349 s sm
v 187.839 299.158 s sm
v 206.213 265.663 s sm
v 212.295 224.125 s sm
v 109.37 339.014 s sm
v 133.263 396.029 s sm

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
