# Signer Cookbook

## Creating a Signature

```typescript
import {nopodofo as npdf, NPDFAnnotationFlag, NPDFAnnotation, pdfDate} from 'nopodofo'
const doc = new npdf.Document()
doc.load('/path/to/pdf', e => {
  if(e) {/* handle error */}

  // Create an annotation for the SignatureField
  const rect = new npdf.Rect(0, 0, 10, 10),
      page = doc.getPage(0),
      annot = page.createAnnotation(NPDFAnnotation.Widget, rect)

  // This signature is going to be hidden, for a visible signature add an appearance stream
  annot.flags = NPDFAnnotationFlag.Hidden | NPDFAnnotationFlag.Invisible

  const field = new npdf.SignatureField(annot, doc)

  // Set field properties
  field.setReason('test')
  field.setLocation('here')
  field.setCreator('me')
  field.setFieldName('signer.sign')

  // This will set the date as now
  field.setDate()
  // or create a new date and pass to the setDate function
  let d = new npdf.Date(pdfDate(new Date('2012-12-12')))
  field.setDate(d)
})
```

## Signing a Document

```typescript
import {nopodofo as npdf} from 'nopodofo'
const doc = new npdf.Document()
doc.load('/path/to/pdf', e => {
  let field = /* See above example */
  const signer = new npdf.Signer(doc, '/output/path')
  signer.signatureField = field
  signer.loadCertificateAndKey('/cert', '/key', (error, signatureLength) => {
    if (error) { /* handle error */}
    signer.write(signatureLength, (e, d) => {
      if (e) { /* handle error*/ }
      // signed document has been written to /output/path provided in Signer constructor 
    })
  })
})
```

## Get a Signature

```typescript
import {nopodofo as npdf} from 'nopodofo'
const doc = new npdf.Document()
doc.load('/path/to/pdf', e => {
  let signatures  = doc.getSignatures()
  if(signatures.length > 0) {
    for(let i = 0; i < signature.length; i++) {
      const so = signatures[i].getObject()
      // Signature Object will contain the signature as a Hexidecimal String,
      // the signed byte range as ByteRange and more.
    }
  }
})
```