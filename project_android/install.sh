#!/bin/sh
rm -rf ./libs
rm -rf ./bin
rm -rf res/raw/*
make clean
make -j 8
make install
