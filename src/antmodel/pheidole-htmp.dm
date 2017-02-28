# file 'antmodel/pheidole-h.dm' 
p name pheidole-h
p ppmm 239
p dirnode 15
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

s as C antcol.mhd
s bg C antbg.mhd
s ca K as 1 bg -1
s ant source ca f clampl 0
s sm0 source ant g 6.5
s sm source sm0 f bias 2 0.0463772 4.57895 1.16875e+006
s S1 source sm d
s S0 source S1 f bias 2 0.00322155 68.5809 1.16875e+006

v 624.3 357.499 s S0
v 632.847 303.795 s S0
v 632.86 247.382 s S0
v 625.642 217.254 s S0
v 616.01 180.2 s S0
v 600.146 140.292 s S0
v 554.196 86.3581 s S0
v 511.233 75.723 s S0
v 477.486 79.0232 s S0
v 435.797 127.961 s S0
v 422.827 180.085 s S0
v 421.73 235.869 s S0
v 426.41 286.591 s S0
v 451.858 358.537 s S0
v 476.516 391.08 s S0
v 525.682 424.285 s S0
v 614.989 394.98 s S0
v 557.271 279.319 s sm
v 537.687 227.04 s sm
v 522.552 185.537 s sm
v 485.948 132.575 s sm
v 480.785 337.135 s sm
v 555.227 363.481 s sm

e 1 2 sc 36.0712 rl 54.3185 es f 2
e 2 3 sc 27.9434 rl 56.3318 es f 2
e 3 4 sc 11.8136 rl 31.2298 es f 1
e 4 5 sc 7.2116 rl 38.6951 es f 1
e 5 6 sc 13.9352 rl 43.1531 es f 2
e 6 7 sc 24.3441 rl 70.8821 es f 3
e 7 8 sc 18.733 rl 44.1339 es f 2
e 8 9 sc 14.9094 rl 33.7275 es f 1
e 9 10 rl 64.2173 es f 3
e 10 11 sc 38.9169 rl 53.6377 es f 2
e 11 12 sc 30.2746 rl 55.8173 es f 2
e 12 13 sc 17.8887 rl 51.0588 es f 2
e 13 14 rl 76.535 es f 3
e 16 17 rl 94.0169 es f 4
e 17 1 sc 16.0865 rl 38.6777 es f 1
e 21 7 sc 36.5731 rl 82.5558
e 8 21 sc 26.7565 rl 62.2576
e 21 9 sc 28.7445 rl 54.2999
e 10 21 sc 37.9731 rl 50.4041
e 21 11 rl 79.1113
e 11 20 rl 99.8026
e 20 21 sc 23.4763 rl 64.5263 es f 3
e 20 19 sc 9.24344 rl 44.0748 es f 2
e 19 18 sc 27.77 rl 56.1889 es f 2
e 18 23 rl 84.2782 es f 4
e 23 16 rl 67.5828
e 15 23 sc 42.8447 rl 83.569
e 23 22 rl 78.7585 es f 4
e 22 18 rl 96.051 es f 4
e 22 19 rl 123.606 es f 6
e 19 12 rl 116.214
e 12 20 rl 112.769
e 20 6 sc 35.02 rl 89.9411
e 6 19 rl 106.788
e 19 5 rl 91.2552
e 4 19 sc 45.7276 rl 88.4979
e 19 3 rl 97.1474
e 3 18 rl 82.2283
e 2 18 rl 79.447
e 1 18 rl 103.054
e 1 23 sc 27.1388 rl 69.222
e 23 17 rl 67.5326
e 22 15 sc 22.5507 rl 53.6614
e 22 14 sc 13.9951 rl 35.7453
e 13 22 rl 74.0494
e 19 13 rl 126.287
e 20 7 rl 104.025
e 16 15 sc 40.2441 rl 59.3512 es f 3
e 15 14 sc 18.1348 rl 41.4127 es f 2
