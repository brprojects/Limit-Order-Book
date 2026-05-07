#!/bin/bash

mkdir build
cd build
cmake ..
make
cp LimitOrderBook ../
cp GenerateOrders ../
cp ./test/LimitOrderBookTests ../
