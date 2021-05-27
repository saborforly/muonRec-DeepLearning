from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from datetime import datetime
import os.path
import time

import numpy as np
from six.moves import xrange  # pylint: disable=redefined-builtin
import tensorflow as tf

#from tensorflow.models.image.junonn import junonn
import junonn_inference_vgg16_5 as junonn_inference_vgg
import junonn_loss
import junonn_inputs_tt as junonn_inputs
import re

import tensorflow.python
from tensorflow.python.framework.errors_impl import InvalidArgumentError
FLAGS = tf.app.flags.FLAGS

tf.app.flags.DEFINE_string('train_dir', r'G:/python/CNN-net/temp/junonn_train',
                           """Directory where to write event logs """
                          """and checkpoint.""")                        
tf.app.flags.DEFINE_string('ckp_dir', r'G:/python/CNN-net/temp/junonn_checkpoint',
                           """Directory where to load event logs """
                           """and checkpoint.""")
tf.app.flags.DEFINE_integer('max_steps', 180000,
                            """Number of batches to run.""")
tf.app.flags.DEFINE_boolean('log_device_placement', False,
                            """Whether to log device placement.""")

tf.app.flags.DEFINE_string('train_data', r'C:\Users\sabor\Desktop\DL\DL-CNN\deep_sphere\data-proprecess',
                           """Path to the CIFAR-10 data directory.""")

MOVING_AVERAGE_DECAY = 0.999
INITIAL_LEARNING_RATE = 0.1
LEARNING_RATE_DECAY_FACTOR = 0.1
decay_steps=50000
num_gpus=4

def tower_loss(scope, images, labels):
  logits = junonn_inference_vgg.inference(images)
  # Calculate loss.
  _ = junonn_loss.loss(logits, labels)
  losses = tf.get_collection('losses', scope)
  total_loss = tf.add_n(losses, name='total_loss')
  for l in losses + [total_loss]:
    loss_name = re.sub('tower_gpu_[0-9]*/', '', l.op.name)
    tf.summary.scalar(loss_name, l)

  return total_loss 


def average_gradients(tower_grads):
  average_grads = []
  for grad_and_vars in zip(*tower_grads):
    # Note that each grad_and_vars looks like the following:
    #   ((grad0_gpu0, var0_gpu0), ... , (grad0_gpuN, var0_gpuN))
    grads = []
    for g, _ in grad_and_vars:
      # Add 0 dimension to the gradients to represent the tower.
      expanded_g = tf.expand_dims(g, 0)

      # Append on a 'tower' dimension which we will average over below.
      grads.append(expanded_g)

    # Average over the 'tower' dimension.
    grad = tf.concat(axis=0, values=grads)
    grad = tf.reduce_mean(grad, 0)

    # Keep in mind that the Variables are redundant because they are shared
    # across towers. So .. we will just return the first tower's pointer to
    # the Variable.
    v = grad_and_vars[0][1]
    grad_and_var = (grad, v)
    average_grads.append(grad_and_var)
  return average_grads


