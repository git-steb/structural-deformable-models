# file 'linde.dm' 
p name linde
#p springconst 0.8
#p damping     0.1
#p mass        0.05
#p grav        0
#p viscousdrag 0.1
#p imageforce  1
#p torque      5

s g5 g 5
s g6 g 6
s ed source g5 d
s co source g6 c
s S0 k g5 1 ed 0.5
s S1 k g5 1 co 0.4

v 395 248 s S1
v 422 279 s S1
v 397 287 s S1
v 387 235 s S0
v 393 237 s S0
v 405 237 s S0
v 420 245 s S0
v 431 259 s S0
v 431 273 s S0
v 428 289 s S0
v 419 294 s S0
v 412 291 s S0
v 406 297 s S0
v 393 298 s S0
v 382 286 s S0
v 374 273 s S0
v 375 259 s S0
v 384 242 s S0
v 402 269 s g6

e 1 2
e 2 3
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
e 18 4
e 3 1
e 1 18
e 1 4
e 1 5
e 1 6
e 1 17
e 1 16
e 1 7
e 16 3
e 3 15
e 14 3
e 3 13
e 12 3
e 12 2
e 2 11
e 10 2
e 2 9
e 18 19
e 19 3
e 19 1
e 19 2
e 19 8
e 19 7
e 7 2
e 19 16
e 19 12
