# Using NoPoDoFo in AWS Lambda

[NoPoDoFoLambda](https://github.com/corymickelson/NoPoDoFoLambda) provides a pre-built binary/library for installing into your lambda function.
NoPoDoFoLambda will be updated with each new release of NoPoDoFo.

## Installation
This package is meant __only__ for use in AWS Lambda.
 - `npm install nopodofolambda`
 - require/import into your project, `const {npdf} = require('nopodofolambda')`

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
 - export LDFLAGS=-Wl,-rpath=/var/task/node_modules/nopodofolambda/lib

### Building NoPoDoFo
Clone the repo and checkout the tag or branch or version desired.
 - git clone https://github.com/corymickelson/NoPoDoFo
 - npm install
 - tsc -p tsconfig.json
 - npm test (optional, test everything is built properly)

### Finished

With all our binaries built we can download them to our local machine
 - scp -i pemfile user@host:/path/to/binary path/to/local/location

The above build is available for easy installation and usage [here](https://github.com/corymickelson/NoPoDoFoLambda)
