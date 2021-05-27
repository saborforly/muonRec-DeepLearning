
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

def add_neighbor(input_layer,nside_layer):
    pixs=pix_nb(nside_layer)
    k0=input_layer.get_shape()[0].value
    k2=input_layer.get_shape()[2].value
    end_input = tf.zeros([k0,1,k2], tf.float32)                  
    
    input_layer=tf.concat([input_layer, end_input],axis=1)                  
    output=tf.gather(input_layer,pixs,axis=1)                
    return output

def conv_op(input_op,kernel_shape,stride,name):
    with tf.variable_scope(name) as scope:
        kernel=tf.get_variable("w", shape=kernel_shape, dtype=tf.float32, initializer=tf.contrib.layers.xavier_initializer())        
        conv=tf.nn.conv1d(input_op,kernel,stride=stride,padding="SAME")
        
        
        n_out=kernel_shape[-1]
        #biases=tf.Variable(tf.constant(0.0,shape=[n_out],dtype=tf.float32),trainable=True,name='b')
        biases=tf.get_variable("b", shape=[n_out], dtype=tf.float32, initializer=tf.constant_initializer(0.0))
        z=tf.nn.bias_add(conv, biases)
        activation=tf.nn.relu(z,name='relu')
        return activation
        
def fc_op(input_op,n_out,name):
    n_in=input_op.get_shape()[-1].value
    with tf.variable_scope(name) as scope:
        #initializer=tf.contrib.layers.xavier_initializer()
        kernel=tf.get_variable("w", shape=[n_in,n_out], dtype=tf.float32, initializer=tf.contrib.layers.xavier_initializer())
        
        #add L2 regularizer to w
        weight_decay = tf.multiply(tf.nn.l2_loss(kernel),0.004, name='weight_loss')
        tf.add_to_collection('losses', weight_decay)                
        
        biases=tf.get_variable("b", shape=[n_out], dtype=tf.float32, initializer=tf.constant_initializer(0.1))
        activation = tf.nn.relu(tf.matmul(input_op, kernel) + biases, name='relu')
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
    images_nb=add_neighbor(images, nside) #add neighbor data and then conv neighbor
    conv_nb=conv_op(images_nb, [9,2,64], 9,name='conv_neighbor1_1') #conv 1D neighbor 9  strides=9
    images_nb=add_neighbor(conv_nb, nside)
    conv_nb=conv_op(images_nb, [9,64,64], 9,name='conv_neighbor1_2')
    pool1=mpool_op(conv_nb, [4], [4], name='pool1')
    nside1=int(nside/2) #32
    
    
    images_nb=add_neighbor(pool1, nside1)
    conv_nb=conv_op(images_nb, [9,64,128], 9,name='conv_neighbor2_1')
    images_nb=add_neighbor(conv_nb, nside1)
    conv_nb=conv_op(images_nb, [9,128,128], 9,name='conv_neighbor2_2') 
    pool2=mpool_op(conv_nb, [4], [4],name='pool2')
    nside2=int(nside1/2) #16

    images_nb=add_neighbor(pool2, nside2)
    conv_nb=conv_op(images_nb, [9,128,256], 9,name='conv_neighbor3_1')
    images_nb=add_neighbor(conv_nb, nside2)
    conv_nb=conv_op(images_nb, [9,256,256], 9,name='conv_neighbor3_2')
    images_nb=add_neighbor(conv_nb, nside2)
    conv_nb=conv_op(images_nb, [9,256,256], 9,name='conv_neighbor3_3')
    pool3=mpool_op(conv_nb, [4], [4],name='pool3')
    nside3=int(nside2/2)  #8
    
    images_nb=add_neighbor(pool3, nside3)
    conv_nb=conv_op(images_nb, [9,256,512], 9,name='conv_neighbor4_1')
    images_nb=add_neighbor(conv_nb, nside3)
    conv_nb=conv_op(images_nb, [9,512,512], 9,name='conv_neighbor4_2')
    images_nb=add_neighbor(conv_nb, nside3)
    conv_nb=conv_op(images_nb, [9,512,512], 9,name='conv_neighbor4_3')    
    pool4=mpool_op(conv_nb, [4], [4],name='pool4')
    nside4=int(nside3/2)  #4


    images_nb=add_neighbor(pool4, nside4)
    conv_nb=conv_op(images_nb, [9,512,512], 9,name='conv_neighbor5_1')    
    images_nb=add_neighbor(conv_nb, nside4)
    conv_nb=conv_op(images_nb, [9,512,512], 9,name='conv_neighbor5_2')
    images_nb=add_neighbor(conv_nb, nside4)
    conv_nb=conv_op(images_nb, [9,512,512], 9,name='conv_neighbor5_3')
    pool5=mpool_op(conv_nb, [4], [4],name='pool5')
    nside5=int(nside4/2) #2

    #with tf.device('/gpu:1'): 
    resh1 = tf.reshape(pool5, [FLAGS.batch_size, -1])
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


