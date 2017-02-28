# file 'antmodel/ant_anochetus-middle_34805.dm' 
p name anochetus-m
p ppmm 210
p dirnode 6
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 239.005 104.875 s S0
v 279.945 87.5577 s S0
v 347.541 81.3246 s S0
v 428.656 91.561 s S0
v 478.177 111.693 s S0
v 540.604 138.526 s S0
v 547.068 177.001 s S0
v 291.789 179.98 s S0
v 350.274 137.778 s sm
v 388.987 174.545 s sm
v 461.293 178.905 s sm
v 297.164 147.917 s sm

e 1 2 rl 44.6777 es f 2
e 2 3 rl 67.6261 es f 4
e 3 4 rl 80.3131 es f 4
e 4 5 rl 53.354 es f 3
e 5 6 rl 67.7983 es f 3
e 6 7 sc 43.5624 rl 38.5954 es f 3
e 9 8 rl 72.1339
e 10 9 rl 54.0881 es f 2
e 9 3 sc 32.367 rl 57.2869
e 10 4 sc 47.6158 rl 92.8652
e 11 4 rl 93.1438
e 11 10 rl 74.2359 es f 6
e 11 5 rl 68.8938
e 11 6 rl 90.9554
e 11 7 rl 85.1285
e 10 8 rl 98.8337
e 12 1 rl 72.8119
e 12 8 sc 35.9716 rl 31.5027
e 12 9 sc 48.8454 rl 51.9695 es f 3
e 12 2 sc 48.8778 rl 60.759
e 9 4 rl 93.2616
e 12 3 rl 81.3829
