# Using NoPoDoFo in AWS Lambda

[NoPoDoFoLambda](https://github.com/corymickelson/NoPoDoFoLambda) provides a pre-built library dependencies for running nopodofo in aws lambda.

## Installation
 - `npm install nopodofolambda`, prebuilt library dependencies
 - `npm install nopodofo@awslambda`, nopodofo compiled with the correct rpath for running in aws lambda. __Important__ please ensure pre-built binary is fetched and the project is NOT rebuilt 

## The Build
Binaries are built on a Centos 7 image with the following dependencies:
 - devtoolset-7
 - cmake built from source, version >= 3.8
 - node, version >= 8.10
 - node install global: cmake-js, typescript, node-gyp (optional)
 - openssl-devel >= 1.1.0
 - libtiff-devel
 - libjpeg-devel
 - libpng-devel
 - freetype-devel
 - fontconfig-devel
 - libidn-devel
 - libz-devel

Setup our build environment to have the correct compiler, and variables:
 - scl enable devtoolset-7 bash
 - export CC=$(which gcc)
 - export CXX=$(which g++)
 - LDFLAGS=-Wl,-rpath=/var/task/node_modules/nopodofolambda/lib node-pre-gyp rebuild

### Finished

With all our binaries built we can download them to our local machine
 - scp -i pemfile user@host:/path/to/binary path/to/local/location

The above build is available for easy installation and usage [here](https://github.com/corymickelson/NoPoDoFoLambda)
