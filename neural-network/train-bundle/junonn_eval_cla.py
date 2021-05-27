from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
from datetime import datetime
import math
import time
import tensorflow.python.platform
from tensorflow.python.platform import gfile
import numpy as np
import tensorflow as tf
import junonn_inputs_bundle
import junonn_inference_vgg16_bundle
import junonn_train
import junonn_loss_bundle as junonn_loss
FLAGS = tf.app.flags.FLAGS
tf.app.flags.DEFINE_boolean('use_fp16', False,"""Train the model using fp16.""")
tf.app.flags.DEFINE_string('eval_dir', r'G:/python/CNN-net/keep_eval/',"""Directory where to write event logs.""")
tf.app.flags.DEFINE_string('eval_data', r'G:\python\CNN-net\data',"""Either 'test' or 'train_eval'.""")
tf.app.flags.DEFINE_string('ckp_dir', r'G:/python/CNN-net/keep/',"""Directory where to read model checkpoints.""")
tf.app.flags.DEFINE_integer('eval_interval_secs', 30 ,"""How often to run the eval.""")
tf.app.flags.DEFINE_integer('num_examples', 640,"""Number of examples to run.""")
tf.app.flags.DEFINE_boolean('run_once', False,"""Whether to run eval only once.""")
tf.app.flags.DEFINE_integer('batch_size', 64,
                            """Number of images to process in a batch.""")
def eval_once(saver, summary_writer, summary_op,logits,labels):
    
    #gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.8)
    #config=tf.ConfigProto(gpu_options=gpu_options,intra_op_parallelism_threads = 4)
    with tf.Session() as sess:
        ckpt = tf.train.get_checkpoint_state(FLAGS.ckp_dir)
        if ckpt and ckpt.model_checkpoint_path:
            saver.restore(sess, ckpt.model_checkpoint_path)
            global_step = ckpt.model_checkpoint_path.split('/')[-1].split('-')[-1]            
        else:
            print('No checkpoint file found')
            return
        coord = tf.train.Coordinator()
        try:
            threads = []
            for qr in tf.get_collection(tf.GraphKeys.QUEUE_RUNNERS):
                threads.extend(qr.create_threads(sess, coord=coord, daemon=True,start=True))
            num_iter = int(math.ceil(FLAGS.num_examples / FLAGS.batch_size))
            true_count = 0  # Counts the number of correct predictions.
            total_sample_count = num_iter * FLAGS.batch_size
            step = 0
            #logits=sess.run(logits)
            #labels=sess.run(labels)
            labels=tf.reshape(labels,[-1])
            top_k_op = tf.nn.in_top_k(logits, labels , 1)
            
            while step < num_iter and not coord.should_stop():
                predictions,logits_value,labels_value = sess.run([top_k_op,logits[0:1,],labels[0]])
                print(logits_value)
                print(labels_value)
                true_count += np.sum(predictions)
                step += 1
            # Compute precision @ 1.

            precision = true_count / total_sample_count
            print('%s: precision @ 1 = %.3f' % (datetime.now(), precision))
            summary = tf.Summary()
            summary.ParseFromString(sess.run(summary_op))
            summary.value.add(tag='Precision @ 1', simple_value=precision)
            summary_writer.add_summary(summary, global_step)

        except Exception as e:  # pylint: disable=broad-except
            coord.request_stop(e)

        coord.request_stop()
        coord.join(threads, stop_grace_period_secs=10)


def evaluate():
    with tf.Graph().as_default():
        eval_data = FLAGS.eval_data
        images, labels = junonn_inputs_bundle.inputs(eval_data=eval_data,train_data=False)
        logits = junonn_inference_vgg16_bundle.inference(images)
        lossed,top_k_op = junonn_loss.loss(logits, labels)  
        
        variable_averages = tf.train.ExponentialMovingAverage(junonn_train.MOVING_AVERAGE_DECAY)
        variables_to_restore = variable_averages.variables_to_restore()
        saver = tf.train.Saver(variables_to_restore)
        # Build the summary operation based on the TF collection of Summaries.
        summary_op = tf.summary.merge_all()
        graph_def = tf.get_default_graph().as_graph_def()
        summary_writer = tf.summary.FileWriter(FLAGS.eval_dir,graph_def=graph_def)
        while True:
            eval_once(saver, summary_writer, summary_op,logits,labels)
            if FLAGS.run_once:
                break
            time.sleep(FLAGS.eval_interval_secs)
            
def main(argv=None):
    if gfile.Exists(FLAGS.eval_dir):
        gfile.DeleteRecursively(FLAGS.eval_dir)
    gfile.MakeDirs(FLAGS.eval_dir)
    
    
    evaluate()
if __name__ == '__main__':
    tf.app.run()
