#! /bin/sh

git submodule update --init --recursive --force  -- "extlibsb040/cairoQPaintDevice"
git submodule update --init --recursive --force  -- "extlibsb040/FCSSimulator"
git submodule update --init --recursive --force  -- "extlibsb040/JKQtPlotter"
git submodule update --init --recursive --force  -- "extlibsb040/libtiff_tools"
git submodule update --init --recursive --force  -- "extlibsb040/sdfflib"
git submodule update --init --recursive --force  -- "extlibsb040/StatisticsTools"
git submodule update --init --recursive --force  -- "extlibsb040/TinyMAT"
git submodule update --init --recursive --force  -- "extlibsb040/TinyTIFF"


pwd
cd ./extlibsb040/cairoQPaintDevice
pwd
git checkout master
git pull
cd ..
cd ..
pwd

cd ./extlibsb040/FCSSimulator
pwd
git checkout master
git pull
cd ..
cd ..
pwd

cd ./extlibsb040/JKQtPlotter
pwd
git checkout master
git pull
cd ..
cd ..
pwd

cd ./extlibsb040/libtiff_tools
pwd
git checkout master
git pull
cd ..
cd ..
pwd

cd ./extlibsb040/sdfflib
pwd
git checkout master
git pull
cd ..
cd ..
pwd

cd ./extlibsb040/StatisticsTools
pwd
git checkout master
git pull
cd ..
cd ..
pwd

cd ./extlibsb040/TinyMAT
pwd
git checkout master
git pull
cd ..
cd ..
pwd

cd ./extlibsb040/TinyTIFF
pwd
git checkout master
git pull
cd ..
cd ..
pwd
