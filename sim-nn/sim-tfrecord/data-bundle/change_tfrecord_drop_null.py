import os
from math import *
import gc
from six.moves import xrange  # pylint: disable=redefined-builtin
import tensorflow as tf
tf.app.flags.DEFINE_string('input_dir', r'G:/python/CNN-net/result_bundle/test-data/',
                           """Path to the CIFAR-10 data directory.""")
tf.app.flags.DEFINE_string('output_dir', r'G:/python/CNN-net/result_bundle/test-data/',
                           """Path to the G:/python/data-change/ data directory.""")
tf.app.flags.DEFINE_integer('start', 110,                   
                           """start count.""")
FLAGS = tf.app.flags.FLAGS
import numpy as np
import math
import random

def _floatlist_feature(value):
    return tf.train.Feature(float_list=tf.train.FloatList(value=value))
def read(file,start):
    change_file=os.path.basename(file)
    print(file)
    writer = tf.python_io.TFRecordWriter(FLAGS.output_dir+"evt_1000_"+str(start)+".bin.tfrecords")
    test_null=[[-1 for j in range(360)] for k in range(180)]
    test_null=np.array(test_null)        
    for serialized_example in tf.python_io.tf_record_iterator(file):
        example = tf.train.Example()
        example.ParseFromString(serialized_example)
        image = example.features.feature['image_raw'].float_list.value
        label = example.features.feature['label_raw'].float_list.value
        
        image_qt = np.reshape(image,[180,360,2])
        

        imageq=image_qt[:,:,0]
        if (imageq==test_null).all():
            continue

        example = tf.train.Example(features = tf.train.Features(feature={
            'image_raw': _floatlist_feature(image),
            'label_raw': _floatlist_feature(label),
        }))
        
        writer.write(example.SerializeToString())        
        
    writer.close()

def change_tfrecords():
    if(os.path.isdir(FLAGS.input_dir)):
        pathDir =  os.listdir(FLAGS.input_dir)
        filenames = [os.path.join(FLAGS.input_dir, 'evt_1000_%d.bin.tfrecords' % i)
                         for i in xrange(276,280)]
        i=-1
        for allDir in filenames:
            i=i+1
            start=FLAGS.start+i
            #print(allDir)
            if os.path.isfile(allDir):
                read(allDir,start)
    if(os.path.isfile(FLAGS.input_dir)):
        start=FLAGS.start
        for i in range(2):
            read(FLAGS.input_dir,start)
            start=start+1

change_tfrecords()
