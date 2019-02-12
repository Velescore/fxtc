![Veles-Logo](https://i.imgur.com/OP0aW7y.jpg?3)

Veles Core integration/staging tree
======================================
![Licence](https://img.shields.io/github/license/Velescore/Veles.svg?style=flat)   ![Latest Release](https://img.shields.io/github/release/Velescore/Veles.svg?style=flat) ![Master Build Status](https://api.travis-ci.com/Velescore/Veles.svg?branch=master)    ![Development Version](https://img.shields.io/github/tag-pre/Velescore/Veles.svg?colorB=blue&label=dev&logo=github&style=flat) ![Development Build Status](https://img.shields.io/travis/com/Velescore/Veles/development.svg?logo=github&style=flat)

Official repository for Veles Core node and wallet. Current version of Veles Core is **v0.17.0 "Aged Amnesia"**. Default branch **master** contains source code for the latest stable release of Veles Core. Builds from the **development** branch are not recommended for mining or production environment. 

https://veles.network

Linux Build Instructions and Notes
==================================

Dependencies
----------------------
1.  Update packages

        sudo apt-get update

2.  Install required packagages

        sudo apt-get install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils libboost-all-dev

3.  Install Berkeley DB 4.8

        sudo apt-get install software-properties-common
        sudo add-apt-repository ppa:bitcoin/bitcoin
        sudo apt-get update
        sudo apt-get install libdb4.8-dev libdb4.8++-dev

4.  Install QT 5

        sudo apt-get install libminiupnpc-dev libzmq3-dev
        sudo apt-get install libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev protobuf-compiler libqrencode-dev

Build
----------------------
1.  Clone the source:

        git clone https://github.com/Velescore/Veles

2.  Build Veles Core:

    Configure and build the headless Veles Core binaries as well as the GUI (if Qt is found).

    You can disable the GUI build by passing `--without-gui` to configure.
        
        ./autogen.sh
        ./configure
        make

3.  It is recommended to build and run the unit tests:

        make check


Mac OS X Build Instructions and Notes
=====================================
See (doc/build-osx.md) for instructions on building on Mac OS X.



Windows (64/32 bit) Build Instructions and Notes
=====================================
See (doc/build-windows.md) for instructions on building on Windows 64/32 bit.
