# Getting Started
PoDoFo is a cross-platform low-level pdf library. NoPoDoFo is a nodejs bindings library to PoDoFo.
NoPoDoFo requires PoDoFo 0.9.6 (latest release). PoDoFo installation instructions for Windows and Linux follows.

## Build Dependencies

Note: Please use your platforms native package manager

## Windows


Building podofo on windows is made much simpler with the help of vcpkg. 
This build does not support AES encryption option, to enable AES you will need libidn v1 (which unfortunately is not available via vcpkg).
PoDoFo can use fontconfig, this build does not include fontconfig, what this means for you is that in order to use a font you must provide the
path to the font. Please see [Fonts](https://github.com/corymickelson/NoPoDoFo/tree/master/guides/font.md) for more information.
 - Install [vcpkg](https://github.com/Microsoft/vcpkg)
 - Run `vcpkg install podofo:x64-windows`

Building NoPoDoFo from source requires CMake.
 - Set environment variable `vcpkg_path` to the root of your vcpkg installation, ex: `set vcpkg_path=C:\\vcpkg`
 - run `git clone https://github.com/corymickelson/NoPoDoFo`
 - run `npm install`
 - run `./node_modules/.bin/cmake-js build`
 - run `./node_modules/.bin/tsc -p tsconfig.json`

NoPoDoFo also provides pre-built binaries. To install a pre-built in your npm project run `npm install https://github.com/corymickelson/NoPoDoFo/releases/download/{version}/nopodofo-{version}-win-x86_64.tar.gz`

## Linux

There are many linux distro's, but for this guide I will only be covering arch linux. Other distro's will have similar
installation instruction, if you are truely blocked on the installation of PoDoFo please create an issue.

dependencies:
 - openssl-dev
 - fontconfig-dev
 - libtiff-dev
 - boost
 - libidn-dev
 - libjpeg-turbo-dev
 - libpng-dev
 - freetype-dev
 - zlib-dev

Install dependencies with `pacman -S [dependencies]`

Install PoDoFo from source.
Below is a small bash script for easy installation.
```bash 
git clone https://github.com/svn2github/podofo $HOME/podofo && \
cd $HOME/podofo && \
mkdir $HOME/podofo/build && \
cd $HOME/podofo/build && \
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/  && \
make && make install
```

Building NoPoDoFo from source.
 - run `git clone https://github.com/corymickelson/NoPoDoFo`
 - run `npm install`
 - run `./node_modules/.bin/cmake-js build`
 - run `./node_modules/.bin/tsc -p tsconfig.json`

## Mac
todo

## Building NoPoDoFo

With all dependencies installled you can install NoPoDoFo with `npm i nopodofo`
