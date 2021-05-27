from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from datetime import datetime
import math
import time
import os
import numpy as np
import tensorflow as tf
import datetime
import junonn_train_cd as junonn_train
import junonn_inference_res50_4 as junonn_inference_vgg
import junonn_inputs as junonn_inputs
from tensorflow.python.framework.errors_impl import InvalidArgumentError

FLAGS = tf.app.flags.FLAGS

tf.app.flags.DEFINE_string('input', r'G:\python\CNN-net\data',
                           """Path to the CIFAR-10 data directory.""")
tf.app.flags.DEFINE_string('output', r'data.txt',
                           """Directory where to write event logs """
                           """and checkpoint.""")                        
tf.app.flags.DEFINE_string('ckp_dir', r'G:/python/CNN-net/keep_dict',
                           """Directory where to load event logs """
                           """and checkpoint.""")
tf.app.flags.DEFINE_integer('evtmax', 2,
                            """Number of batches to run.""")
tf.app.flags.DEFINE_integer('batch_size', 1,
                            """Number of images to process in a batch.""")

def eval_once(saver,evals, output):
  with tf.Session() as sess:
    ckpt = tf.train.get_checkpoint_state(FLAGS.ckp_dir)
    if ckpt and ckpt.model_checkpoint_path:
      # Restores from checkpoint
      #saver = tf.train.import_meta_graph(r'G:/python/CNN-net/temp/junonn_train/model.ckpt-2.meta') 
      saver.restore(sess, ckpt.model_checkpoint_path)
      # Assuming model_checkpoint_path looks something like:
      #   /my-favorite-path/junonn_train/model.ckpt-0,
      # extract global_step from it.
      global_step = ckpt.model_checkpoint_path.split('/')[-1].split('-')[-1]
    else:
      print('No checkpoint file found')
      return

    # Start the queue runners.
    coord = tf.train.Coordinator()
    try:
      threads = []
      for qr in tf.get_collection(tf.GraphKeys.QUEUE_RUNNERS):
        threads.extend(qr.create_threads(sess, coord=coord, daemon=True,
                                         start=True))

      num_iter = int(math.ceil(FLAGS.evtmax / FLAGS.batch_size))
      print ('num_iter:%d for once,evtmax:%d,batch_size:%d' %(num_iter,FLAGS.evtmax,FLAGS.batch_size))
      total_sample_count = num_iter * FLAGS.batch_size
      print ("total_sample_count:%d for once" %(total_sample_count))
      step = 0
      starttime = datetime.datetime.now()
      while step < num_iter and not coord.should_stop():
          labels,logits,angles = sess.run(evals)
          labels[:,:3]*=17700
          logits[:,:3]*=17700
          result = np.concatenate([labels,logits,angles],axis=1)
          #print (logits)
          np.savetxt(output,result,'%s')
          step+=1
      endtime = datetime.datetime.now()
      print ((endtime - starttime).seconds)

    except Exception as e:  # pylint: disable=broad-except
      coord.request_stop(e)

    coord.request_stop()
    coord.join(threads, stop_grace_period_secs=10)

def inputs(filenames,batch_size):
   
    filenames = [os.path.join(FLAGS.input, 'evt_1000_%d.bin.tfrecords' % i)
                 for i in range(1,121)]
    for f in filenames:
      if not tf.gfile.Exists(f):
        raise ValueError('Failed to find file: ' + f)    
    filename_queue = tf.train.string_input_producer(filenames)

    read_input = junonn_inputs.read_junonn(filename_queue)
    return junonn_inputs._generate_image_and_label_batch(read_input.image,read_input.label,
                                                       batch_size*10, batch_size,
                                                       shuffle=False)

def evaluate():
  """Eval CIFAR-10 for a number of steps."""
  with tf.Graph().as_default() as g:
    # Get images and labels for CIFAR-10.
    images, labels_full = inputs(FLAGS.input,FLAGS.batch_size)
    labels = labels_full[:,:6]
    #labels_angle = labels_full[:,6:]

    
    # Build a Graph that computes the logits predictions from the
    # inference model.
    logits,endpoint = junonn_inference_vgg.resnet_v2_152(images,6)
    #logits = junonn_inference_vgg.inference(images)
    angles = tf.acos(tf.reduce_sum(tf.multiply(logits,labels)[:,3:],axis=1,keep_dims=True)/tf.sqrt(tf.reduce_sum(tf.square(logits[:,3:]),axis=1,keep_dims=True)))*180/np.pi
    evals= [labels,logits,angles]
    

    #Restore the moving average version of the learned variables for eval.
    variable_averages = tf.train.ExponentialMovingAverage(junonn_train.MOVING_AVERAGE_DECAY)
    variables_to_restore = variable_averages.variables_to_restore()
    saver = tf.train.Saver(variables_to_restore)

    output = open(FLAGS.output,'w')
    eval_once(saver,evals, output)
    output.close()


def main(argv=None):  # pylint: disable=unused-argument
  evaluate()


if __name__ == '__main__':
  tf.app.run()
