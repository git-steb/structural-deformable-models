# file 'kastanie.obj' 
# s d0 is dataset at scale 0 (full resolution) internally

p name kastanie

# -- all parameters and their default values
#p springconst	0.1
#p damping	0.1
#p mass		0.05
#p grav		0.0
#p viscousdrag	0.1
#p imageforce     1.0
#p torque		5.0

#p springconst 0.8

s g4 g 4
s g5 g 5
s ed source g5 d
#s ck source g4 c
#s  source ck g 4
s S0 k ed 1 g5 0.5
#s S0 k 5 1 0.6 0.4

v 402 105.471 s g4
v 404 109.471 s S0
v 407 114.471 s S0
v 417 124.471 s S0
v 423 140.471 s S0
v 411 169.471 s S0
v 403 189.471 s S0
v 401 193.471 s S0
v 399 200.471 s g4
v 396 193.471 s S0
v 393 186.471 s S0
v 385 166.471 s S0
v 377 141.471 s S0
v 382 124.471 s S0
v 389 116.471 s S0
v 395 113.471 s S0
v 399 109.471 s S0

v 400 150 s g4

v 388 130 s S0
v 412 130 s S0
v 408 165 s S0
v 392 165 s S0

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
e 17 1

e 1 18
e 5 18
e 18 9
e 18 13

e 19 14
e 19 20
e 20 4

e 20 21

e 21 6
e 21 22
e 22 12

e 22 19

e 18 19
e 18 20
e 18 21
e 18 22
