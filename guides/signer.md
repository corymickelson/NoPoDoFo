# NoPoDoFo Signer

The NoPoDoFo Signer class is an attempt at exposing an easy to understand, easy to use signature api. NoPoDoFo Signer wraps PoDoFo PdfSignatureField, PdfOutputDevice, PdfSignOutputDevice, and PdfRefCountedBuffer classes. NoPoDoFo Signer also exposes a `signature` function to read-in, parse, and return a valid digital signature from a certificate and private key file, <mark>Use of Signer.signature depends on Openssl</mark>. Digital signing has only been tested on linux.


### Generating a certificate with Openssl
The following have been copied from the IBM knowledge center.
The following will generate a private key and certificate.
 ```
 openssl req -newkey rsa:2048 -nodes -keyout key.pem -x509 -days 365 -out certificate.pem
 ```

### Signature as string

To digitally sign a document we first need a valid signature. NoPoDoFo will return a DER formatted signature provided a valid certificate and private key file. As stated above this function depends on NoPoDoFo compiled with Openssl.

```typescript
import {signature} from 'nopodofo/lib/signer'
import {join} from 'path'
const cert = join(__dirname, '/path/to/certificate.pem'),
    pkey = join(__dirname, '/path/to/privatekey.pem')

const signData = signature(cert, pkey) // if you've set a password, pass the password as the last (third) argument to signature()
```

### Loading a document in preparation for signing
To sign a document in NoPoDoFo, the document needs to have been loaded with updates on. Loading a document for updating is accomplished by adding `true` to the load command.

```typescript
import {Document} from 'nopodofo/lib/document'
const doc = new Document('path/to/doc', true)
```

### Prepare signature field

When NoPoDoFo signs a document, it takes the document, the signature, and the signature field, and optionally if an output path is provided the document will be written to the destination,
otherwise the document will be returned as a nodejs buffer.
NoPoDoFo will handle allocating space for the signature on the document, and writing the signature to the document.

```typescript
const doc = new Document()
doc.load('/path/to/doc', {forUpdate: true}, e => {
    if(e instanceof Error) {
        // handle error
    }
    // a signature field requires an annotation, the acro form, and document
    const rect = new Rect([0, 0, 10, 10]),
        page = doc.getPage(1),
        annot = page.createAnnotation(NPDFAnnotation.Widget, rect),
        form = new Form(doc)
    annot.flag = NPDFAnnotationFlag.Hidden | NPDFAnnotationFlag.Invisible
    const field = new SignatureField(annot, doc)

    // set signature properties
    field.setReason('example')
    field.setLocation('here')
    field.setCreator('NoPoDoFo')
    field.setFieldName('NoPoDoFoSignatureField')
    field.setDate() // set's date now
})
```

### Signer 
NoPoDoFo signer is responsible for setting the signature field, signature, and persisting the document.

```typescript
const doc = new Document()
doc.load('/path/to/doc.pdf', {forUpdate: true}, e => {
    if(e instanceof Error) {
        // handle error
    }
    let form = new Form(doc),
        rect = new Rect([0.0, 0.0, ,0.0, ,0.0]),
        page = doc.getPage(1),
        annot = page.createAnnotation(NPDFAnnotation.Widget, rect)
    annot.flag = NPDFAnnotationFlag.Hidden | NPDFAnnotationFlag.Invisible
    const field = new SignatureField(annot, doc)
    //set field properties
    let data = signature('path/to/cert', 'path/to/privatekey')
    // create signer
    let signer = new Signer(doc)
    signer.setField(field)
    signer.signSync(sign) // signer will allocate and write the signature to the document. If a output path is provided the document will be written to the destination, otherwise this function will return a nodejs buffer
})
```

For a complete example of applying a digital signature to a pdf document see [here](https://github.com/corymickelson/NoPoDoFo/blob/master/lib/signer.spec.ts#L13-L45)