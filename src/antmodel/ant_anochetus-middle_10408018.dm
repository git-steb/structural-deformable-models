# file 'antmodel/ant_anochetus-middle_10408018.dm' 
p name anochetus-m
p ppmm 213
p dirnode 6
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 514.574 167.019 s S0
v 554.008 164.512 s S0
v 621.836 176.405 s S0
v 682.999 219.087 s S0
v 742.453 223.035 s S0
v 807.051 230.767 s S0
v 844.539 263.038 s S0
v 538.368 246.946 s S0
v 621.767 227.275 s sm
v 649.145 274.964 s sm
v 750.967 286.681 s sm
v 560.025 208.201 s sm

e 1 2 rl 39.8352 es f 2
e 2 3 rl 69.1956 es f 4
e 3 4 rl 75.1486 es f 4
e 4 5 rl 59.4389 es f 3
e 5 6 rl 65.2738 es f 3
e 6 7 sc 34.8444 rl 50.9178 es f 3
e 9 8 rl 86.1435
e 10 9 rl 54.2766 es f 2
e 9 3 sc 32.1849 rl 48.1544
e 10 4 sc 39.6828 rl 61.5534
e 11 4 rl 96.8275
e 11 10 rl 100.038 es f 6
e 11 5 rl 63.8766
e 11 6 sc 40.0878 rl 77.9574
e 11 7 rl 96.0725
e 10 8 rl 114.746
e 12 1 rl 61.6787
e 12 8 sc 32.1115 rl 44.918
e 12 9 rl 63.9514 es f 3
e 12 2 rl 43.9755
e 9 4 rl 62.5098
e 12 3 rl 68.7122
