# NoPoDoFo Forms

NoPoDoFo supports creating, modifying, and filling interactive forms (AcroForm). 
To access a documents acroform use the `Document.form` accessor. 
You can use the acroform's dictionary to access the fields array, but the preferred way is through the page.
All annotations are indexed and easily available via `getField`, `getFields`, `getAnnotation` and `getAnnotations`.  

### Setting the default appearance stream
An acroforms default appearance stream is 12pt black arial. You can override the defaults on the annotation directly.
```typescript
let doc = new npdf.Document()
doc.load('/path/to/doc.pdf', e => {
  if(e instanceof Error) {
    /* handle error */
  }
  doc.form.needAppearances = true // use the default appearance stream.
})
```

### Signature Fields
A Signature field is instantiated and used slightly differently than the other Field types. For a complete example see the signer [spec]().
A signature field works in conjunction with the acroform object. A modification to the signature field should be reflected in the acroforms
SigFlags. SigFlags values can be one of the following:
  - (0) no signature set
  - (1) signature exists, appendOnly = false
  - (2) document is in appendOnly mode, signature may or may not exist
  - (3) signature exist, and document is in appendOnly mode

