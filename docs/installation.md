# Installation

Requirements:

 - PoDoFo >= 0.9.6
 - CMake >= 3
 - Nodejs >= 8

The recommended installation process is as follows:

## Windows

<mark>Windows support is still a work in progress</mark>

Please install [vcpkg](https://github.com/Microsoft/vcpkg) as your package manager for building and installing PoDoFo.
To install PoDoFo with vcpkg run `vcpkg install podofo:x64-windows`[^1]. NoPoDoFo requires the path to vcpkg root directory as environment variable `vcpkg_path`.
Building NoPoDoFo from source requires CMake.
 - Set environment variable `vcpkg_path` to the root of your vcpkg installation, ex: `set vcpkg_path=C:\\vcpkg`
 - run `git clone https://github.com/corymickelson/NoPoDoFo`
 - run `npm install`
 - run `./node_modules/.bin/cmake-js build`
 - run `./node_modules/.bin/tsc -p tsconfig.json`

NoPoDoFo also provides pre-built binaries for Windows.
To install a pre-built in your npm project run `npm install https://github.com/corymickelson/NoPoDoFo/releases/download/{version}/nopodofo-{version}-win-x86_64.tar.gz`

## Linux

Please use your native package manager, the following is for arch (pacman). Other distro's will have similar
installation instruction, if you are truly blocked on the installation of PoDoFo please create an issue.

dependencies:
 - openssl-dev
 - fontconfig-dev
 - libtiff-dev
 - boost (optional)
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
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local  && \
make && make install
```

Building NoPoDoFo from source.
 - run `git clone https://github.com/corymickelson/NoPoDoFo`
 - run `npm install`
 - run `./node_modules/.bin/cmake-js build`
 - run `./node_modules/.bin/tsc -p tsconfig.json`

### AWS Lambda

Incorporating NoPoDoFo into your AWS Lambda functions is easily accomplished by installing the pre-built release packages.
In addition to installing this package `nopodofo` you will also need to install `nopodofolambda`. The latter package is a collection of
binaries used by PoDoFo that are not included in an AWS linux instance.
__Please note this will only run on AWS Lambda Node8__.
 - run `npm i -S https://github.com/corymickelson/NoPoDoFo/releases/download/v0.7.0/nopodofo-v0.7.0-linux-aws-x64-node-v8.tar.gz` - install nopodofo v0.7.0
 - run `npm i -S nopodofolambda`

You can also build this yourself using docker. Pull image nopodofo/aws, this is a centos 7 image which closely resembles the amazon linux image. Binaries
built on this will also run on AWS

 - Start a bash shell by running `docker run -it nopodofo/aws /bin/sh`
 - Enable devtoolset-7 `source /opt/rh/devtoolset-7/enable`
 - Clone this module `git clone https://github.com/corymickelson/NoPoDoFo`
 - Install dependencies `npm i`
 - Build package `LDFLAGS=-Wl,-rpath=/var/task/node_modules/nopodofolambda/lib ./node_modules/.bin/cmake-js build`


[1]: The Windows vcpkg PoDoFo build does not include all optional dependencies. This build does not support
AES256 encryption. Missing optional dependencies are: libidn, fontconfig. As previously stated, this build does not
support AES256 encryption which is caused from libidn not being available. The lack of fontconfig does not disable
features such as libidn but will require you to provide additional information when using the [Font](font.md) class.
