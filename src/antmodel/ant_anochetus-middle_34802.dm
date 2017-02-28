# file 'antmodel/ant_anochetus-middle_34802.dm' 
p name anochetus-m
p ppmm 156
p dirnode 6
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 261.429 140.091 s S0
v 301.32 130.735 s S0
v 370.85 122.631 s S0
v 443.463 153.142 s S0
v 501.893 170.094 s S0
v 567.08 187.143 s S0
v 599.159 230.464 s S0
v 300.97 215.297 s S0
v 366.526 167.432 s sm
v 405.079 207.904 s sm
v 503.958 234.979 s sm
v 304.861 171.686 s sm

e 1 2 rl 40.9926 es f 2
e 2 3 rl 70.8521 es f 4
e 3 4 rl 78.7015 es f 4
e 4 5 rl 61.7457 es f 3
e 5 6 rl 68.3255 es f 3
e 6 7 sc 41.606 rl 54.2723 es f 3
e 9 8 rl 80.6809
e 10 9 rl 54.4331 es f 2
e 9 3 sc 32.2178 rl 46.3415
e 10 4 sc 45.1377 rl 66.6267
e 11 4 rl 101.312
e 11 10 rl 103.834 es f 6
e 11 5 rl 64.8595
e 11 6 rl 78.7242
e 11 7 rl 96.1315
e 10 8 rl 105.85
e 12 1 rl 53.647
e 12 8 sc 33.9232 rl 43.6695
e 12 9 sc 48.7963 rl 60.5652 es f 3
e 12 2 rl 44.1517
e 9 4 rl 78.6666
e 12 3 rl 81.0458
