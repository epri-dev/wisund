# Building the software on the Raspberry Pi  {#pibuild}
This page describes both the installation of the software on a Raspberry Pi if the installer has already been created, and also detailed instructions on how to build it from scratch on the Pi.

## Installing the software 

First, obtain and install a Raspberry Pi image on a suitable microSD card.  Instructions and download images are available via <https://www.raspberrypi.org/downloads/raspbian/>.  

Insert the resulting microSD card into the Raspberry Pi power it up.  

Run `raspi-config` as root (`sudo raspi-config`) and do the following things:

under "Advanced Options", choose "SSH" and enable the SSH Server.  Next, also under "Advanced Options", "Serial" disable the login shell via serial.  Optionally, you may also wish to set the locale and expand the filesystem but neither of these are strictly necessary.

When you have completed these selections, finish and (if prompted) reboot.  For versions of the Raspbian operating system that are based on images before 2017, edit the file `/boot/config.txt` to add the following line:

    device_tree=

If there is a line that says either `enable_uart=0` or `enable_uart=1`, either delete the line or comment it out by adding a `#` character to the front of the line.

Images that are 2017 or newer only need `uart0=on`.

Now reboot the Pi and copy the `testmode-0.1.1-Linux.sh` file onto the Pi via `ssh` or other means. Execute the shell script as root:

    sudo ./testmode-0.1.1-Linux.sh

Accept the license and the default installation location and the executables will be installed in `testmode-0.1.1-Linux` in a `bin` directory.

## Building the software 

Install the following packages

    sudo raspi-config
    sudo apt-get upgrade
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get install vim-gnome vim cmake libboost-all-dev
    sudo apt-get install gcc-4.8 g++-4.8 bison flex
    sudo apt-get install libasio-dev 
    
For the unit tests:

    sudo apt-get install libcppunit-dev 

For the documentation:

    sudo apt-get install doxygen texlive-font-utils graphviz

Note that with the versions of Doxygen and Raspbian available at the moment (December 2016), there is a [problem with Doxygen's support of sqlite3](https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=758975) that manifests itself as a large number of warnings when generating the documentation, as well as a sqlite database file which grows without bound.
