import tensorflow as tf
import numpy as np
FLAGS = tf.app.flags.FLAGS
def loss(logits, labels):
    labels=tf.cast(labels,tf.int32)
    #sparse_labels = tf.reshape(labels, [junonn_inputs.FLAGS.batch_size, 1])
    #indices = tf.reshape(tf.range(junonn_inputs.FLAGS.batch_size), [junonn_inputs.FLAGS.batch_size, 1])
    #print(tf.shape(indices))
    #concated = tf.concat([indices, sparse_labels],1)
    #dense_labels = tf.sparse_to_dense(concated,
    #                               [junonn_inputs.FLAGS.batch_size, NUM_CLASSES],
    #                                1.0, 0.0)
    #cross_entropy = tf.nn.softmax_cross_entropy_with_logits(
    #  logits=logits, labels=dense_labels ,name='cross_entropy_per_example')
    
    labels=tf.reshape(labels,[-1])
    
    cross_entropy = tf.nn.sparse_softmax_cross_entropy_with_logits(
        logits=logits, labels=labels ,name='cross_entropy_per_example')
    
    cross_entropy_mean = tf.reduce_mean(cross_entropy, name='cross_entropy')
    
    
    #print(tf.shape(logits)[0],tf.shape(dense_labels)[0])
    
    top_k_op = tf.nn.in_top_k(logits, labels, 1)
    top_num=tf.where(top_k_op)
    
    sm = tf.reshape(top_num,[-1])
    true_count=tf.shape(sm)[0]
    
    precious=tf.divide(true_count,FLAGS.batch_size) 
    precious = tf.convert_to_tensor(precious)
    #precision=tf.reshape(true_count,[-1])
    
    
    
    tf.add_to_collection('precision', true_count)
    tf.add_to_collection('losses', cross_entropy_mean)
    
    with tf.name_scope('eval'):
        with tf.name_scope('accuracy_rate') as scope:
            tf.summary.scalar('losses', cross_entropy_mean) 
            tf.summary.scalar('precision',precious)

    
    # The total loss is defined as the cross entropy loss plus all of the weight
    # decay terms (L2 loss).
    return tf.add_n(tf.get_collection('losses'), name='total_loss'),precious



