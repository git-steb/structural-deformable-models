# file 'models/ant_head_34139.dm' 
p name pheidole-h
p ppmm 248
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

v 273.878 443.988 s S0
v 314.098 401.631 s S0
v 344.096 340.362 s S0
v 349.908 268.019 s S0
v 349.859 265.07 s S0
v 350.066 262.138 s S0
v 355.052 166.619 s S0
v 314.061 132.987 s S0
v 265.006 117.911 s S0
v 230.983 126.904 s S0
v 229.518 127.995 s S0
v 193.015 165.965 s S0
v 165.255 213.907 s S0
v 151.678 247.336 s S0
v 132.029 273.955 s S0
v 112.996 336.846 s S0
v 102.501 363.801 s S0
v 101.934 365.025 s S0
v 216.959 457.652 s S0
v 242.709 323.06 s sm
v 252.757 271.236 s sm
v 276.624 220.9 s sm
v 283.972 193.965 s sm
v 167.285 300.233 s sm
v 185.282 392.128 s sm

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
