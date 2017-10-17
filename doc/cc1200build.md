# Building the CC1200 firmware  {#cc1200build}
This document describes how to build the radio firmware associated with this project using TI's Code Composer Studio (CCS) version 6.1 or later.  Because TI's software is built to run under Windows, this portion of the code is built on Windows.  Windows 7 and Windows 10 have both been successfully used, but check TI's documentation for both the latest version and details on which operating systems and versions are supported.  The code is intended to be run on [TI's CC1200 Development Kit](http://www.ti.com/tool/cc1200dk).  The steps are:

  1. Install the tools
  2. Create the source tree
  3. Import the project
  4. Build and flash the software into the device

These steps are each described in detail below.

## Install the tools 
The first step is to install CCS version 6.1 or later.  This may be downloaded from [TI's web site](http://www.ti.com/tool/ccstudio).  The version to install is the one for the MSP430 processor.  Once CCS is installed, the code may be built using the following steps.

The setup also uses [cygwin](https://www.cygwin.com) to simplify the initial setup and configuration of the firmware project.

## Create the source tree 
There is a Cygwin script name `project` that is in the `tools/TI-cc1200EB-scripts` directory which can be used for creating the source tree.  Because CCS requires a particular layout, the files are typically copied from their locations in the repository to the working directory that CCS will use.  The `project` program uses a configuration file named `config` which should be located in the same directory from which `projects` is run.  There is a sample configuration file in the same directory, and if the `project` program is run without a `config` file existing, it will exit with a warning without doing anything else.  

`config.sample` file contents:

    # configuration file for the "project" script
    #
    # this is the name of the file containing the full list of project files
    FILELIST=filelist.txt

    # point this to the root directory of your local git repository
    # for example, if you were in the /home/smith directory and had executed
    # "git clone ..." to clone the project, the following value could be used:
    GITROOT="/home/smith/Open-Wi-SUN-Stack"

    # This points to the directory into which the project will be created
    # in Code Composer Studio.
    CCSROOT="/home/smith/wisun-project/TRxEB-FAN-1"

Generally, only the `GITROOT` and `CCSROOT` entries will need to be changed if `project` is run from within that same directory.  The `project` program has a few options:

echo
: lists all file pairs from GITROOT and CCSROOT

diff
: creates a patch file using GITROOT as source and CCSROOT as dest

listchanged
: lists files that would be copied by updateproject

newproject
: creates start of new project from GITROOT into CCSROOT

updateproject
: copies only changed files from GITROOT to CCSROOT

updategit
: copies only changed files from CCSROOT to GITROOT

Generally, one should execute `project newproject` just once to initialize the project.

## Import the project
Once the files have been copied, run CCS to import the project.  When CCS first starts, it asks for a *workspace* directory.  Choose the directory **above** the CCSROOT directory mentioned above.  For example, with the example config file line:

    CCSROOT="/home/smith/wisun-project/TRxEB-FAN-1"

The directory to choose is the Windows equivalent of the `/home/smith/wisun-project` directory.  If you're not sure of the mapping between the Cygwin and Windows directories, you can navigate to that directory in Cygwin and execute the command `cygpath -w $PWD` which might return somethine like

    C:\cygwin64\home\smith\wisun-project

Choose **Project --> Import CCS Projects...** to import the project.  When prompted, select the *same* directory as was chosen above (e.g. `C:\cygwin64\home\smith\wisun-project`).  CCS should automatically discover the project (named TRxEB-FAN-1 in the example above).  Make sure the "copy files into directory" box is unchecked and select the "Finish" button to import the project.  

## Build and flash the software into the device
With the USB-FET attached to the CC1200 development board and attached to the computer via USB, select **Run -> Debug** (or use the F11 key) to both compile the code and to load it into the CC1200 development board.

This starts the debugger, which gains control of the target, erases the target memory, programs the target memory with the application, and resets the target.  At this point the program may be run by pressing **F8**  which causes the program to run. Alternately you can use the icon on the command bar which looks like the play button on an old VCR.
