from datetime import datetime
import os.path
import time

import numpy as np
from six.moves import xrange  # pylint: disable=redefined-builtin
import tensorflow as tf

#from tensorflow.models.image.junonn import junonn
import junonn_inference_vgg16_bundle as junonn_inference

import junonn_loss_bundle as junonn_loss
import junonn_inputs_bundle as junonn_inputs
import junonn_train

FLAGS = tf.app.flags.FLAGS

tf.app.flags.DEFINE_boolean('use_fp16', False, """Train the model using fp16.""")
tf.app.flags.DEFINE_string('train_dir', r'G:/python/CNN-net/keep/',
                           """Directory where to write event logs """
                           """and checkpoint.""")                        
tf.app.flags.DEFINE_string('ckp_dir', r'G:/python/CNN-net/keep/',
                           """Directory where to load event logs """
                           """and checkpoint.""")
tf.app.flags.DEFINE_integer('max_steps', 100000,
                            """Number of batches to run.""")
tf.app.flags.DEFINE_boolean('log_device_placement', False,
                            """Whether to log device placement.""")

tf.app.flags.DEFINE_string('train_data', r'G:\python\CNN-net\data',
                           """Path to the CIFAR-10 data directory.""")
tf.app.flags.DEFINE_integer('batch_size', 1,
                            """Number of images to process in a batch.""")

def train():
    with tf.Graph().as_default():
        
        sess = tf.Session()
        
        global_step = tf.Variable(0, trainable=False)
       
        # Get images and labels for CIFAR-10.
        images,labels = junonn_inputs.inputs(eval_data=False, train_data = FLAGS.train_data)

        float_images = tf.cast(images,tf.float32)

        # Build a Graph that computes the logits predictions from the
        # inference model.
        logits = junonn_inference.inference(float_images)
        

        # Calculate loss.
        lossed,top_k_op = junonn_loss.loss(logits, labels)
        #print(tf.shape(lossed))

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
            
            
            #_, loss_value,precision = sess.run([train_op,lossed,top_k_op])
            _, loss_value,precision,logits_value,labels_value = sess.run([train_op,lossed,top_k_op,logits[0:1,],labels[0]])
            #precision=count/junonn_inputs.FLAGS.batch_size            
           
            duration = time.time() - start_time

            assert not np.isnan(loss_value), 'Model diverged with loss = NaN'
   
            if step % 2 == 0:
                num_examples_per_step = FLAGS.batch_size
                examples_per_sec = num_examples_per_step / duration
                sec_per_batch = float(duration)
                format_str = ('%s: step %d, loss = %.2f, precision=%.2f(%.1f examples/sec; %.3f '
                      'sec/batch)')
                print (format_str % (datetime.now(), step, loss_value,precision,
                             examples_per_sec, sec_per_batch))

            if step % 20 == 0:
                print(logits_value,labels_value)
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
    train()


if __name__ == '__main__':
    tf.app.run()
