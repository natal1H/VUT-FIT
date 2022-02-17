from cnn.cnn import Network
from cnn.util import *
import numpy as np
import argparse
import pickle

"""SFC project - practical classification task using BP
    Author: Natália Holková
    Login: xholko02
"""

parser = argparse.ArgumentParser(description='Test CNN')
parser.add_argument("-m", "--model", help="Path to trained model", default="model.pkl")
parser.add_argument("-n", "--num_tests", help="Number of images to test", default=5)

if __name__ == "__main__":
    args = parser.parse_args()

    # Download and extract test MNIST data if not already done
    download_MNIST_zip()
    unzip_dataset()

    num_tests = int(args.num_tests)
    test_images = extract_data(file="mnist/t10k-images-idx3-ubyte.gz", num_images=num_tests)
    test_labels = extract_labels(file="mnist/t10k-labels-idx1-ubyte.gz", num_images=num_tests).reshape(num_tests, 1)

    test_data = np.hstack((test_images, test_labels))  # Stack together images and labels
    np.random.shuffle(test_data)  # Shuffle the data

    # Load the trained model
    infile = open(args.model, 'rb')
    params = pickle.load(infile)  # unpickle
    infile.close()
    CNN = Network()  # Load trained parameters into model
    CNN.load_params(params)

    correctly_predicted = 0

    for i in range(num_tests):
        print("Test {}:".format(i))
        image1D = test_data[i][0:-1]  # for predicting image nee
        image_cnn = image1D.reshape(1, 28, 28)  # Image needs different shape for plotting (depth, height, width)
        image_plot = image1D.reshape(28, 28, 1)  # Image needs different shape for plotting (height, width, depth)
        label = test_data[i][-1]

        # Let the model predict
        pred_class, pred_prob = CNN.predict(image_cnn)

        print("\t- expected label {}".format(int(label)))
        print("\t- predicted label {} with probability {:.2f}%".format(int(pred_class), pred_prob * 100))

        if int(label) == int(pred_class):
            correctly_predicted += 1

    # Print total accuracy
    print("--------")
    print("Accuracy on the test data: {:.2f}%".format(correctly_predicted / num_tests * 100))