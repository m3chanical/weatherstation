#! /bin/bash

INSTALL_DIR="~/esp"

if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$NAME
    VER=$VERSION_ID
else
    OS=$(uname -s)
    VER=$(uname -r)
fi

if [ $OS = "Ubuntu" ]; then
    echo
    echo "[ INSTALLING ESP-IDF ]"
    echo 

    mkdir -p $INSTALL_DIR 
    cd $INSTALL_DIR

    echo
    echo "[ CLONING LATEST ESP-IDF ]"
    echo

    # TODO: Check git return value
    git clone -b v4.0.1 --recursive https://github.com/espressif/esp-idf.git

    echo
    echo "[ INSTALLING DEPENDENCIES ]"
    echo 

    # TODO: check directory exists first

    cd esp-idf
    ./install.sh
    . ~/esp/esp-idf/export.sh
else
    echo "[!] Unsupported Linux release. Ubuntu only. [!]"
    exit 1
fi


