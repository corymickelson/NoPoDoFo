# NoPoDoFo


nopodofo, a native node module for reading, parsing, modifying, and creating pdf documents with [PoDoFo](http://podofo.sourceforge.net/index.html).

<span style="background-color:#FFFF00">Still in early development.</span>

# New Features!
 - Get / Set Form Fields
 - Access to Pdf data types (PdfObject, PdfArray, etc..., allows for things such as extracting images from a document [example](https://github.com/corymickelson/nopodofo/blob/master/lib/page.spec.ts#L145-L190))

## In Progress:
 - Add Images
 - Add Digital Signature

### Installation

Currently tested on Mac and Linux

nopodofo has been developed against the PoDoFo trunk or github master( v0.9.6 ). It is recommended that when using nopodofo you compile PoDoFo from source. Thankfully this is incredibly simple on OSX and Linux, instructions are provided on the PoDoFo source README.html.  

nopodofo requires PoDoFo built with dependencies: zlib, openssl, libpng, libtiff, libjpeg, fontconfig, and freetype2

nopodofo requires:
 - [PoDoFo](http://podofo.sourceforge.net/index.html)
 - [cmake-js](https://www.npmjs.com/package/cmake-js)
 - [Node.js](https://nodejs.org/) v8.6+ (nopodofo is built with [N-Api](https://nodejs.org/dist/latest-v8.x/docs/api/n-api.html))

For usage in AWS Lambda, PoDoFo prebuilt binaries are available [here](https://github.com/corymickelson/Commonopodofo_PoDoFo)

To install nopodofo run 
`npm i -S nopodofo`

### ToDo

 - Setup documentation
 - Add build dependencies to project repo
 - Add older Node.js version (v4.x, v6.x) support
 - Examples

### Usage

TypeDocs available [here](https://corymickelson.github.io/NoPoDoFo/index)
