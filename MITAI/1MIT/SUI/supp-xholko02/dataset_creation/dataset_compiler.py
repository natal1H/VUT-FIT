import numpy as np
import os
from argparse import ArgumentParser

"""
Authors:
Filip Bali, xbalif00
Natalia Holkova, xholko02
Roland Zitny, xzitny01
Vit Bartak, xbarta47
"""

parser = ArgumentParser(prog='SUI-dataset_compiler')
parser.add_argument('-d', '--dir', help="Dataset location", default="./dataset/")
parser.add_argument('-s', '--save_loc', help="Save location for compiled dataset", default="./dataset_compiled")
args = parser.parse_args()

DIR = args.dir
NEW_NAME = args.save_loc
LEN = 634

uninitialized = True

# Compiles all files into single file
for filename in os.listdir(DIR):
    if filename.endswith(".npy"):
        loc = os.path.join(DIR, filename)
        arr = np.load(loc)

        if uninitialized:
            final_arr = arr
            uninitialized = False
        else:
            final_arr = np.append(final_arr, arr, axis=0)

# save to new file
np.save(NEW_NAME, final_arr)

