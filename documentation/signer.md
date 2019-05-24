# API Documentation for Signer

- [API Documentation for Signer](#api-documentation-for-signer)
  - [NoPoDoFo Signer](#nopodofo-signer)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [signatureField](#signaturefield)
  - [Methods](#methods)
    - [loadCertificateAndKey](#loadcertificateandkey)
    - [write](#write)

## NoPoDoFo Signer

This class is a wrapper for the classes necessary to execute and apply a signature to the document.
NoPoDoFo currently supports PKCS7 (adbe-pkcs7-detached) signature only.
Please see the [Signer Cookbook](./cookbook/signer.md) for an example.

```typescript
class Signer {
  signatureField: SignatureField

  constructor(doc: Document, output?: string)

	loadCertificateAndKey(certificate: string | Buffer, opts?: { password?: string, pKey?: string | Buffer }, cb?: Callback<Number>): void
  write(minSignatureSize: Number, cb: Callback<Buffer | string>): void
}
```

## Constructors
------------------

```typescript
constructor(doc: Document, output?: string)
```

Create a new instance of a NoPoDoFo Signer. The Signer is the only means of signing a document.

## Properties
-------------

### signatureField

The page [Annotation](./annotations.md) used to store the signature.

## Methods
-----------

### loadCertificateAndKey

Loads the Certificate and Private Key and stores the values into the Signer instance.
Values are not retrievable but are stored for use on Signer.write method invocation.
After both cert and pkey are loaded, a minimal signature size is calculated and returned to
the caller. To complete the signing process this minimum signature size value needs to be
provided to the write method.

```typescript
loadCertificateAndKey(certificate: string | Buffer, opts?: { password?: string, pKey?: string | Buffer }, cb?: Callback<Number>): void
```

### write

Signs the document output to disk or a node buffer. The loadCertificateAndKey must be loaded prior to calling write

```typescript
write(minSignatureSize: Number, cb: Callback<Buffer | string>): void
```
