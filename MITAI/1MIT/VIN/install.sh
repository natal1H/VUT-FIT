#!/bin/bash
virtualenv -p python3 VINeffects # create new virtual environment
source VINeffects/bin/activate # activate the environment
pip install -r requirements.txt # install python package requirements
