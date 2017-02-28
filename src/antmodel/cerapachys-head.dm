# file 'antmodel/cerapachys-head.dm' 
p name cerapachys-head
p ppmm 285
p dirnode 15
#p springconst 50
#p damping     5
#p mass        1
#p grav        0
#p viscousdrag 2
#p imageforce  70
#p torque      100

sensors antsensors.cfg

v 268.985 380.069 s S0
v 269.907 344.998 s S0
v 265.467 308.24 s S0
v 258.019 281.359 s S0
v 247.906 261.243 s S0
v 231.221 239.558 s S0
v 199.824 209.929 s S0
v 175.99 211.453 s S0
v 160.05 218.403 s S0
v 134.027 249.144 s S0
v 127.504 281.427 s S0
v 130.32 313.993 s S0
v 138.571 345.751 s S0
v 159.185 389.966 s S0
v 180.066 405.448 s S0
v 210.754 425.226 s S0
v 266.831 405.577 s S0
v 220.649 333.389 s sm
v 204.225 300.661 s sm
v 191.173 277.617 s sm
v 165.716 249.565 s sm
v 176.426 374.782 s sm
v 227.241 386.203 s sm

e 1 2 sc 36.0712 rl 34.7389 es f 2
e 2 3 sc 27.9434 rl 36.0265 es f 2
e 3 4 sc 11.8136 rl 19.9728 es f 1
e 4 5 sc 7.2116 rl 24.7471 es f 1
e 5 6 sc 13.9352 rl 27.5982 es f 2
e 6 7 sc 24.3441 rl 45.3321 es f 3
e 7 8 sc 18.733 rl 28.2255 es f 2
e 8 9 sc 14.9094 rl 21.5701 es f 1
e 9 10 rl 41.0696 es f 3
e 10 11 sc 38.9169 rl 34.3035 es f 2
e 11 12 sc 30.2746 rl 35.6975 es f 2
e 12 13 sc 17.8887 rl 32.6542 es f 2
e 13 14 rl 48.9473 es f 3
e 16 17 rl 60.1277 es f 4
e 17 1 sc 16.0865 rl 24.736 es f 1
e 21 7 sc 36.5731 rl 52.7979
e 8 21 sc 26.7565 rl 39.8163
e 21 9 sc 28.7445 rl 34.727
e 10 21 sc 37.9731 rl 32.2355
e 21 11 rl 50.595
e 11 20 rl 63.8279
e 20 21 sc 23.4763 rl 41.2673 es f 3
e 20 19 sc 9.24344 rl 28.1877 es f 2
e 19 18 sc 27.77 rl 35.9351 es f 2
e 18 23 rl 53.8994 es f 4
e 23 16 rl 43.222
e 15 23 sc 42.8447 rl 53.4458
e 23 22 rl 50.3693 es f 4
e 22 18 rl 61.4286 es f 4
e 22 19 rl 79.0512 es f 6
e 19 12 rl 74.3237
e 12 20 rl 72.1205
e 20 6 sc 35.02 rl 57.5211
e 6 19 rl 68.2954
e 19 5 rl 58.3615
e 4 19 sc 45.7276 rl 56.5981
e 19 3 rl 62.1298
e 3 18 rl 52.5884
e 2 18 rl 50.8097
e 1 18 rl 65.9073
e 1 23 sc 27.1388 rl 44.2703
e 23 17 rl 43.1899
e 22 15 sc 22.5507 rl 34.3187
e 22 14 sc 13.9951 rl 22.8606
e 13 22 rl 47.3577
e 19 13 rl 80.7658
e 20 7 rl 66.5283
e 16 15 sc 40.2441 rl 37.9576 es f 3
e 15 14 sc 18.1348 rl 26.4851 es f 2
