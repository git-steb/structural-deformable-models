#a quad
p name quad

#s g5 g 5
s S2 g 5.5

s co source S2 c
s S0 source co g 5
s S1 source S2 d

v 130 130 s S0
v 200 130 s S0
v 200 200 s S0
v 130 200 s S0

v 165 130 s S2
v 200 165 s S2
v 165 200 s S2
v 130 165 s S2

v 165 165 s S2

e 1 5
e 5 2
e 2 6
e 6 3
e 3 7
e 7 4
e 4 8
e 8 1

e 9 5
e 9 6
e 9 7
e 9 8
