# NoPoDoFo

This package provides native bindings to node.js for parsing and modifying pdf documents with [PoDoFo](http://podofo.sourceforge.net/index.html).
NoPoDoFo is still in <mark>early development.</mark> 

### __Current Version 0.1.1__
[![NPM](https://nodei.co/npm/nopodofo.png?downloads=true&downloadRank=true)](http://nodei.co/npm/nopodofo)

## New Features
 - Digital Signing 
 - Access to Pdf data types (PdfObject, PdfArray, etc..., allows for things such as extracting images from a document [example](https://github.com/corymickelson/nopodofo/blob/master/lib/page.spec.ts#L145-L190))

## In Progress
 - windows builds
 - create new pdf's

## Installation

Built againt Mac OSX, Linux, and Windows 10

NoPoDoFo has been developed against the PoDoFo trunk or github master( v0.9.6 ) and built with dependencies zlib, openssl, libpng, libtiff, libjpeg, fontconfig, and freetype2.
It is recommended that when using NoPoDoFo you compile PoDoFo from source.
If PoDoFo is not built with the above dependencies not all features will be available. NoPoDoFo will throw an Error 'NotCompiled' when trying to access a feature that PoDoFo has not been compiled with.

NoPoDoFo requires:
 - [PoDoFo](http://podofo.sourceforge.net/index.html)
 - [cmake-js](https://www.npmjs.com/package/cmake-js) optional, to build with CMake run `npm run compile`
 - [Node.js](https://nodejs.org/) v8.6+ (nopodofo is built with [N-Api](https://nodejs.org/dist/latest-v8.x/docs/api/n-api.html))

For usage in AWS Lambda, PoDoFo prebuilt binaries are available [here](https://github.com/corymickelson/Commonopodofo_PoDoFo)

#### Linux
Before cloning and installing NoPoDoFo, clone and build PoDoFo.
To install to a different location change `-DCMAKE_INSTALL_PREFIX`. 
For more information on building podofo read the README.html from podofo root directory.

 - Clone PoDoFo from [git](https://github.com/svn2github/podofo) or [svn](http://svn.code.sf.net/p/podofo/code/podofo/trunk)
 - run `mkdir ../podofo-build`
 - run `cd ../podofo-build`
 - run `cmake -DCMAKE_INSTALL_PREFIX=/usr .. -DFREETYPE_INCLUDE_DIR=/usr/include/freetype2 -DPODOFO_BUILD_SHARED=1 -DPODOFO_HAVE_JPEB_LIB=1 -DPODOFO_HAVE_PNG_LIB=1 -DPODOFO_HAVE_OPENSSL_1_1=1 -DPODOFO_HAVE_TIFF_LIB=1 -DWANT_LIB64=1`
 - run `make && sudo make install`
 - Install NoPoDoFo by running `npm install -S nopodofo --podofo_library={path to libpodofo.so} --podofo_include_dir={path to podofo include directory, usually /usr/include}`

#### Windows
NoPoDoFo has been built and tested for windows using the vcpkg podofo. To install podofo
using vcpkg see [here](https://github.com/Microsoft/vcpkg), <mark>vcpkg podofo is not built with libidn or openssl</mark> and as such is missing features available in NoPoDoFo. 
The vcpkg of podofo provides a 32bit podofo binary, ensure that your node.js installation is also 32bit.
 - run `npm install -S nopdofo --podofo_build_path={path to podofo.dll} --podofo_include_path={path to podofo/podofo.h}`

### ToDo

 - Error handling for features PoDoFo has not been built with.
 - Setup documentation
 - Examples / Example project

### Usage

TypeDocs available [here](https://corymickelson.github.io/NoPoDoFo/index)

#### **Guides**
 - [NoPoDoFo Installation and Setup](guides/getting_started.md)
 - [NoPoDoFo.Document](guides/document.md)
 - [NoPoDoFo.StreamDocument](guids/stream_document.md)
 - [NoPoDoFo.Primitives](guides/primitives.md)
 - [NoPoDoFo.Painter](guides/painter.md)
 - [NoPoDoFo.Page](guides/page.md)
 - [NoPoDoFo.Object](guides/object.md)