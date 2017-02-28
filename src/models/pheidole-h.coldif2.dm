# file 'nshape.dm' 
p name pheidole-h
p ppmm 239
#p springconst 0.1
#p damping     0.1
#p mass        0.05
#p grav        0
#p viscousdrag 0.1
#p imageforce  1
#p torque      5

#s datred   m 1 0 0 
#s datgreen m 0 1 0
#s datblue  m 0 0 1
#s smooth-r g 5.5          #default is datred
#s smooth-g source datgreen g 5.5
#s smooth-b source datblue  g 5.5
#s smooth-col k smooth-r 1 smooth-g 1 smooth-b 1
##s smooth-col k d0 1 datgreen 1 datblue 1
#s col-dist source smooth-col M
s col-dist M
s xcdg source col-dist d
s cdg source xcdg g 6
s smooth-col g 6
#s col-dist g 4
                                  
v 293 433 s cdg
v 322 398 s cdg
v 340 360 s cdg
v 357 316 s cdg
v 355 263 s cdg
v 355 241 s cdg
v 338 161 s cdg
v 307 128 s cdg
v 268 113 s cdg
v 221 125 s cdg
v 193 157 s cdg
v 169 200 s cdg
v 146 251 s cdg
v 124 302 s cdg
v 111 343 s cdg
v 139 398 s cdg
v 154 431 s cdg
v 163 453 s cdg
v 258 463 s cdg
                                   
v 246 360 s smooth-col
v 251 291 s smooth-col
v 266 237 s smooth-col
v 279 185 s smooth-col
v 167 330 s smooth-col
v 206 418 s smooth-col

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
