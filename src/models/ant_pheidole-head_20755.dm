# file 'models/ant_head_20755.dm' 
p name pheidole-h
p ppmm 374
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

v 272.185 375.718 s S0
v 293.677 339.83 s S0
v 308.528 304.838 s S0
v 323.334 258.3 s S0
v 316.746 196.867 s S0
v 317.354 183.95 s S0
v 297.378 103.976 s S0
v 257.028 88.1502 s S0
v 218.901 104.827 s S0
v 190.332 127.694 s S0
v 170.823 147.927 s S0
v 143.982 181.256 s S0
v 117.03 226.769 s S0
v 100.297 271.989 s S0
v 93.5227 318.411 s S0
v 123.772 365.697 s S0
v 132.416 391.382 s S0
v 137.082 405.875 s S0
v 236.57 406.211 s S0
v 224.86 309.938 s sm
v 225.684 249.765 s sm
v 244.371 199.615 s sm
v 257.933 153.249 s sm
v 143.451 293.897 s sm
v 186.4 369.848 s sm

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
