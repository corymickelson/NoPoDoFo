# NoPoDoFo Document

The NoPoDoFo Document class is a wrapper around PoDoFo::PdfMemDocument. This is the core class for reading and manipulating pdf documents.
This class was designed to allow easy access to the pdf object structure and should be used whenever you want to change the structure of
the pdf document. If you are only <i>creating</i> a pdf file use the [StreamDocument](https://github.com/corymickelson/NoPoDoFo/tree/master/guides/stream_document.md) class.


### Loading a document

When loading a pdf document, NoPoDofo will throw error `Password required` when a user/owner password has been set on the target document.
``` typescript
let doc:Document = new nopodofo.Document()
doc.load('/path/to/doc.pdf', (e:Error) => {
    if(e.message.includes('Password required')) {
        doc.password = 'secret'
    }
})

```

### Persisting changes

NoPoDoFo will __not__ persist any changes made until the `write` method has been called on the loaded document.

`NoPoDoFo.Document.write()` can accept an output path, which will write the modified document to disk. If the `write` method is called without an output path argument, the `write` command will return a node.js buffer of the modified document.

``` typescript
// Write to disk
doc.write('path/to/output.pdf', (e:Error) => {
    if(e instanceof Error) {
        // handle error
    }
})
// Write to buffer
doc.write((e:Error, d:Buffer) => {
    if(e) {
        // handle error
    }
    assert(Buffer.isBuffer(d))
})
```

### [Document Encryption](https://github.com/corymickelson/NoPoDoFo/tree/master/guides/encrypt.md)
NoPoDoFo provides the ability to encrypt a document through ```Document.createEncrypt```.
Encryption options are dependent on PoDoFo built with libidn and OpenSSL.
``` typescript
let doc = new NoPoDoFo.Document()
doc.load('/path/to/doc.pdf', (e) => {
    if(e.message.includes('Password required')) {
        // handle error or set password if password is known
    }
    let encrypt:EncryptionOption = {
        userPassword: 'secret',
        ownerPassword: 'secret',
        protection: ['Copy', 'Print', 'Edit'],
        algorithm: 'aesv2',
        keyLength: 40
    }
    doc.createEncrypt(encrypt) // Creates a PdfEncrypt object and sets document encrypt to this object.
    doc.write('output/path/doc.pdf', (e:Error) => {}) // write the document with new/updated encryption
})

```

### Pdf body (pdf object / pdf catalog)

A PDF is comprised of an array of [NoPoDoFo.Obj](obj.md), each object contains a dictionary of key/value pairs. A PDF can be modified by adding, removing, or altering the contents of a PdfObject(s), an example of this can be seen [here](https://github.com/corymickelson/nopodofo/blob/master/lib/page.spec.ts#L145-L190).
Document `.getObjects()` will return an array of [NoPoDoFo.Obj](https://corymickelson.github.io/NoPoDoFo/modules/_object_.html)

``` typescript
let doc = new NoPoDoFo.Document()
doc.load('/path/to/doc.pdf', () => {
    if(e.message.includes('Password required')) {
        doc.password = 'secret'
    }
    let body = doc.getObjects()
    // do something with pdf object
})
```