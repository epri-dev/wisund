#!/bin/bash

freshen() {
    sudo apt-get -y upgrade
    sudo apt-get -y update
    sudo apt-get -y upgrade
}

install_required() {
    sudo apt-get -y install vim-gnome vim cmake libboost-all-dev telnet gcc g++ bison flex libasio-dev gawk git libcppunit-dev doxygen texlive-latex-base texlive-latex-extra texlive-font-utils graphviz
}

clone_or_refresh() {
    if [ -e /home/pi/wisund/.git/ ]; then
        cd /home/pi/wisund
        git pull
    else
        git clone https://github.com/epri-dev/wisund.git
    fi
}

build_and_install() {
    mkdir -p /home/pi/wisund/build
    cd /home/pi/wisund/build
    cmake ..
    make -j4
    sudo make install
    if grep -q "wisund" /etc/rc.local; then
        return 0
    else
        cat <<EOF > /etc/rc.local &&
#!/bin/sh -e
#
# rc.local
#
# This script is executed at the end of each multiuser runlevel.
# Make sure that the script will "exit 0" on success or any other
# value on error.
#
# In order to enable or disable this script just change the execution
# bits.
#
# By default this script does nothing.

# start Wi-SUN stack
/home/pi/wisund/src/rc.local &

# Print the IP address
_IP=$(hostname -I) || true
if [ "$_IP" ]; then
  printf "My IP address is %s\n" "$_IP"
fi

exit 0
EOF
        chmod +x /etc/rc.local
    fi
}

enable_uart() {
    sudo sed -i '/uart0=on/d' /boot/config.txt
    sudo raspi-config nonint do_serial 1
    sudo sed -i 's/enable_uart=0/uart0=on/' /boot/config.txt
}

showhelp() {
echo "Usage: buildall.sh freshen|install_required|clone_or_refresh|build_and_install|all|help

all     - do everything
help    - prints this text
freshen - does update/upgrade of Linux + all installed software
install_required - installs the required software for building everything in wisund
clone_or_refresh - uses git to either clone or refresh the wisund source code
build_and_install - builds and installs the wisund software
"
}

for i in $*
do
    case $i in
    all)
        freshen
        install_required
        clone_or_refresh
        build_and_install
        ;;
    freshen)
        freshen
        ;;
    install_required)
        install_required
        ;;
    clone_or_refresh)
        clone_or_refresh
        ;;
    build_and_install)
        build_and_install
        ;;
    help)
        showhelp
        ;;
    *)
        showhelp
        ;;
    esac
done
