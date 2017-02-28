# file 'antmodel/ant_anochetus-middle_34801.dm' 
p name anochetus-m
p ppmm 173
p dirnode 6
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 244.044 143.893 s S0
v 272.695 117.052 s S0
v 339.966 106.222 s S0
v 414.271 115.708 s S0
v 472.571 119.393 s S0
v 537.711 121.963 s S0
v 578.16 152.979 s S0
v 315.131 222.343 s S0
v 348.459 148.507 s sm
v 388.76 172.95 s sm
v 488.685 178.702 s sm
v 302.381 171.715 s sm

e 1 2 rl 39.4182 es f 2
e 2 3 rl 68.6301 es f 4
e 3 4 rl 75.4191 es f 4
e 4 5 rl 59.8646 es f 3
e 5 6 rl 66.1443 es f 3
e 6 7 sc 42.7138 rl 52.6982 es f 3
e 9 8 rl 79.7936
e 10 9 rl 47.7206 es f 2
e 9 3 sc 32.576 rl 44.7316
e 10 4 sc 44.7802 rl 63.9274
e 11 4 rl 96.6503
e 11 10 rl 100.116 es f 6
e 11 5 rl 62.146
e 11 6 rl 75.5853
e 11 7 rl 92.3832
e 10 8 rl 87.3048
e 12 1 rl 64.6529
e 12 8 sc 32.1583 rl 51.8523
e 12 9 sc 45.6495 rl 51.6295 es f 3
e 12 2 rl 62.339
e 9 4 rl 73.7163
e 12 3 rl 76.6277
