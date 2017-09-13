# N_Pdf


NPdf, a native node module for reading, parsing, modifying, and creating pdf documents with [PoDoFo](http://podofo.sourceforge.net/index.html).

<span style="background-color:#FFFF00">Still in early development.</span>

# New Features!

 - Add an image
 - Rotate pages
 - Get / Set Form Fields
 - Delete Page
 - Merge Documents
 - Get Pdf Document info ( isLinearized, page count, has password, ... more to come)

### Installation

NPdf requires:
 - [PoDoFo](http://podofo.sourceforge.net/index.html)
 - [cmake-js](https://www.npmjs.com/package/cmake-js)
 - [Node.js](https://nodejs.org/) v8+ (NPdf is built with [N-Api](https://nodejs.org/dist/latest-v8.x/docs/api/n-api.html))
 - [Boost](http://www.boost.org/) FileSystem module only

For usage in AWS Lambda, PoDoFo prebuilt binaries are available [here](https://github.com/corymickelson/CommonPdf_PoDoFo)

To install Npdf run 
`npm i -S n_pdf`

### Usage

Getting started
``` javascript
import {Document, Page} from 'npdf'
```
NPdf includes a definition file for developer happiness. Import `src/npdf.d.ts` into your editor of choice.

For Webstorm go to settings > language & frameworks > javascript > libraries and add and select the definitions file.


