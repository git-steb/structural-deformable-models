# file 'pappel1.dm' 
p name pappel
#p springconst 0.8
#p damping     0.1
#p mass        0.05
#p grav        0
#p viscousdrag 0.1
#p imageforce  1
#p torque      5

s g4 g 4
s g5 g 5
s g6 g 6
s ed source g5 d
s co source g6 c
s S0 k g5 0.5 ed 1

v 209.151 300.978 s g4
v 214.157 306.104 s S0
v 215.616 306.792 s S0
v 220.826 310.472 s S0
v 230.345 318.27 s S0
v 234.564 324.341 s S0
v 238.987 335.608 s S0
v 236.202 342.928 s S0
v 232.275 345.094 s S0
v 222.767 347.943 s S0
v 212.585 350.851 s S0
v 202.63 349.791 s S0
v 194.515 347.67 s S0
v 188.473 345.171 s S0
v 183.842 339.074 s S0
v 187.028 328.51 s S0
v 191.99 319.32 s S0
v 195.303 315.859 s S0
v 198.92 311.371 s S0
v 200.692 310.654 s S0
v 201.484 308.693 s S0
v 210.265 328.401 s g6
v 208.884 314.007 s co
v 226.802 335.074 s co
v 194.438 338.319 s co

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
e 19 20
e 20 21
e 21 1
e 23 24
e 24 25
e 25 23
e 22 23
e 22 24
e 22 25
e 23 1
e 23 3
e 22 4
e 23 4
e 24 4
e 24 8
e 22 11
e 24 11
e 25 11
e 25 15
e 22 18
e 25 18
e 23 18
e 23 19
e 3 20
e 19 2
