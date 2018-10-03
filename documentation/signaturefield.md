# API Documentation for SignatureField

- [API Documentation for SignatureField](#api-documentation-for-signaturefield)
  - [NoPoDoFo SignatureField](#nopodofo-signaturefield)
  - [Constructors](#constructors)
  - [Properties](#properties)
  - [Methods](#methods)
    - [setAppearanceStream](#setappearancestream)

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
---------------

```typescript
new(annotation: Annotation, doc: Document): SignatureField
```

Create a new instance of a SignatureField object. The [Annotation](./annotations.md) must be of type `Widget`.

## Properties
-------------

## Methods
-----------------

### setAppearanceStream

```typescript
setAppearanceStream(xObj: any, appearance: NPDFAnnotationAppearance, name: string): void
```

Set an appearance stream for this field. Setting the appearance stream `AP` is comprised of a set of instructions written
to an [XObject](./xobject.md), the type of appearance as one of NPDFAnnotationAppearance, and an optional name for the state of
the appearance. Please familiarize yourself with the [spec](https://wwwimages2.adobe.com/content/dam/acom/en/devnet/pdf/PDF32000_2008.pdf) section
12.5.5 for proper usage.

```typescript
setReason(reason: string): void
```

Set signature reason

```typescript
setLocation(location: string): void
```

Set signature location

```typescript
setCreator(creator: string): void
```

Set creator of signature

```typescript
setDate(dateTime?: string|nopodofo.Date): void
```

Set the date of signature

```typescript
addCertificateReference(perm: NPDFCertificatePermission): void
```

Add certification dictionaries and references to the [Document catalog](./document.md#catalog)
Document modification permission as NPDFCertificatePermission

```typescript
setFieldName(name: string): void
```

Set the name of the signature field

```typescript
getObject(): Object
```

Get the signature object

```typescript
ensureSignatureObject(): void
```

Ensure the signature field has set a signature [Object](./object.md).
If the signature object has not been set a `Sig` object is created and added to
the field dictionary. This is useful for when creating an instance of this class from
an existing annotation to ensure the signature object is set.