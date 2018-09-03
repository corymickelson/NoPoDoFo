# API Documentation for SignatureField

* [Properties](#properties)
* [Methods](#methods)
  * [setAppearanceStream](#setappearancestream)
  * [setReason](#setreason)
  * [setLocation](#setlocation)
  * [setCreator](#setcreator)
  * [setDate](#setdate)
  * [setFieldName](#setfieldname)
  * [addCertificateReference](#setcertificatereference)
  * [getObject](#getobject)
  * [ensureSignatureObject](#ensuresignatureobject)

## NoPoDoFo SignatureField
A SignatureField is a type of [Form](./form.md) field that contains a digital signature. This class is **not** derived
from the Field base class. A signature field can present a graphical interface prompting the user to "sign" the document.
The process of signing a [Document](./document.md) requires a combination of this class and the [Signer](./signer.md) class,
for an example please see the [signing cookbook](./cookbook/signing.md).
```typescript
class SignatureField {
  new(annotation: Annotation, doc: Document): SignatureField

  setAppearanceStream(xObj: any, appearance: NPDFAnnotationAppearance, name: string): void
  setReason(reason: string): void
  setLocation(location: string): void
  setCreator(creator: string): void
  setDate(dateTime?: string): void
  addCertificateReference(perm: NPDFCertificatePermission): void
  setFieldName(name: string): void
  getObject(): Object
  ensureSignatureObject(): void
}
```

## Constructors

```typescript
new(annotation: Annotation, doc: Document): SignatureField
```

## Properties

## Methods