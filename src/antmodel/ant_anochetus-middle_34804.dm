# file 'antmodel/ant_anochetus-middle_34804.dm' 
p name anochetus-m
p ppmm 255
p dirnode 6
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 239.673 109.985 s S0
v 279.746 102.339 s S0
v 341.703 96.6067 s S0
v 414.999 102.566 s S0
v 463.35 114.913 s S0
v 506.951 125.014 s S0
v 524.94 157.939 s S0
v 313.747 220.852 s S0
v 344.263 150.24 s sm
v 384.311 180.683 s sm
v 453.755 176.896 s sm
v 301.378 146.76 s sm

e 1 2 rl 39.8865 es f 2
e 2 3 rl 61.2341 es f 4
e 3 4 rl 72.6804 es f 4
e 4 5 rl 47.9877 es f 3
e 5 6 rl 43.1251 es f 3
e 6 7 sc 43.5624 rl 35.7144 es f 3
e 9 8 rl 76.3051
e 10 9 rl 49.936 es f 2
e 9 3 sc 32.367 rl 51.3448
e 10 4 sc 47.6158 rl 83.5206
e 11 4 rl 85.5166
e 11 10 rl 67.6058 es f 6
e 11 5 rl 60.7341
e 11 6 rl 76.0836
e 11 7 rl 71.0304
e 10 8 rl 79.1474
e 12 1 rl 73.1709
e 12 8 sc 35.9716 rl 72.71
e 12 9 sc 48.8454 rl 41.3244 es f 3
e 12 2 sc 48.8778 rl 49.2388
e 9 4 rl 85.8648
e 12 3 rl 64.416
