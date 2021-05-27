
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os

from six.moves import xrange  # pylint: disable=redefined-builtin
import tensorflow as tf

FLAGS = tf.app.flags.FLAGS
IMAGE_SIZE_H = 180
IMAGE_SIZE_W = 360
IMAGE_DEPTH = 2

NUM_ARGS = 2
NUM_EXAMPLES_PER_EPOCH_FOR_TRAIN = 50000
NUM_EXAMPLES_PER_EPOCH_FOR_EVAL = 10000


def read_junonn(filename_queue):

    class JunoEvent(object):
        pass
    result = JunoEvent()

    result.labellength = 1
    result.height = 180
    result.width = 360
    result.depth = 2

    reader = tf.TFRecordReader()
    result.key,value = reader.read(filename_queue)
    features = tf.parse_single_example(value,
                                     features={
                                      'image_raw':tf.FixedLenFeature([result.height*result.width*result.depth],tf.float32),
        'label_raw':tf.FixedLenFeature([result.labellength],tf.float32),
                                  })


    #result.image = tf.reshape(features['image_raw'],[result.height,result.width,result.depth])
    image = tf.reshape(features['image_raw'],[result.height,result.width,result.depth])
    imageq = image[:,:,0]
    imaget = image[:,:,1]
    meanq = tf.reduce_mean(imageq)
    meant = tf.reduce_mean(imaget)
    diffq=imageq-meanq
    difft=imaget-meant

    stdq=tf.sqrt(tf.reduce_mean(tf.square(diffq)))
    stdt=tf.sqrt(tf.reduce_mean(tf.square(difft)))

    resultimageq=diffq/stdq
    resultimaget=difft/stdt

    result.image =  tf.stack([resultimageq,resultimaget],axis=2)

    label_true=features['label_raw'][0]
    

    
    result.label=tf.stack([label_true])
   
    return result


def _generate_image_and_label_batch(image, label, min_queue_examples,
                                    batch_size, shuffle):
    # Create a queue that shuffles the examples, and then
    # read 'batch_size' images + labels from the example queue.
    num_preprocess_threads = 16
    if shuffle:
        images, label_batch = tf.train.shuffle_batch(
        [image, label],
        batch_size=batch_size,
        num_threads=num_preprocess_threads,
        capacity=min_queue_examples + 3 * batch_size,
        min_after_dequeue=min_queue_examples)
    else:
        images, label_batch = tf.train.batch(
        [image, label],
        batch_size=batch_size,
        num_threads=num_preprocess_threads,
        capacity=min_queue_examples + 3 * batch_size)

    # Display the training images in the visualizer.
    tf.summary.image('images_q', images[:,:,:,:1])
    tf.summary.image('images_t', images[:,:,:,1:])
    #tf.reshape(label_batch, [batch_size,1])
    return images, tf.reshape(label_batch, [batch_size,label.get_shape()[0].value])


def inputs_base(eval_data, train_data, batch_size):

    #print(data_dir)
    if not eval_data:
        filenames = [os.path.join(train_data, 'evt_1000_%d.bin.tfrecords' % i)
                 for i in xrange(1,201)]
        
        num_examples_per_epoch = NUM_EXAMPLES_PER_EPOCH_FOR_TRAIN
    else:
        filenames = [os.path.join(eval_data, 'evt_1000_%d.bin.tfrecords' % i)
                 for i in xrange(101,121)]
        num_examples_per_epoch = NUM_EXAMPLES_PER_EPOCH_FOR_EVAL

    for f in filenames:
        if not tf.gfile.Exists(f):
            raise ValueError('Failed to find file: ' + f)

    # Create a queue that produces the filenames to read.
    #print(filenames)
    filename_queue = tf.train.string_input_producer(filenames)

    # Read examples from files in the filename queue.
    read_input = read_junonn(filename_queue)
    reshaped_image = tf.cast(read_input.image, tf.float32)
    reshaped_label = tf.cast(read_input.label, tf.int32)
    height = read_input.height
    width = read_input.width

    # Generate a batch of images and labels by building up a queue of examples.
    return _generate_image_and_label_batch(reshaped_image, reshaped_label,
                                         batch_size*3 , batch_size,
                                         shuffle=True)
def inputs(eval_data,train_data):
    #data_dir = FLAGS.data_dir
    images, labels = inputs_base(eval_data=eval_data,
                                       train_data=train_data,
                                        batch_size=FLAGS.batch_size)
    #print(labels)
    if FLAGS.use_fp16:
        images = tf.cast(images, tf.float16)
        
        labels = tf.cast(labels, tf.float16)
        
    return images, labels
