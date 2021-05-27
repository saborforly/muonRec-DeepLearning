#coding=utf-8
#idea: sphere projection into plane; using sphere convolution kernel
#投影球面到2D，使用不同于(3,3)的卷积核进行卷积运算
#方法：1球面上的3*3的像素点投影到平面上，将球面的位置和平面上的位置相对应(映射表)
#      2.取球面赤道上的3*3核为例，当核移动到球面极点位置的时候3*3的核则对应着新的位置(形变)，卷积时保留原始的3*3不变其他位置填0
#      3.卷积stride向下移动时，核的3*3保持为变量viriable，其他位置填0
