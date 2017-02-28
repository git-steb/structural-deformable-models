# file 'antmodel/anochetus-m.dm' 
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

v 277.764 105.303 s S0
v 312.201 99.5244 s S0
v 372.656 101.236 s S0
v 428.729 135.857 s S0
v 478.403 150.707 s S0
v 532.443 169.354 s S0
v 560.06 205.416 s S0
v 310.339 198.774 s S0
v 367.998 150.46 s sm
v 396.768 187.514 s sm
v 476.76 206.188 s sm
v 314.954 146.157 s sm

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
