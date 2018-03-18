# NoPoDoFo Encrypt
NoPoDoFo (PoDoFo compiled with libidn and OpenSSL) provides the ability to set owner / user password(s), encryption key length, document protection, and encryption algorithm options.
See [EncryptionOption](https://github.com/corymickelson/nopodofo/blob/master/lib/encrypt.ts#L21-L27) for details.

To encrypt a document see the [Document guide](https://github.com/corymickelson/NoPoDoFo/tree/master/guides/document.md)

The encryption object can be retrieved from the ```Document``` class property ```encrypt```.
If the loaded document does not have an encryption object ```null``` will be returned.
It is the developer's responsibility to check for null when using the encrypt object.

```typescript
interface IEncrypt {
    owner: string
    user: string
    encryptionKey: string
    keyLength: number
    protections: ProtectionSummary
    isAllowed(action: ProtectionOption): boolean
}
```

## Getting an Encryption Object from a Document
```typescript
let doc:Document = new nopodofo.Document('/path/to/doc.pdf')
doc.on('ready', () => {
  const encrypt:IEncrypt
  if(doc.encrypt) {
    encrypt= doc.encrypt
    // get the summary of allowed actions on this document
    const protections = encrypt.protections
    // check that you are allowed to perform desired action
    // ...do action
  }
}).on('error', (e:Error) => {
      if(e.message.includes('Password required')) {
          doc.password = 'secret'
      }
  })
```