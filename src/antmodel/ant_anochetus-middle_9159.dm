# file 'antmodel/ant_anochetus-middle_9159.dm' 
p name anochetus-m
p ppmm 159
p dirnode 6
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 280.842 101.688 s S0
v 315.704 100.819 s S0
v 374.141 102.908 s S0
v 429.212 136.122 s S0
v 477.385 151.97 s S0
v 529.919 170.197 s S0
v 559.885 200.724 s S0
v 308.332 197.083 s S0
v 367.785 151.421 s sm
v 395.969 188.228 s sm
v 475.53 208.794 s sm
v 314.363 146.544 s sm

e 1 2 rl 34.9395 es f 2
e 2 3 rl 60.5015 es f 4
e 3 4 rl 65.9543 es f 4
e 4 5 rl 51.8366 es f 3
e 5 6 rl 57.1603 es f 3
e 6 7 sc 34.8444 rl 45.4294 es f 3
e 9 8 rl 75.133
e 10 9 rl 46.8652 es f 2
e 9 3 sc 32.1849 rl 49.3343
e 10 4 sc 39.6828 rl 60.8257
e 11 4 rl 85.1613
e 11 10 rl 82.158 es f 6
e 11 5 rl 55.495
e 11 6 sc 40.0878 rl 66.8098
e 11 7 rl 83.2841
e 10 8 rl 87.236
e 12 1 rl 55.2112
e 12 8 sc 32.1115 rl 52.8956
e 12 9 rl 53.1558 es f 3
e 12 2 rl 46.7578
e 9 4 rl 62.3599
e 12 3 rl 73.1673
