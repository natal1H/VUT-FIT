import numpy as np
from cnn.util import nanArgmax

"""SFC project - practical classification task using BP
    Author: Natália Holková
    Login: xholko02
"""

class ConvLayer:
    def __init__(self, stride=1):
        self.stride = stride

    def feedforward(self, image, filter, bias):
        """Feed forward the Convolution layer. Compute convolution of 'filter' over 'image'

        Args:
            image (numpy array): input image data

        Returns:
            numpy array: convolution matrixes
        """

        # Get dimensions
        (num_filt, _, filt_size, _) = filter.shape # Filter dimensions: Number of filters, image depth, filter size, filter size
        _, img_size, _ = image.shape  # Image dimensions: depth, size, size

        # Calculate output dimensions after convolution
        output_dim = int((img_size - filt_size) / self.stride) + 1

        # Prepare the output matrix 
        output = np.zeros((num_filt, output_dim, output_dim))

        # Perform convolution for each filter
        for current_filt in range(num_filt):
            y_index = out_y = 0

            # Move the filter vertically
            while y_index + filt_size <= img_size:
                x_index = out_x = 0
                
                # Move the filter horizontally
                while x_index + filt_size <= img_size:
                    # Perform the actual convolution operation and add bias
                    output[current_filt, out_y, out_x] = np.sum(filter[current_filt] * image[:, y_index: y_index + filt_size, x_index: x_index + filt_size])
                    output[current_filt, out_y, out_x] += bias[current_filt]
                    x_index += self.stride
                    out_x += 1
                y_index += self.stride
                out_y += 1

        return output  # final convolution matrix

    def backpropagate(self, dConv_prev, conv_in, filter):
        """Backpropagate the Convolution layer

        Args:
            dConv_prev (float): result of previous convolution
            conv_in (numpy array): Input into convolution layer

        Result:
            float: loss gradient in output
            float: loss gradient in filter
            float: loss gradient in bias
        """
        
        # Get the dimensions
        (num_filt, filt_depth, filt_size, _) = filter.shape  # Filter dimensions: Number of filters, depth, size, size
        (_, orig_dim, _) = conv_in.shape  # only one dimension important since it's a square

        # Initialize the derivatives to zeros
        dOutput = np.zeros(conv_in.shape) 
        dFilter = np.zeros(filter.shape)
        dBias = np.zeros((num_filt, 1))

        # Iterate over all filters
        for current_filt in range(num_filt):
            y_index = out_y = 0

            # Move vertically
            while y_index + filt_size <= orig_dim:
                x_index = out_x = 0

                # Move horizontally
                while x_index + filt_size <= orig_dim:
                    # Loss gradient of filter
                    dFilter[current_filt] += dConv_prev[current_filt, out_y, out_x] * conv_in[:, y_index: y_index + filt_size, x_index: x_index + filt_size]
                    # Loss gradient of the input to the convolution
                    dOutput[:, y_index: y_index + filt_size, x_index: x_index + filt_size] += dConv_prev[current_filt, out_y, out_x] * filter[current_filt]
                    x_index += self.stride
                    out_x += 1
                y_index += self.stride
                out_y += 1

            # Loss gradient of the bias
            dBias[current_filt] = np.sum(dConv_prev[current_filt])                

        return dOutput, dFilter, dBias


class MaxPoolLayer:
    def __init__(self, size=2, stride=2):
        self.size = size  # Size of the window we downsize
        self.stride = stride

    def feedforward(self, images):
        """Feed forward the MaxPool layer

        Args:
            images (numpy array): image data after convolutions

        Returns:
            numpy array: downsampled input data
        """
        # Get the dimensions
        num_images, img_height, img_width = images.shape  # Image dimensions: number of images (because image was passed through filters), height, width

        # Calculate the output dimensions
        output_height = int((img_height - self.size) / self.stride) + 1 
        output_width = int((img_width - self.size) / self.stride) + 1 

        # Prepare the output matrix
        output = np.zeros((num_images, output_height, output_width))

        # Perform the max pool over each filtered image
        for current_image in range(num_images):
            y_index = out_y = 0

            # Move the window vertically
            while y_index + self.size <= img_height:
                x_index = out_x = 0
                
                # Move the window horizontally
                while x_index + self.size <= img_width:
                    
                    # Choose the max value in the window
                    output[current_image, out_y, out_x] = np.max(images[current_image, y_index: y_index + self.size, x_index: x_index + self.size])
                    x_index += self.stride
                    out_x += 1
                y_index += self.stride
                out_y += 1
        
        return output

    def backpropagate(self, dPool, orig):
        """Backpropagate the MaxPool layer

        Args:
            dPool (float): previous result
            orig (numpy array): original input to max pool layer

        Returns:
            float: loss gradient of max pool layer
        """
        
        # Get dimensions
        (num_images, orig_dim, _) = orig.shape  # Dimensions: number of filtered images into MaxPool, size of image (it's square)

        # Initialize derivative to zeros
        dOutput = np.zeros(orig.shape)

        # Iterate over all original images
        for i in range(num_images):
            y_index = out_y = 0

            # Move vertically
            while y_index + self.size <= orig_dim:
                x_index = out_x = 0
            
                # Move horizontally
                while x_index + self.size <= orig_dim:
                    # Get index of largest value in the input of current window
                    (row, col) = nanArgmax(orig[i, y_index: y_index + self.size, x_index: x_index + self.size])
                    dOutput[i, y_index + row, x_index + col] = dPool[i, out_y, out_x]
                    x_index += self.stride
                    out_x += 1
                y_index += self.stride
                out_y += 1
        
        return dOutput


class FullyConnectedLayer:
    def __init__(self):
        pass

    def feedforward(self, pooled_image):
        """Feedforward the pooled image throught fully connected layer and flatten into 1D vector

        Args:
            pooled_image (numpy array): image after pooling

        Returns:
            vector: flattened pooled image
        """
        (num_pools, pool_size, _) = pooled_image.shape  # Dimension: number of pooled images from max pool, size of that pooled image, 3rd dim same as 2nd
        fully_connected = pooled_image.reshape((num_pools * pool_size * pool_size, 1)) # Flatten into 1D and return
        return fully_connected
