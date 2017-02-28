# file 'antmodel/pheidole-h.dm' 
p name pheidole-h
p ppmm 239
p dirnode 15

#p springconst 50
#p damping     5
#p mass        1
##p grav        0
#p viscousdrag 2
#p imageforce  130
#p torque      100

s datgreen m 0 1 0
s datblue  m 0 0 1
s smooth-r g 6          #default is datred
s smooth-g source datgreen g 6
s smooth-b source datblue  g 6
s smooth-col K smooth-r 1 smooth-g 1 smooth-b 1
#s smooth-col k d0 1 datgreen 1 datblue 1
s col-dist source smooth-col M
s cdg0 source col-dist d
s cdg source cdg0 f bias 2
#s smooth-col g 4
#s col-dist g 4

v 378.085 537.996 s cdg
v 386.297 486.755 s cdg
v 386.2 432.913 s cdg
v 379.427 404.218 s cdg
v 370.105 368.839 s cdg
v 354.999 330.799 s cdg
v 311.215 279.342 s cdg
v 270.281 269.342 s cdg
v 238.089 272.323 s cdg
v 198.193 318.998 s cdg
v 185.938 368.788 s cdg
v 184.849 421.966 s cdg
v 189.305 470.338 s cdg
v 213.678 538.866 s cdg
v 237.107 569.872 s cdg
v 283.971 601.6 s cdg
v 369.196 573.76 s cdg
v 314.186 463.398 s smooth-col
v 295.457 413.546 s smooth-col
v 281.036 373.963 s smooth-col
v 246.078 323.429 s smooth-col
v 241.205 518.506 s smooth-col
v 312.237 543.654 s smooth-col
                                   
e 1 2 sc 23.733 rl 51.8071
e 2 3 sc 15.3208 rl 53.7273
e 3 4 sc 7.50764 rl 29.7859
e 4 5 sc 8.21114 rl 36.906
e 5 6 sc 12.5755 rl 41.1579
e 6 7 sc 20.9227 rl 67.6049
e 7 8 sc 15.5121 rl 42.0934
e 8 9 sc 12.1147 rl 32.1681
e 9 10 sc 19.9014 rl 61.2482
e 10 11 sc 25.4006 rl 51.1578
e 11 12 sc 23.6999 rl 53.2366
e 12 13 sc 17.0979 rl 48.6981
e 13 14 sc 31.954 rl 72.9964
e 16 17 sc 38.6247 rl 89.6701
e 17 1 sc 10.0185 rl 36.8894
e 21 7 sc 27.8325 rl 78.7389
e 8 21 sc 24.8028 rl 59.3792
e 21 9 sc 20.9389 rl 51.7894
e 10 21 sc 25.1752 rl 48.0737
e 21 11 sc 48.0889 rl 75.4536
e 11 20 sc 45.399 rl 95.1881
e 20 21 sc 23.7449 rl 61.5429
e 20 19 sc 15.9493 rl 42.037
e 19 18 sc 21.9497 rl 53.591
e 18 23 sc 40.255 rl 80.3816
e 23 16 sc 28.0928 rl 64.4582
e 15 23 sc 42.6898 rl 79.7052
e 23 22 sc 33.6262 rl 75.1171
e 22 18 sc 41.9741 rl 91.6101
e 22 19 rl 117.891
e 19 12 sc 33.1569 rl 110.841
e 12 20 rl 107.555
e 20 6 sc 26.5126 rl 85.7827
e 6 19 rl 101.851
e 19 5 sc 37.2887 rl 87.036
e 4 19 sc 32.9292 rl 84.4062
e 19 3 sc 42.4188 rl 92.6557
e 3 18 sc 36.0545 rl 78.4265
e 2 18 sc 46.0641 rl 75.7738
e 1 18 sc 46.3465 rl 98.2892
e 1 23 sc 28.826 rl 66.0215
e 23 17 sc 33.8729 rl 64.4103
e 22 15 sc 23.4764 rl 51.1804
e 22 14 sc 18.2164 rl 34.0926
e 13 22 sc 37.4522 rl 70.6257
e 19 13 sc 42.9454 rl 120.448
e 20 7 sc 39.6674 rl 99.2157
e 16 15 sc 28.6844 rl 56.6071
e 15 14 sc 12.9073 rl 39.498
