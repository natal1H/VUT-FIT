import numpy as np
import requests
import os
import gzip
import zipfile

"""SFC project - practical classification task using BP
    Author: Natália Holková
    Login: xholko02
"""

## Data preparation

def download_MNIST_zip(save_path="mnist.zip"):
    """Downloads the MNIST dataset for training the CNN

    Args:
        save_path (str, optional): Location for storing the dataset zip. Defaults to "mnist.zip".
    """
    if not os.path.isfile(save_path):
        # Download the zip file 
        r = requests.get("https://data.deepai.org/mnist.zip", stream=True)
        with open(save_path, 'wb') as fd:
            for chunk in r.iter_content(chunk_size=128):
                fd.write(chunk)

def unzip_dataset(filename="mnist.zip", dir="mnist"):
    """Unzips downloaded MNIST dataset

    Args:
        filename (str, optional): downloaded dataset location. Defaults to "mnist.zip".
        dir (str, optional): name of directory where to extract. Defaults to "mnist".
    """
    if not os.path.exists(dir):
        with zipfile.ZipFile(filename,"r") as zip_ref:
            zip_ref.extractall(dir)

def extract_data(file, num_images=500, image_width=28):
    """Extracts image data from gz file into numpy array

    Args:
        file (str): file location.
        num_images (int, optional): number of images to extract. Defaults to 500.
        image_width (int, optional): image size. Defaults to 28.

    Returns:
        numpy array: array containing extracted images
    """
    with gzip.open(file) as bytestream:
        bytestream.read(16)
        buffer = bytestream.read(image_width * image_width * num_images)
        data = np.frombuffer(buffer, dtype=np.uint8).astype(np.float32)
        data = data.reshape(num_images, image_width * image_width)

    return data

def extract_labels(file, num_images=500):
    """Extracts labels from gz file into numpy array

    Args:
        file (str): file location.
        num_images (int, optional): number of labels to extract. Defaults to 500.

    Returns:
        np array: array containing extracted labels
    """
    with gzip.open(file) as bytestream:
        bytestream.read(8)
        buffer = bytestream.read(1 * num_images)
        labels = np.frombuffer(buffer, dtype=np.uint8).astype(np.int64)

    return labels

## Activation function

def softmax(x):
    """Softmax activation function

    Args:
        x: array of input values

    Returns: probabilities between 0 and 1

    """
    f = np.exp(x - np.max(x))  # shift values to avoid blow out
    return f / f.sum(axis=0)

## Function for loss

def categorical_cross_entropy(y_pred, y_label):
    """Categorical cross entropy loss - used for classification

    Args:
        y_pred (int array): Predicted y values
        y_label (int array): Actual y values

    Returns:
        float: total loss
    """
    return -np.sum(y_label * np.log(y_pred))


## Functions for initializations

def initialize_filter(shape, scale=1.0):
    """Initialize filter to normal distribution

    Args:
        shape (tuple): filter shape
        scale (float, optional): scale factor. Defaults to 1.0.

    Returns:
        numpy array: Initialized filter
    """
    standart_deviation = scale/np.sqrt(np.prod(shape))
    return np.random.normal(loc=0, scale=standart_deviation, size=shape)

def initialize_weights(shape):
    """Initialize weights

    Args:
        shape (tuple): weights shape

    Returns:
        numpy array: Initialized weights
    """
    return np.random.standard_normal(size=shape) * 0.01

## Other util functions

def nanArgmax(array):
    """Get biggest value (non-Nan) index in an array

    Args:
        array (array): array of value

    Returns:
        int array: indeces of biggest values
    """
    index = np.nanargmax(array)
    indeces = np.unravel_index(index, array.shape)
    return indeces

def normalize_images(images):
    """Normalized images with mean and standart deviation of whole training dataset

    Args:
        images: input images to normalize

    Returns: 
        numpy array: images normalized by mean and std
    """
    images -= int(np.mean(images))
    images /= int(np.std(images))
    return images