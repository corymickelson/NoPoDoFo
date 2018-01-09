# PoDoFo Build

## Windows

Building podofo on windows is made much simpler with the help of vcpkg. Vcpkg does offer a podofo pacakage but for nopodofo we will want to build podofo from source with additional libraries that are not included in the vcpkg compiled podofo.
This build does not support AES encryption option, to enable AES you will need libidn v1.
 - Install [vcpkg](https://github.com/Microsoft/vcpkg)
 - Install vcpkg's
   - tiff-dev
   - libpng-dev
   - jpeg-dev 
   - fontconfig-dev
   - freetype-dev
   - openssl-dev
   - zlib-dev


NoPoDoFo is developed with podofo compiled with all optional libraries (As noted above this build is missing libidn). In the previous step we've included most optional libraries, in this step we will clone podofo and build.

 - Clone [PoDoFo](https://github.com/svn2github/podofo)
 
Open with visual studio's open / project / cmake. If a default CMakeSettings.json is  not generated select the CMakeLists.txt file and select Change CMake settings. Add the following to the `x86-Release` configuration(s):

``` json
"variables": [
  {
    "name": "CMAKE_TOOLCHAIN_FILE",
   "value": "path to your vcpkg vcpkg.cmake file"
  }, {
    "name": "LIBCRYPTO_LIBRARY_NAMES",
    "value": "libeay32"
  }, {
    "name": "LIBCRYPTO_INCLUDE_DIR",
    "value": "path to include/openssl"
  }]
```

Now build podofo in visual studio, make sure you select a Release x86 build. The build output is defined in the CMakeSettings.json configuration.

## Linux

There are many linux distro's, but for this guide I will only be covering arch linux. Other distro's will have similar
installation instruction, if you are truely blocked on the installation of PoDoFo please create an issue.
NoPoDoFo provides bindings for PoDoFo compiled with all optional dependencies. Please install the following:

 - openssl-dev
 - fontconfig-dev
 - libtiff-dev
 - libidn-dev
 - libjpeg-turbo-dev
 - libpng-dev
 - freetype-dev
 - zlib-dev

Install dev dependencies with `yaourt -S [dependencies]`

 - Clone [PoDoFo](https://github.com/svn2github/podofo)

Building and installing podofo on linux is simple. Run the following:
 - `cd podofo && mkdir build`
 - `cmake -DCMAKE_INSTALL_PREFIX=/usr .. \
		-DFREETYPE_INCLUDE_DIR=/usr/include/freetype2 \
		-DPODOFO_BUILD_SHARED=1 \
		-DPODOFO_HAVE_JPEG_LIB=1 \
		-DPODOFO_HAVE_PNG_LIB=1 \
		-DPODOFO_HAVE_TIFF_LIB=1`

cmake will output a bunch of stuff to the console, ensure that both openssl and libidn are found during this process, without libidn or openssl NoPoDoFo will not have complete encryption support / functionality.

 - run `make && [sudo] make install`
 - see instruction for building NoPoDoFo

## Docker

NoPoDoFo also provides a docker file for alpine linux with podofo built from source. To use this image for your project, in your dockerfile add the following

`FROM corymickelson/nopodofo-alpine:latest`


# Building NoPoDoFo

With PoDoFo built it is time to compile and link NoPoDoFo. NoPoDoFo provides a CMakeLists file for building with CMake as well as a gyp file for building with node-gyp.

<mark>CMake does not always work on windows, WIP</mark>

## Windows
Windows usage is still in testing, not all features may work.
Installing NoPoDoFo with node-gyp. 
 - Clone NoPoDoFo from git or npm
   -  `npm i -S nopodofo` 
   - `git clone https://github.com/corymickelson/NoPoDoFo.git nopodofo`
 - `cd nopodofo`
 - `npm i --podofo_library="path to directory containing files:  podofo.lib and podofo.dll" --podofo_include_dir="path to podofo source directory"`

## Linux
Installing NoPoDoFo with CMake
If you installed PoDoFo with the instructions provided above, PoDoFo will already be on your path in /usr. If you installed PoDoFo somewhere else you 
may need to open CMakeLists.txt and add the path to 

`
find_path(PODOFO_INCLUDE_DIR podofo/podofo.h PATHS /usr/include /usr/local/include /* Here */)
`

`
find_library(PODOFO_LIBRARY NAMES podofo PATHS /usr/lib64 /usr/lib /usr/local/lib /* Here */)
`
 - Clone NoPoDoFo from git or npm
   -  `npm i -S nopodofo` 
   - `git clone https://github.com/corymickelson/NoPoDoFo.git nopodofo`
 - `cd nopodofo`
 - `npm run compile`


## Compile for JS
NoPoDoFo's interface is written in [Typescript](https://www.typescriptlang.org/). For typescript projects, you should import the class directly from the lib 
directory. For JS projects you can run `npm run lib-build` and `require('nopodofo')`
## Check Installation

 It's a good idea to check that NoPoDoFo has been built and linked properly. Running this check will also inform you to whether or not any features available in NoPoDoFo are not available due to PoDoFo not being compiled with all optional dependencies.

  - `npm run lib-build && npm test`

