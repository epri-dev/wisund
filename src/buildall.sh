#!/bin/bash
sudo apt-get -y upgrade
sudo apt-get -y update
sudo apt-get -y upgrade
sudo apt-get -y install vim-gnome vim cmake libboost-all-dev telnet gcc g++ bison flex libasio-dev gawk git libcppunit-dev doxygen texlive-latex-base texlive-latex-extra texlive-font-utils graphviz
git clone https://github.com/epri-dev/wisund.git
cd wisund
mkdir build
cd build
cmake ..
make -j4
sudo make install
sudo raspi-config nonint do_serial 1
sudo raspi-config nonint set_config_var enable_uart 1 /boot/config.txt
