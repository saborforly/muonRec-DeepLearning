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
import junonn_inference_vgg16 as junonn_inference_vgg
import junonn_loss
import junonn_inputs_tt as junonn_inputs
import junonn_train as junonn_train


import tensorflow.python
from tensorflow.python.framework.errors_impl import InvalidArgumentError
FLAGS = tf.app.flags.FLAGS

tf.app.flags.DEFINE_string('train_dir', r'G:/python/CNN-net/temp/junonn_train',
                           """Directory where to write event logs """
                          """and checkpoint.""")                        
tf.app.flags.DEFINE_string('ckp_dir', r'G:/python/CNN-net/temp/junonn_checkpoint',
                           """Directory where to load event logs """
                           """and checkpoint.""")
tf.app.flags.DEFINE_integer('max_steps', 200000,
                            """Number of batches to run.""")
tf.app.flags.DEFINE_boolean('log_device_placement', False,
                            """Whether to log device placement.""")

tf.app.flags.DEFINE_string('train_data', r'G:\python\CNN-net\data',
                           """Path to the CIFAR-10 data directory.""")

def train():
    with tf.Graph().as_default():
        #cpu_num = int(os.environ.get('CPU_NUM', 1))
        #gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.9)
        #sess = tf.Session(config=tf.ConfigProto(gpu_options=gpu_options))
        #sess = tf.Session(config=tf.ConfigProto(device_count={"CPU": cpu_num},inter_op_parallelism_threads = cpu_num,intra_op_parallelism_threads = cpu_num,log_device_placement=True))
        sess = tf.Session()
        global_step = tf.Variable(0, trainable=False)
       
        # Get images and labels for CIFAR-10.
        images,labels = junonn_inputs.inputs(eval_data=False, train_data = FLAGS.train_data)

        float_images = tf.cast(images,tf.float32)

        # Build a Graph that computes the logits predictions from the
        # inference model.
        logits = junonn_inference_vgg.inference(float_images)
        

        # Calculate loss.
        lossed = junonn_loss.loss(logits, labels)

        # Build a Graph that trains the model with one batch of examples and
        # updates the model parameters.
        train_op =junonn_train.train(lossed, global_step)

        # Create a saver.
        #saver = tf.train.Saver(tf.all_variables())
        saver = tf.train.Saver(tf.global_variables())

        # Build the summary operation based on the TF collection of Summaries.
        summary_op = tf.summary.merge_all()

        # Start running operations on the Graph.
        
        ckpt = tf.train.get_checkpoint_state(FLAGS.ckp_dir)
        old_step = 1

        print("start...")
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
            _, loss_value = sess.run([train_op, lossed])
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
    #os.environ["CUDA_DEVICE_ORDER"]="PCI_BUS_ID"   # see issue #152
    #gpu_id='4,5'
    #os.environ["CUDA_VISIBLE_DEVICES"]=str(gpu_id)
    
    train()


if __name__ == '__main__':
    tf.logging.set_verbosity(tf.logging.DEBUG)  
    tf.app.run()
