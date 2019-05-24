# API Documentation for Encrypt

- [API Documentation for Encrypt](#api-documentation-for-encrypt)
  - [NoPoDoFo Encrypt](#nopodofo-encrypt)
  - [Constructors](#constructors)
    - [Example](#example)
  - [Properties](#properties)
    - [owner](#owner)
    - [user](#user)
    - [encryptionKey](#encryptionkey)
    - [keyLength](#keylength)
    - [protections](#protections)
  - [Methods](#methods)
    - [isAllowed](#isallowed)

## NoPoDoFo Encrypt

A PDF can encrypt it's contents, all strings and stream objects, to prevent unauthorized user's from viewing sensitive information.
EncryptOption(s) are dependent on PoDoFo built with libidn (used for AES256, if libidn is absent only rc4v1 and rv4v2 are available) and OpenSSL.

```typescript
class Encrypt {
  static createEncrypt(opts: EncryptOption): NPDFExternal<Encrypt>
  new(doc: Document): Encrypt
  new(copy: NPDFExternal<Encrypt>): Encrypt
  owner: string
  user: string
  encryptionKey: string
  keyLength: number
  protections: ProtectionSummary
  isAllowed(action: ProtectionOption): boolean
}
```

## Constructors
```typescript
new(copy: NPDFExternal<Encrypt>): Encrypt
```
Create an Encrypt object from an NPDFExternal<Encrypt> pointer.

```typescript
new(doc: Document): Encrypt
```
Create an Encrypt object from the [Document](./document.md) `encrypt` property. If the document does not have an Encrypt value set, returns an error.

```typescript
static createEncrypt(opts: EncryptOption): NPDFExternal<Encrypt>
```
Create a new instance of a pdf encrypt object with options EncryptOption. This method does not return an Encrypt object but a pointer
to an internal instance of a PoDoFo PdfEncrypt object. The return value is not inspectable from javascript; the return value is only
intended to be used to encrypt a [Document](./document.md) by setting this value as the `Document().encrypt` property. For more information
on the return value type see [NPDFExternal](./cookbook/datatypes.md#external)

### Example

``` typescript
let doc = new NoPoDoFo.Document()
doc.load('/path/to/doc.pdf', (e) => {
    if(e.message.includes('Password required')) {
        // handle error or set password if password is known
    }
    let encrypt = npdf.Encrypt.createEncrypt({
        ownerPassword: 'secret',
        userPassword: 'secret', // if empty the user does not have to enter a password to open the document
        keyLength: 40,
        protection: ['Edit', 'FillAndSign'],
        algorithm: 'rc4v1'
    })
    doc.encrypt = encrypt // set the encrypt object on  the document
    doc.write('output/path/doc.pdf', (e:Error) => {}) // write the document with new/updated encryption
})

```

## Properties
----------------

### owner
Get the owner object value

### user
Get the user object value

### encryptionKey
Get the encryption key.

### keyLength
Get the encryption key length

### protections
Get the protections applied to the [Document](./document.md) as ProtectionSummary

## Methods
---------

### isAllowed

```typescript
isAllowed(action: ProtectionOption): boolean
```

Check if a ProtectionOption is allowed.
