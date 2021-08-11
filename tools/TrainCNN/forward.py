import tensorflow as tf


def get_weight(shape, regularizer=None):
    w = tf.Variable(tf.truncated_normal(shape, stddev=0.1))
    if regularizer is not None:
        tf.add_to_collection('losses', tf.contrib.layers.l2_regularizer(regularizer)(w))
    return w


def get_bias(shape):
    b = tf.Variable(tf.zeros(shape))
    return b


def conv2d(x, w):
    return tf.nn.conv2d(x, w, strides=[1, 1, 1, 1], padding="VALID")


def avg_pool_2x2(x):
    return tf.nn.avg_pool(x, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding="VALID")


def max_pool_2x2(x):
    return tf.nn.max_pool(x, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding="VALID")


# 第一层卷积核大小
CONV1_KERNAL_SIZE = 5

# 第一层卷积输出通道数
CONV1_OUTPUT_CHANNELS = 6

# 第二层卷积核大小
CONV2_KERNAL_SIZE = 3

# 第二层卷积输出通道数
CONV2_OUTPUT_CHANNELS = 10

# 第三层卷积核大小
CONV3_KERNAL_SIZE = 3

# 第三层卷积输出通道数
CONV3_OUTPUT_CHANNELS = 14

# 第一层全连接宽度
FC1_OUTPUT_NODES = 60

# 第二层全连接宽度（输出标签类型数）
FC2_OUTPUT_NODES = 15

# 输出标签类型数
OUTPUT_NODES = FC2_OUTPUT_NODES


def forward(x, regularizer=None, keep_rate=tf.constant(1.0)):
    vars = []
    vars_name = []
    nodes = []

    conv1_w = get_weight(
        [CONV1_KERNAL_SIZE, CONV1_KERNAL_SIZE, int(x.shape[3]), CONV1_OUTPUT_CHANNELS]
    )
    conv1_b = get_bias([CONV1_OUTPUT_CHANNELS])
    conv1 = tf.nn.relu(tf.nn.bias_add(conv2d(x, conv1_w), conv1_b))
    pool1 = avg_pool_2x2(conv1)
    print("conv1: ", conv1.shape)
    print("pool1: ", pool1.shape)
    vars.extend([conv1_w, conv1_b])
    vars_name.extend(["conv1_w", "conv1_b"])
    nodes.extend([conv1, pool1])

    conv2_w = get_weight(
        [CONV2_KERNAL_SIZE, CONV2_KERNAL_SIZE, CONV1_OUTPUT_CHANNELS, CONV2_OUTPUT_CHANNELS]
    )
    conv2_b = get_bias([CONV2_OUTPUT_CHANNELS])
    conv2 = tf.nn.relu(tf.nn.bias_add(conv2d(pool1, conv2_w), conv2_b))
    pool2 = avg_pool_2x2(conv2)
    print("conv2: ", conv2.shape)
    vars.extend([conv2_w, conv2_b])
    vars_name.extend(["conv2_w", "conv2_b"])
    nodes.extend([conv2, pool2])

    conv3_w = get_weight(
        [CONV3_KERNAL_SIZE, CONV3_KERNAL_SIZE, CONV2_OUTPUT_CHANNELS, CONV3_OUTPUT_CHANNELS]
    )
    conv3_b = get_bias([CONV3_OUTPUT_CHANNELS])
    conv3 = tf.nn.relu(tf.nn.bias_add(conv2d(pool2, conv3_w), conv3_b))
    print("conv3: ", conv3.shape)
    vars.extend([conv3_w, conv3_b])
    vars_name.extend(["conv3_w", "conv3_b"])
    nodes.extend([conv3])

    conv_shape = conv3.get_shape().as_list()
    node = conv_shape[1] * conv_shape[2] * conv_shape[3]
    reshaped = tf.reshape(conv3, [-1, node])
    reshaped = tf.nn.dropout(reshaped, keep_rate)
    print("reshaped: ", reshaped.shape)

    fc1_w = get_weight([node, FC1_OUTPUT_NODES], regularizer)
    fc1_b = get_bias([FC1_OUTPUT_NODES])
    fc1 = tf.nn.relu(tf.matmul(reshaped, fc1_w) + fc1_b)
    vars.extend([fc1_w, fc1_b])
    vars_name.extend(["fc1_w", "fc1_b"])
    nodes.extend([fc1])

    fc2_w = get_weight([FC1_OUTPUT_NODES, FC2_OUTPUT_NODES], regularizer)
    fc2_b = get_bias([FC2_OUTPUT_NODES])
    fc2 = tf.matmul(fc1, fc2_w) + fc2_b
    vars.extend([fc2_w, fc2_b])
    vars_name.extend(["fc2_w", "fc2_b"])
    nodes.extend([fc2])

    return nodes, vars, vars_name
