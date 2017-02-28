# file 'antmodel/ant_anochetus-middle_10005965.dm' 
p name anochetus-m
p ppmm 360
p dirnode 6
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 456.083 192.146 s S0
v 496.803 162.182 s S0
v 578.181 141.975 s S0
v 670.993 157.397 s S0
v 746.216 172.961 s S0
v 825.724 204.305 s S0
v 856.904 263.105 s S0
v 535.338 269.14 s S0
v 578.686 197.579 s sm
v 629.984 230.073 s sm
v 746.093 252.035 s sm
v 518.924 206.965 s sm

e 1 2 rl 50.0034 es f 2
e 2 3 rl 84.6181 es f 4
e 3 4 rl 94.6732 es f 4
e 4 5 rl 75.3927 es f 3
e 5 6 rl 83.513 es f 3
e 6 7 sc 43.5805 rl 64.7983 es f 3
e 9 8 rl 82.5712
e 10 9 rl 63.0609 es f 2
e 9 3 sc 33.4049 rl 57.2388
e 10 4 sc 48.2113 rl 83.4152
e 11 4 rl 121.251
e 11 10 rl 121.283 es f 6
e 11 5 rl 79.8658
e 11 6 rl 95.0936
e 11 7 rl 114.462
e 10 8 rl 102.083
e 12 1 rl 65.3636
e 12 8 sc 36.4042 rl 63.1949
e 12 9 sc 44.5801 rl 61.2931 es f 3
e 12 2 rl 48.4604
e 9 4 rl 98.7716
e 12 3 rl 88.271