def train():
    with tf.Graph().as_default(), tf.device('/cpu:0'):
        global_step = tf.Variable(0, trainable=False)
       
        # Get images and labels for CIFAR-10.
        images,labels = junonn_inputs.inputs(eval_data=False, train_data = FLAGS.train_data)
        float_images = tf.cast(images,tf.float32)
        batch_queue = tf.contrib.slim.prefetch_queue.prefetch_queue([float_images, labels], capacity=2 * num_gpus)

        
        lr = tf.train.exponential_decay(INITIAL_LEARNING_RATE,global_step,decay_steps,LEARNING_RATE_DECAY_FACTOR,staircase=True)        
        opt = tf.train.GradientDescentOptimizer(lr)
        tower_grads = []
        with tf.variable_scope(tf.get_variable_scope()):
            for gpu_id in range(num_gpus):
                with tf.device('/gpu:%d' % gpu_id):
                    with tf.name_scope('tower_gpu_%d' % (gpu_id)) as scope:
                        # Build a Graph that computes the logits predictions from the
                        # inference model.
                        image_batch, label_batch = batch_queue.dequeue()
                        # Calculate loss.
                        loss = tower_loss(scope, image_batch, label_batch)
                        # Reuse variables for the next tower.
                        tf.get_variable_scope().reuse_variables()
                        # Build a Graph that trains the model with one batch of examples and
                        # updates the model parameters.
                        summaries = tf.get_collection(tf.GraphKeys.SUMMARIES, scope)

                        grads = opt.compute_gradients(loss)
                        tower_grads.append(grads)

        grads = average_gradients(tower_grads)
        summaries.append(tf.summary.scalar('learning_rate', lr))

        apply_gradient_op = opt.apply_gradients(grads, global_step=global_step)
        variable_averages = tf.train.ExponentialMovingAverage(MOVING_AVERAGE_DECAY, global_step)
        variables_averages_op = variable_averages.apply(tf.trainable_variables())
        # Group all updates to into a single train op.
        train_op = tf.group(apply_gradient_op, variables_averages_op)
        # Create a saver.
        #saver = tf.train.Saver(tf.all_variables())
        saver = tf.train.Saver(tf.global_variables())

        # Build the summary operation based on the TF collection of Summaries.
        summary_op = tf.summary.merge_all()

        # Start running operations on the Graph.
        sess = tf.Session(config=tf.ConfigProto(allow_soft_placement=True, log_device_placement=False)) 
        ckpt = tf.train.get_checkpoint_state(FLAGS.ckp_dir)
        old_step = 1
        if ckpt and ckpt.model_checkpoint_path:
            print("load saver..")
            saver.restore(sess, ckpt.model_checkpoint_path)
            old_step = int(ckpt.model_checkpoint_path.split('/')[-1].split('-')[-1])
            print ("oldstep:",old_step)
        else:
            if tf.gfile.Exists(FLAGS.train_dir):
                tf.gfile.DeleteRecursively(FLAGS.train_dir)
            tf.gfile.MakeDirs(FLAGS.train_dir)
            # Build an initialization operation to run below.
            init = tf.global_variables_initializer()
            sess.run(init)

        # Start the queue runners.
        tf.train.start_queue_runners(sess=sess)

        summary_writer = tf.summary.FileWriter(FLAGS.train_dir, sess.graph)
        

        for step in xrange(old_step,FLAGS.max_steps):
            start_time = time.time()
            _, loss_value = sess.run([train_op, loss])
            duration = time.time() - start_time

            assert not np.isnan(loss_value), 'Model diverged with loss = NaN'
            
                
   
            if step % 2 == 0:
                num_examples_per_step = FLAGS.batch_size
                examples_per_sec = num_examples_per_step / duration
                sec_per_batch = float(duration)

                format_str = ('%s: step %d, loss = %.2f (%.1f examples/sec; %.3f '
                      'sec/batch)')
                print (format_str % (datetime.now(), step, loss_value,
                             examples_per_sec, sec_per_batch))

            if step % 100 == 0:
                try:
                    summary_str = sess.run(summary_op)
                    summary_writer.add_summary(summary_str, step)
                except InvalidArgumentError as e:
                    print('-----------\n')
                    print(e)
                    print('-----------\n')
            # Save the model checkpoint periodically.
            if step % 200 == 0 or (step + 1) == FLAGS.max_steps:                
                checkpoint_path = os.path.join(FLAGS.train_dir, 'model.ckpt')
                saver.save(sess, checkpoint_path, global_step=step)
            


def main(argv=None):  # pylint: disable=unused-argument
     
    import os
    #cpu_num = int(os.environ.get('CPU_NUM', 1))
    #os.environ["CUDA_DEVICE_ORDER"]="PCI_BUS_ID"   # see issue #152
    #gpu_id='0,1,2,4,5,6,7'
    #os.environ["CUDA_VISIBLE_DEVICES"]=str(gpu_id)
    
    train()


if __name__ == '__main__':
    tf.app.run()
