# Installation

Requirements:

- PoDoFo >= 0.9.6
- CMake >= 3
- Nodejs >= 8
- OpenSSL

The recommended installation process is as follows:

## Windows

NoPoDoFo **does** provide a prebuilt package for windows x64, the prebuilt is built on windows 10 and can be installed into your npm project with
`npm i https://github.com/corymickelson/NoPoDoFo/releases/download/v1.0.0/nopodofo-v1.0.0-pre-win64.tar.gz`

The recommended build for NoPoDoFo on Windows requires building PoDoFo from source.

To build from source first fetch the source code; podofo source code is available via github at `https://github.com/corymickelson/podofo.git .` or svn at `https://sourceforge.net/p/podofo/code/HEAD/tree/podofo/trunk .`

The following instructions are for building PoDoFo via visual studio 2019, the same instructions should work for 2017.

PoDoFo required libraries can be installed via vcpkg. If you do not have vcpkg setup,
please go to [vcpkg](https://github.com/Microsoft/vcpkg) and follow the installation instructions.

Please take not that libidn v1 is not available via vcpkg, these instructions will **not** include libidn.
PoDoFo and thus NoPoDoFo built without libidn will **not** include support for AES256 encryption. 

With vcpkg installed; install the following packages (all packages are windows-x64)

- libjpeg-turbo
- tiff
- libpng
- openssl
- freetype
- zlib

After all the above have installed ensure they are accessible to our project, this is easily accomplished
with the `vcpkg integrate install` command.

Open visual studio, select open folder and then select the PoDoFo root directory. Once this has loaded, 
right click the CMakeLists.txt file at the root of the project and click `CMake Settings for PoDoFo`

Update to match the following, replace cmakeToolchainFile path with the correct path on your system:

```json
{
    "name": "x64-Release",
    "generator": "Ninja",
    "configurationType": "Release",
    "inheritEnvironments": [ "msvc_x64_x64" ],
    "buildRoot": "${env.USERPROFILE}\\CMakeBuilds\\podofo\\${name}",
    "installRoot": "${env.USERPROFILE}\\CMakeBuilds\\podofo\\install\\${name}",
    "cmakeCommandArgs": "",
    "buildCommandArgs": "-v",
    "ctestCommandArgs": "",
    "variables": [
    {
        "name": "LIBCRYPTO_LIBRARY_NAMES",
        "value": "libeay32",
        "type": "STRING"
    }
    ],
    "cmakeToolchain": "${Path to vcpkg.cmake file, if you do not know run vcpkg integrate install}"
}
```

On save this should re-generate the cmake cache, but if it does not right click the CMakeLists.txt file
at the root of the project and select `Generate cache for PoDoFo`.

Now we can select the `Build All` from the `Build` dropdown.
After the build has completed select `Install PoDoFo` from the `Build` dropdown.

This concludes building PoDoFo.

Building NoPoDoFo:

Clone NoPoDoFo `git clone git@github:corymickelson/NoPoDoFo.git .`

To build NoPoDoFo run:

- `cd nopodofo`
- `npm install`
- `npm run build -- --CDPODOFO_BUILD_PATH={path to buildRoot} --CDPODOFO_INSTALL_PATH={path to installRoot} --CDCMAKE_TOOLCHAIN_FILE={path to vcpkg.cmake}`

ex npm run command: `npm run build -- --CDCMAKE_TOOLCHAIN_FILE=C:\LIBS\VCPKG\SCRIPTS\BUILDSYSTEMS\VCPKG.CMAKE --CDPODOFO_BUILD_PATH=C:\Users\micke\CMakeBuilds\podofo\build\x64-Debug-Ninja --CDPODOFO_INSTALL_PATH=C:\Users\micke\CMakeBuilds\podofo\install\x64-Debug-Ninja`

## Mac

PoDoFo v0.9.6 is available in MacPorts or vcpkg.

Building NoPoDoFo

- `cd nopodofo`
- `npm i nopodofo`
- `npm run build`


## Linux

Please use your native package manager, the following is for arch (pacman). Other distro's will have similar
installation instruction, if you are truly blocked on the installation of PoDoFo please create an issue.

dependencies:

- openssl
- fontconfig
- libtiff
- boost (optional)
- libidn
- libjpeg-turbo
- libpng
- freetype
- zlib

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
