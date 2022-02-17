from cnn.cnn import Network
import numpy as np
import matplotlib.pyplot as plt
import argparse

"""SFC project - practical classification task using BP
    Author: Natália Holková
    Login: xholko02
"""

parser = argparse.ArgumentParser(description='Train CNN')
parser.add_argument("-s", "--save_path", help="Path for saving the model", default="model.pkl")
parser.add_argument("-n", "--number_train", help="Number of training images", default=500)
parser.add_argument("-e", "--epochs", help="Number of epochs", default=3)
parser.add_argument("-b", "--batch_size", help="Batch size", default=32)

if __name__ == "__main__":
    args = parser.parse_args()

    CNN = Network()
    loss = CNN.train(num_images=int(args.number_train), epochs=int(args.epochs),
                     batch_size=int(args.batch_size), save_path=args.save_path)

    # Plot loss
    plt.plot(loss, 'r')
    plt.xlabel('# Iterations')
    plt.ylabel('Loss')
    plt.legend('Loss', loc='upper right')
    plt.show()
