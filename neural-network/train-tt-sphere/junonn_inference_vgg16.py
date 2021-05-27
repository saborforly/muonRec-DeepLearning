
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import gzip
import os
import re
import sys
import tarfile

from six.moves import urllib
import tensorflow as tf
import numpy as np
import healpy as hp

TOWER_NAME = 'tower'
FLAGS = tf.app.flags.FLAGS
#tf.contrib.layers.xavier_initializer_conv2d()
#tf.truncated_normal_initializer(stddev=5e-2, dtype=tf.float32)
def pix_nb(nside_layer):
    npix = hp.nside2npix(nside_layer)
    pixs=[]
    #loop all the pixel
    for i in range(npix):
        pixs.append(i)
        theta,phi=hp.pix2ang(nside_layer,i,nest=True)
        nb_pix=hp.get_all_neighbours(nside_layer,theta, phi,nest=True)

        for pix in nb_pix:
            pixs.append(pix)
    pixs=np.array(pixs)
    return pixs
#pix_nb_64=pix_nb(64)
pix_nb_32=pix_nb(32)
pix_nb_16=pix_nb(16)
pix_nb_8=pix_nb(8)
pix_nb_4=pix_nb(4)
pix_nb_2=pix_nb(2)

def add_neighbor(input_layer,nside_layer):
    if nside_layer==8:
        pixs=pix_nb_8
    elif nside_layer==4:
        pixs=pix_nb_4
    elif nside_layer==2:
        pixs=pix_nb_2
    elif nside_layer==32:
        pixs=pix_nb_32
    elif nside_layer==16:
        pixs=pix_nb_16
    k0=input_layer.get_shape()[0].value
    k2=input_layer.get_shape()[2].value
    end_input = tf.zeros([k0,1,k2], tf.float32)                  
    
    input_layer=tf.concat([input_layer, end_input],axis=1)                  
    output=tf.gather(input_layer,pixs,axis=1)                
    return output

def conv_op(input_op,kernel_shape,stride,name):
    with tf.name_scope(name) as scope:
        kernel=tf.get_variable(scope+"w", shape=kernel_shape, dtype=tf.float32, initializer=tf.contrib.layers.xavier_initializer())        
        conv=tf.nn.conv1d(input_op,kernel,stride=stride,padding="SAME")
        
        
        n_out=kernel_shape[-1]
        #biases=tf.Variable(tf.constant(0.0,shape=[n_out],dtype=tf.float32),trainable=True,name='b')
        biases=tf.get_variable(scope+"b", shape=[n_out], dtype=tf.float32, initializer=tf.constant_initializer(0.0))
        z=tf.nn.bias_add(conv, biases)
        activation=tf.nn.relu(z,name=scope+'relu')
        return activation
        
def fc_op(input_op,n_out,name):
    n_in=input_op.get_shape()[-1].value
    with tf.name_scope(name) as scope:
        #initializer=tf.contrib.layers.xavier_initializer()
        kernel=tf.get_variable(scope+"w", shape=[n_in,n_out], dtype=tf.float32, initializer=tf.contrib.layers.xavier_initializer())
        
        #add L2 regularizer to w
        weight_decay = tf.multiply(tf.nn.l2_loss(kernel),0.004, name=scope+'weight_loss')
        tf.add_to_collection('losses', weight_decay)                
        
        biases=tf.get_variable(scope+"b", shape=[n_out], dtype=tf.float32, initializer=tf.constant_initializer(0.1))
        #activation=tf.nn.relu_layer(input_op, kernel, biases, name=scope+'relu')
        activation = tf.nn.relu(tf.matmul(input_op, kernel) + biases, name=scope+'relu')
        return activation
    
def mpool_op(input_op,pool_shape,stride,name):
    return tf.nn.pool(input_op, window_shape=pool_shape, strides=stride, padding='SAME',pooling_type="MAX",
                       name=name)

def _activation_summary(x):
    tensor_name = re.sub('%s_[0-9]*/' % TOWER_NAME, '', x.op.name)
    #tf.summary.histogram(tensor_name + '/activations', x)
    tf.summary.scalar(tensor_name + '/sparsity', tf.nn.zero_fraction(x))

def inference(images):
    nside=64
    print("input shape is: ",images)
    #images_nb=add_neighbor(images, nside) #add neighbor data and then conv neighbor
    #conv_nb=conv_op(images_nb, [9,2,64], 9,name='conv_neighbor1_1') #conv 1D neighbor 9  strides=9
    #images_nb=add_neighbor(conv_nb, nside)
    #conv_nb=conv_op(images_nb, [9,64,64], 9,name='conv_neighbor1_2')
    conv=conv_op(images, [4,2,64], 4,name='conv1') #conv 1D continuous 4
    nside1=int(nside/2) #32
    print("conv1 shape is: ",conv)
    
    
    images_nb=add_neighbor(conv, nside1)
    conv_nb=conv_op(images_nb, [9,64,128], 9,name='conv_neighbor2_1')
    conv=conv_op(conv_nb, [4,128,128], 4,name='conv2')
    nside2=int(nside1/2) #16

    images_nb=add_neighbor(conv, nside2)
    conv_nb=conv_op(images_nb, [9,128,256], 9,name='conv_neighbor3_1')
    conv=conv_op(conv_nb, [4,256,256], 4,name='conv3')
    nside3=int(nside2/2)  #8
    print("conv3 shape is: ",conv)


    
    images_nb=add_neighbor(conv, nside3)
    conv_nb=conv_op(images_nb, [9,256,512], 9,name='conv_neighbor4_1')
    conv=conv_op(conv_nb, [4,512,512], 4,name='conv4')
    nside4=int(nside3/2)  #4


    images_nb=add_neighbor(conv, nside4)
    conv_nb=conv_op(images_nb, [9,512,512], 9,name='conv_neighbor5_1')    
    conv=conv_op(conv_nb, [4,512,512], 4,name='conv5')
    nside5=int(nside4/2) #2
    
    images_nb=add_neighbor(conv, nside5)
    conv_nb=conv_op(images_nb, [9,512,512], 9,name='conv_neighbor6_2')
    conv=conv_op(conv_nb, [4,512,512], 4,name='conv6')
    nside5=int(nside4/2) #1

    #with tf.device('/gpu:1'): 
    resh1 = tf.reshape(conv, [FLAGS.batch_size, -1])
    fc=fc_op(resh1, 1024,name='fc6')
    _activation_summary(fc)
    fc=fc_op(fc, 512,name='fc7')
    _activation_summary(fc)
    fc=fc_op(fc, 256,name='fc8')
    _activation_summary(fc)

    with tf.variable_scope('softmax_linear') as scope:
        kernel=tf.get_variable("w", shape=[256,6], dtype=tf.float32, initializer=tf.contrib.layers.xavier_initializer())
        biases=tf.get_variable("b", shape=[6], dtype=tf.float32, initializer=tf.constant_initializer(0.0))
        softmax_linear=tf.add(tf.matmul(fc, kernel), biases, name=scope.name) 
        

    return softmax_linear


