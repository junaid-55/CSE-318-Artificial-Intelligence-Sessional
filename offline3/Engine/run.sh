#!/bin/bash

g++ -O3 -Wall -shared -std=c++11 $(python -m pybind11 --includes) bindings.cpp -o Chain_reaction.pyd