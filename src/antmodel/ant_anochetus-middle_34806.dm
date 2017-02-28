# file 'antmodel/ant_anochetus-middle_34806.dm' 
p name anochetus-m
p ppmm 103
p dirnode 6
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 368.678 141.19 s S0
v 395.432 131.247 s S0
v 439.49 112.312 s S0
v 492.419 121.88 s S0
v 533.827 122.74 s S0
v 580.496 124.36 s S0
v 610.199 142.94 s S0
v 398.991 190.428 s S0
v 440.424 146.589 s sm
v 472.458 166.09 s sm
v 545.451 169.151 s sm
v 403.857 160.833 s sm

e 1 2 rl 29.2419 es f 2
e 2 3 rl 49.9696 es f 4
e 3 4 rl 55.5834 es f 4
e 4 5 rl 43.3622 es f 3
e 5 6 rl 48.1516 es f 3
e 6 7 sc 43.5805 rl 37.2022 es f 3
e 9 8 rl 58.0668
e 10 9 rl 38.1564 es f 2
e 9 3 sc 33.4049 rl 34.3793
e 10 4 sc 48.2113 rl 49.0163
e 11 4 rl 71.4959
e 11 10 rl 72.9821 es f 6
e 11 5 rl 46.8034
e 11 6 rl 56.8481
e 11 7 rl 69.011
e 10 8 rl 75.9094
e 12 1 rl 39.9367
e 12 8 sc 36.4042 rl 31.3882
e 12 9 sc 44.5801 rl 41.3644 es f 3
e 12 2 rl 32.7867
e 9 4 rl 57.2177
e 12 3 rl 57.9608
