# file 'antmodel/pheidole-h.dm' 
p name pheidole-h
p ppmm 219
p dirnode 15
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 354.044 474.2 s S0
v 352.278 420.378 s S0
v 330.859 363.534 s S0
v 313.011 340.125 s S0
v 290.205 308.417 s S0
v 264.562 273.505 s S0
v 211.978 232.049 s S0
v 175.656 223.191 s S0
v 151.825 229.002 s S0
v 110.767 275.528 s S0
v 102.132 329.93 s S0
v 115.958 394.787 s S0
v 146.065 449.496 s S0
v 186.076 510.503 s S0
v 216.122 540.955 s S0
v 270.797 569.215 s S0
v 349.767 518.037 s S0
v 273.959 418.939 s sm
v 234.479 373.773 s sm
v 200.563 330.043 s sm
v 158.532 281.203 s sm
v 210.736 487.617 s sm
v 286.051 503.588 s sm

e 1 2 rl 53.8643 es f 2
e 2 3 rl 58.0715 es f 2
e 3 4 sc 15.6547 rl 29.5497 es f 1
e 4 5 sc 20.9441 rl 38.975 es f 1
e 5 6 sc 21.9435 rl 44.3425 es f 2
e 6 7 rl 67.4685 es f 3
e 7 8 sc 15.2222 rl 39.148 es f 2
e 8 9 sc 13.522 rl 26.5701 es f 1
e 9 10 rl 62.1456 es f 3
e 10 11 rl 55.4179 es f 2
e 11 12 rl 63.7703 es f 2
e 12 13 sc 20.8584 rl 60.9319 es f 2
e 13 14 rl 72.7417 es f 3
e 16 17 rl 92.715 es f 4
e 17 1 sc 49.3071 rl 44.4645 es f 1
e 21 7 rl 73.0656
e 8 21 rl 60.846
e 21 9 sc 41.3449 rl 53.4003
e 10 21 sc 35.8801 rl 48.2295
e 21 11 rl 75.8648
e 11 20 rl 98.1341
e 20 21 sc 41.2235 rl 63.8963 es f 3
e 20 19 sc 23.3377 rl 54.6691 es f 2
e 19 18 sc 40.8096 rl 59.5837 es f 2
e 18 23 rl 83.7186 es f 4
e 23 16 rl 66.7718
e 15 23 rl 80.2262
e 23 22 rl 77.0694 es f 4
e 22 18 rl 91.8294 es f 4
e 22 19 rl 116.199 es f 6
e 19 12 rl 118.604
e 12 20 rl 106.722
e 20 6 rl 85.1973
e 6 19 rl 104.738
e 19 5 rl 86.602
e 4 19 rl 85.6096
e 19 3 rl 95.445
e 3 18 rl 78.343
e 2 18 rl 77.2238
e 1 18 rl 98.5926
e 1 23 rl 73.8954
e 23 17 rl 65.6583
e 22 15 rl 53.893
e 22 14 sc 9.08539 rl 34.4721
e 13 22 rl 74.1171
e 19 13 rl 117.019
e 20 7 rl 98.0792
e 16 15 rl 60.9446 es f 3
e 15 14 rl 42.4907 es f 2
