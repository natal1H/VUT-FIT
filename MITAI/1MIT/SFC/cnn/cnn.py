import numpy as np
import pickle
from cnn.layers import ConvLayer, MaxPoolLayer, FullyConnectedLayer
from cnn.util import *
from tqdm import tqdm

"""SFC project - practical classification task using BP
    Author: Natália Holková
    Login: xholko02
"""

class Network:
    def __init__(self):
        # Hyper parameters
        self.learningRate = 0.01
        self.beta1 = 0.95
        self.beta2 = 0.99
    
        # Parameters
        self.filter1 = initialize_filter((8, 1, 5, 5))  # (number of filters 1, image depth, filter size, filter size)
        self.filter2 = initialize_filter((8, 8, 5, 5))  # (number of filters 1, number of filters 2, filter size, filter size)
        self.weights3 = initialize_weights((128, 800))
        self.weights4 = initialize_weights((10, 128))

        self.bias1 = np.zeros((self.filter1.shape[0], 1))
        self.bias2 = np.zeros((self.filter2.shape[0], 1))
        self.bias3 = np.zeros((self.weights3.shape[0], 1))
        self.bias4 = np.zeros((self.weights4.shape[0], 1))

        self.numClasses = 10

        # Initialize the layers
        self.convLayer1 = ConvLayer(stride=1)  # 1st hidden layer: Convolution layer
        self.convLayer2 = ConvLayer(stride=1)  # 2nd hidden layer: Convolution layer
        self.poolLayer = MaxPoolLayer(size=2, stride=2)  # 3rd hidden layer: Max pooling layer
        self.fcLayer = FullyConnectedLayer()  # 4th hidden layer: Fully connected layer

    def load_params(self, params):
        """Load model with trained parameters

        Args:
            params: [filter1, filter2, weights3, weights4, bias1, bias2, bias3, bias4]
        """
        # Load the parameters
        [filter1, filter2, weights3, weights4, bias1, bias2, bias3, bias4] = params
        self.filter1 = filter1
        self.filter2 = filter2
        self.weights3 = weights3
        self.weights4 = weights4
        self.bias1 = bias1
        self.bias2 = bias2
        self.bias3 = bias3
        self.bias4 = bias4

    def compute(self, image, label):
        """Backpropagation algorithm. Feedforward image through whole network, compute loss and then backpropagate 
        loss back through the network to compute gradients used for updating

        Args:
            image (numpy array): single image
            label (vector): one-hot encoded label
        """
        ## Forward operations
        conv1 = self.convLayer1.feedforward(image, filter=self.filter1, bias=self.bias1)
        conv1[conv1 <= 0] = 0  # apply ReLU on result
        
        conv2 = self.convLayer2.feedforward(conv1, filter=self.filter2, bias=self.bias2)
        conv2[conv2 <= 0] = 0  # apply ReLU on result
        
        pooled = self.poolLayer.feedforward(conv2)

        fc = self.fcLayer.feedforward(pooled)  # Flattens

        z = self.weights3.dot(fc) + self.bias3  # first dense layer
        z[z <= 0] = 0  # pass through ReLU non-linearity
        
        output = self.weights4.dot(z) + self.bias4  # second dense layer
        
        probs = softmax(output)  # predict class probabilities with the softmax activation function
        
        ## Calculate loss
        loss = categorical_cross_entropy(probs, label)

        ## Backward operations - backward order
        dOutput = probs - label  # Derivative of the loss
        dWeights4 = dOutput.dot(z.T)  # Loss gradient of final dense layer weights
        dBias4 = np.sum(dOutput, axis=1).reshape(self.bias4.shape)  # Loss gradient of final dense layer biases

        dz = self.weights4.T.dot(dOutput)  # Loss gradient of first dense layer outputs
        dz[z <= 0] = 0  # Backpropagate through ReLU
        dWeights3 = dz.dot(fc.T)
        dBias3 = np.sum(dz, axis=1).reshape(self.bias3.shape)

        dFc = self.weights3.T.dot(dz)  # Loss gradients of Fully connected layer
        dPool = dFc.reshape(pooled.shape)  # Reshape to match the pooling layer

        dConv2 = self.poolLayer.backpropagate(dPool, conv2)  # Backpropagate through the max pool layer
        dConv2[conv2 <= 0] = 0  # Backpropagate through ReLU

        dConv1, dFilter2, dBias2 = self.convLayer2.backpropagate(dConv2, conv1, self.filter2)  # Backpropagate through 2nd conv layer
        dConv1[conv1 <= 0] = 0  # Backpropagate through ReLU
    
        dImage, dFilter1, dBias1 = self.convLayer1.backpropagate(dConv1, image, self.filter1)  # Backpropagate through 1st conv layer.
    
        gradients = [dFilter1, dFilter2, dWeights3, dWeights4, dBias1, dBias2, dBias3, dBias4] 

        return gradients, loss
    
    def adamGD(self, batch, dim, n_c, cost):
        """Adam Gradient Descent

        Args:
            batch (int): size of batch to process
            dim (tuple): dimensions
            n_c (int): image depth
            cost (float): loss value

        Returns:
            array: cost array with added value for this run
        """
        
        X = batch[:, 0:-1]  # Get batch inputs
        X = X.reshape(len(batch), n_c, dim, dim)
        Y = batch[:, -1]  # Get batch labels

        cost_ = 0
        batch_size = len(batch)

        # Initialize gradients and momentum, RMS params
        dFilter1 = np.zeros(self.filter1.shape)
        dFilter2 = np.zeros(self.filter2.shape)
        dWeights3 = np.zeros(self.weights3.shape)
        dWeights4 = np.zeros(self.weights4.shape)
        dBias1 = np.zeros(self.bias1.shape)
        dBias2 = np.zeros(self.bias2.shape)
        dBias3 = np.zeros(self.bias3.shape)
        dBias4 = np.zeros(self.bias4.shape)

        v1 = np.zeros(self.filter1.shape)
        v2 = np.zeros(self.filter2.shape)
        v3 = np.zeros(self.weights3.shape)
        v4 = np.zeros(self.weights4.shape)
        bv1 = np.zeros(self.bias1.shape)
        bv2 = np.zeros(self.bias2.shape)
        bv3 = np.zeros(self.bias3.shape)
        bv4 = np.zeros(self.bias4.shape)

        s1 = np.zeros(self.filter1.shape)
        s2 = np.zeros(self.filter2.shape)
        s3 = np.zeros(self.weights3.shape)
        s4 = np.zeros(self.weights4.shape)
        bs1 = np.zeros(self.bias1.shape)
        bs2 = np.zeros(self.bias2.shape)
        bs3 = np.zeros(self.bias3.shape)
        bs4 = np.zeros(self.bias4.shape)

        # Iterate over all images in batch
        for i in range(batch_size):
        
            x = X[i] # Get current image
            y = np.eye(self.numClasses)[int(Y[i])].reshape(self.numClasses, 1)  # Get label as one-hot encoding
            
            # Collect Gradients for training example
            gradients, loss = self.compute(x, y)
            [dFilter1_, dFilter2_, dWeights3_, dWeights4_, dBias1_, dBias2_, dBias3_, dBias4_] = gradients
            
            dFilter1 += dFilter1_
            dBias1 += dBias1_
            dFilter2 += dFilter2_
            dBias2 += dBias2_
            dWeights3 += dWeights3_
            dBias3 += dBias3_
            dWeights4 += dWeights4_
            dBias4 += dBias4_

            cost_ += loss

        # Update parameters
        v1 = self.beta1 * v1 + (1 - self.beta1) * dFilter1 / batch_size # Momentum
        s1 = self.beta2 * s1 + (1 - self.beta2) * (dFilter1 / batch_size)**2 # RMSProp
        self.filter1 -= self.learningRate * v1 / np.sqrt(s1 + 1e-7) # Combine momentum and RMSProp
        
        bv1 = self.beta1 * bv1 + (1 - self.beta1) * dBias1 / batch_size
        bs1 = self.beta2 * bs1 + (1 - self.beta2) * (dBias1 / batch_size)**2
        self.bias1 -= self.learningRate * bv1 / np.sqrt(bs1 + 1e-7)

        v2 = self.beta1 * v2 + (1 - self.beta1) * dFilter2 / batch_size
        s2 = self.beta2 * s2 + (1 - self.beta2) * (dFilter2 / batch_size)**2
        self.filter2 -= self.learningRate * v2 / np.sqrt(s2 + 1e-7)

        bv2 = self.beta1 * bv2 + (1 - self.beta1) * dBias2 / batch_size
        bs2 = self.beta2 * bs2 + (1 - self.beta2) * (dBias2 / batch_size)**2
        self.bias2 -= self.learningRate * bv2 / np.sqrt(bs2 + 1e-7)

        v3 = self.beta1 * v3 + (1 - self.beta1) * dWeights3 / batch_size
        s3 = self.beta2 * s3 + (1 - self.beta2) * (dWeights3 / batch_size)**2
        self.weights3 -= self.learningRate * v3 / np.sqrt(s3 + 1e-7)

        bv3 = self.beta1 * bv3 + (1 - self.beta1) * dBias3 / batch_size
        bs3 = self.beta2 * bs3 + (1 - self.beta2) * (dBias3 / batch_size)**2
        self.bias3 -= self.learningRate * bv3 / np.sqrt(bs3 + 1e-7)

        v4 = self.beta1 * v4 + (1 - self.beta1) * dWeights4 / batch_size
        s4 = self.beta2 * s4 + (1 - self.beta2) * (dWeights4 / batch_size)**2
        self.weights4 -= self.learningRate * v4 / np.sqrt(s4 + 1e-7)

        bv4 = self.beta1 * bv4 + (1 - self.beta1) * dBias4 / batch_size
        bs4 = self.beta2 * bs4 + (1 - self.beta2) * (dBias4 / batch_size)**2
        self.bias4 -= self.learningRate * bv4 / np.sqrt(bs4 + 1e-7)

        cost_ = cost_ / batch_size
        cost.append(cost_)
 
        return cost

    def train(self, img_dim=28, img_depth=1, num_images=500, batch_size=32, epochs=3, save_path='model.pkl'):
        """Train the CNN

        Args:
            img_dim: image dimensions
            img_depth: image depth
            num_images: total number of input images
            batch_size: batch size
            epochs: number of epochs
            save_path: path for saving model

        Returns:
            loss value for each iteration
        """
        # Get training data
        download_MNIST_zip()
        unzip_dataset()

        X = extract_data(file="mnist/train-images-idx3-ubyte.gz", num_images=num_images)
        y = extract_labels(file="mnist/train-labels-idx1-ubyte.gz", num_images=num_images).reshape(num_images, 1)

        # Normalize images
        X = normalize_images(X)

        train_data = np.hstack((X, y))  # Stack together images and labels
        np.random.shuffle(train_data)  # Shuffle the data

        loss = []

        print("Learning rate: {}, batch size: {}".format(self.learningRate, batch_size))

        for epoch in range(epochs):
            print("Epoch {}:".format(epoch + 1))
            np.random.shuffle(train_data)
            batches = [train_data[k: k + batch_size] for k in range(0, train_data.shape[0], batch_size)]

            t = tqdm(batches)  # Progress bar
            for x, batch in enumerate(t):
                loss = self.adamGD(batch, img_dim, img_depth, loss)
                t.set_description("\tLoss: %.2f" % (loss[-1]))

        params = [self.filter1, self.filter2, self.weights3, self.weights4, self.bias1, self.bias2, self.bias3, self.bias4]
        with open(save_path, 'wb') as file:
            pickle.dump(params, file)

        return loss

    def predict(self, image):
        """Predict class for provided image

        Returns:
            Predicted class and its probability
        """
        conv1 = self.convLayer1.feedforward(image, self.filter1, self.bias1)  # 1st convolution
        conv1[conv1 <= 0] = 0  # ReLU

        conv2 = self.convLayer2.feedforward(conv1, self.filter2, self.bias2)  # 2nd convolution
        conv2[conv2 <= 0] = 0  # ReLU

        pooled = self.poolLayer.feedforward(conv2)  # Max pooling

        fc = self.fcLayer.feedforward(pooled)  # Flattens

        z = self.weights3.dot(fc) + self.bias3  # 1st dense layer
        z[z <= 0] = 0  # ReLU

        output = self.weights4.dot(z) + self.bias4  # 2nd dense layer

        probs = softmax(output)  # predict class probabilities with the softmax activation function

        return np.argmax(probs), np.max(probs)
