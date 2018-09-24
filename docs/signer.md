# API Documentation for Signer

* [Properties](#properties)
  * [signatureField](#signaturefield)
* [Methods](#methods)
  * [loadCertificateAndKey](#loadcertificateandkey)
  * [write](#write)

## NoPoDoFo Signer

```typescript
class Signer {
  signatureField: SignatureField

  /**
   * Creates a new instance of Signer. Signer is the only way to
   * @param {Document} doc
   * @param {string} [output] - optional if provided writes to this path, otherwise
   *      a buffer is returned
   * @returns {Signer}
   */
  constructor(doc: Document, output?: string)

  /**
   * Loads the Certificate and Private Key and stores the values into the Signer instance.
   * Values are not retrievable but are stored for use in Signer.sign
   * After both cert and pkey are loaded, a minimal signature size is calculated and returned to
   * the caller. To complete the signing process this minimum signature size value needs to be
   * provided to the write method.
   *
   * @param {string} certificate
   * @param {string} pkey
   * @param {string | Callback} p - either the pkey password or callback
   * @param {Callback} [cb] - callback
   * @returns {Number} - minimum signature size
   */
  loadCertificateAndKey(certificate: string, pkey: string, p: string | Callback<Number>, cb?: Callback<Number>): number

  /**
   * Signs the document output to disk or a node buffer
   * The loadCertificateAndKey must be loaded prior to calling write
   * @see loadCertificateAndKey
   * @param {Number} minSignatureSize
   * @param {Callback} cb
   */
  write(minSignatureSize: Number, cb: Callback<Buffer | string>): void
}
```

## Constructors

## Properties

## Methods