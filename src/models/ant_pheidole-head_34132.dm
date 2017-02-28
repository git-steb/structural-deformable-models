# file 'models/ant_head_34132.dm' 
p name pheidole-h
p ppmm 157
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

v 221.363 333.655 s S0
v 248.621 319.007 s S0
v 271.935 301.839 s S0
v 300.253 277.257 s S0
v 323.523 239.519 s S0
v 326.875 232.936 s S0
v 333.895 172.032 s S0
v 317.606 136.152 s S0
v 288.469 122.558 s S0
v 251.457 126.893 s S0
v 230.171 138.173 s S0
v 196 165.212 s S0
v 162.989 188.013 s S0
v 136.012 218.965 s S0
v 116.727 244.11 s S0
v 117.214 273.976 s S0
v 121.467 303.016 s S0
v 121.301 324.107 s S0
v 193.705 343.092 s S0
v 210.823 277.519 s sm
v 237.005 233.071 s sm
v 265.019 205.958 s sm
v 285.645 176.087 s sm
v 163.138 243.18 s sm
v 162.263 311.884 s sm

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
