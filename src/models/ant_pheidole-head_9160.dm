# file 'models/ant_eye_9160.dm' 
p name pheidole-h
p ppmm 68
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

v 197.355 312.599 s S0
v 218.225 295.94 s S0
v 236.569 277.987 s S0
v 255.709 256.754 s S0
v 270.848 227.947 s S0
v 277.876 217.791 s S0
v 306.781 172.727 s S0
v 299.821 136.954 s S0
v 284.886 126.072 s S0
v 256.471 121.958 s S0
v 230.02 131.195 s S0
v 196.153 148.547 s S0
v 168.839 169.836 s S0
v 146.865 193.741 s S0
v 125.787 216.424 s S0
v 121.511 257.857 s S0
v 119.285 278.83 s S0
v 120.062 294.719 s S0
v 175.965 322.285 s S0
v 182.625 264.05 s sm
v 207.464 222.537 s sm
v 237.758 194.523 s sm
v 266.989 170.485 s sm
v 156.416 224.453 s sm
v 147.439 289.932 s sm

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
