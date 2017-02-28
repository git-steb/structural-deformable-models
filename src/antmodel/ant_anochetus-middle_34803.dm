# file 'antmodel/ant_anochetus-middle_34803.dm' 
p name anochetus-m
p ppmm 190
p dirnode 6
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 263.199 122.311 s S0
v 296.769 99.6424 s S0
v 366.718 86.8955 s S0
v 443.373 104.174 s S0
v 504.208 111.08 s S0
v 571.535 113.33 s S0
v 612.848 148.163 s S0
v 314.253 189.863 s S0
v 370.646 133.295 s sm
v 416.715 161.903 s sm
v 518.799 174.055 s sm
v 311.45 144.811 s sm

e 1 2 rl 41.2443 es f 2
e 2 3 rl 71.7738 es f 4
e 3 4 rl 78.9315 es f 4
e 4 5 rl 62.6029 es f 3
e 5 6 rl 69.1769 es f 3
e 6 7 sc 43.5624 rl 55.1031 es f 3
e 9 8 rl 80.6099
e 10 9 rl 53.7161 es f 2
e 9 3 sc 32.367 rl 47.286
e 10 4 sc 47.6158 rl 66.8989
e 11 4 rl 101.183
e 11 10 rl 104.735 es f 6
e 11 5 rl 65.0409
e 11 6 rl 79.0951
e 11 7 rl 96.667
e 10 8 rl 107.025
e 12 1 rl 53.7106
e 12 8 sc 35.9716 rl 44.1117
e 12 9 sc 48.8454 rl 60.1078 es f 3
e 12 2 sc 48.8778 rl 46.5068
e 9 4 rl 79.1364
e 12 3 rl 80.403
