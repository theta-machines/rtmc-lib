#!/usr/bin/env python3
#
# make.py
#
# Automatically build and test the project
#
# Usage:
#  * `./make.py` - build the library (rtmc_lib.a)
#  * `./make.py test` - build and run tests
#  * `./make.py clean` - remove the build directory
#

import os, sys, shutil

def build():
    os.system("cmake -S . -B build")
    os.system("cmake --build build")

def print_red(msg):
    RED_TEXT = "\x1b[31m"
    NORMAL_TEXT = "\x1b[0m"
    print(f"{RED_TEXT}{msg}{NORMAL_TEXT}")

args = sys.argv[1:]
if(args):
    if(args[0] == "clean"):
        shutil.rmtree("build")

    elif(args[0] == "test"):
        build()
        os.system("ctest --test-dir build")
    
    else:
        print_red(f"Error: unknown argument: '{args[0]}'")

else:
    build()
