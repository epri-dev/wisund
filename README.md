# wisund
This project is code that runs on a Raspberry Pi and communicates with the EPRI Open-Wi-SUN-Stack.  The Open-Wi-SUN-Stack is a separate project that is an open source reference implementation of the [Wi-SUN Alliance](https://www.wi-sun.org) RF mesh protocol, based on IEEE 802.15.4g.  EPRI's implementation includes a serial port running at 115200 bps to allow programs such as this one to communicate to and through the radio.  Communication to the radio is used to set parameters and control the radio and to interrogate the stack for diagnostic information.  Communication through the radio allows virtually any IPv6 traffic to be sent to other Wi-SUN compliant radios.

## Building the software on the Raspberry Pi
This page describes compiling and installing this software on a Raspberry Pi from source.

### Preparing the Pi

First, obtain and install a Raspberry Pi image on a suitable microSD card.  Instructions and download images are available via <https://www.raspberrypi.org/downloads/raspbian/>.  

Insert the resulting microSD card into the Raspberry Pi power it up.  

Run `raspi-config` as root (`sudo raspi-config`) and do the following things:

under "Advanced Options", choose "SSH" and enable the SSH Server.  Next, also under "Advanced Options", "Serial" disable the login shell via serial.  Optionally, you may also wish to set the locale and expand the filesystem but neither of these are strictly necessary.

When you have completed these selections, finish and (if prompted) reboot.  For versions of the Raspbian operating system that are based on images before 2017, edit the file `/boot/config.txt` to add the following line:

    device_tree=

If there is a line that says either `enable_uart=0` or `enable_uart=1`, either delete the line or comment it out by adding a `#` character to the front of the line.

Images that are 2017 or newer only need `uart0=on`.

### Installing the tools

With the Pi connected to the internet, execute the following commands to update the Pi's software and to install required packages on the Pi:

    sudo raspi-config
    sudo apt-get upgrade
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get install vim-gnome vim cmake libboost-all-dev
    sudo apt-get install gcc g++ bison flex libasio-dev
    
If the unit tests are also desired, this command will also be needed:

    sudo apt-get install libcppunit-dev 

To build the documentation in both HTML and PDF formats, the following is also needed:

    sudo apt-get install doxygen texlive-latex-base 
    sudo apt-get texlive-latex-extra texlive-font-utils graphviz

Note that with the versions of Doxygen and Raspbian available at the moment (December 2016), there is a [problem with Doxygen's support of sqlite3](https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=758975) that manifests itself as a large number of warnings when generating the documentation, as well as a sqlite database file which grows without bound.

### Cloning the repository
The repository for this source code may be cloned directly onto the Pi:

    git clone git@github.com:epri-dev/wisund.git

This will create a directory `wisun` and copy all of the source code there.  

### Running CMake 
Navigate into that directory (`cd wisund`) and create a new subdirectory called `build` (`mkdir build`) and then navigate into that directory (`cd build`) to run CMake.  If you wish to compile the unit tests, the command is this:

    cmake -DCMAKE_BUILD_TYPE=Release -DWITH_TEST=1 ..

If you don't want the unit tests, use this:

    cmake -DCMAKE_BUILD_TYPE=Release ..

That will identify the locations and versions of all of the compiler and build tools and create a `Makefile`.  

### Building the software
Once CMake has created the `Makefile`, we run `make` to build the software.  There are a few options that we can use, but the simplest way to build all software is to simply run `make` with no command line parameters:

    make 

To speed things up, you can ask make to build using multiple jobs simultaneously.

    make -j8

This creates 8 jobs that run in parallel, speeding up the process.

### Other targets
In addition to building the software, we can also run unit tests and build the extensive documentation in both HTML and PDF formats.  To run unit tests (if `-DWITH_TEST=1` was specified when CMake was invoked), use the following:

    make -j8 test

This will run the various test suites, each containing multiple individual unit tests and report the results which should say that 100% of the tests passed. Note that the tests must be built first, which is part of the overall software build process as described in the previous section.

To build the documentation, run:

    make doc   

That builds the HTML version of the documentation only.  If you wish to also make the PDF version of the documentation, use this:

    make pdf

If you wish to create both, `make pdf` will build both the HTML and PDF versions.  The HTML version is in the relative path `doc/html` and a good starting place is `index.html` in that directory.  The PDF version of the documentation is created in `doc/pdf/` and is named `refman.pdf`.

Finally, the software can also be installed on the Pi by using this:

    sudo make install

Note that `root` privileges are required to install the software, hence to use of `sudo`.  There are other possible options to build, most of which are subsets of what has already been shown.  These can all be listed by running `make help`. 
