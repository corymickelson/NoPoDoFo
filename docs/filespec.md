# API Documentation for FileSpec

* [Properties](#properties)
  * [name](#name)
* [Methods](#methods)
  * [getContents](#getcontents)

## NoPoDoFo FileSpec
A FileSpec [Dictionary](./dictionary.md) is used for embedding files in a PDF. This dictionary contains keys:
 - `F`:  filename
 - `UF`: filename as utf16
 - `EF`: the embedded file
 
The simplist method for attaching a file is using [Document.attachFile](./document.md#attachfile).

```typescript
class FileSpec {

  new(file: string, doc: Base, embed?: boolean): FileSpec
  new(obj: Object): FileSpec

  readonly name: string

  getContents(): Buffer
}
```
## Constructors
```typescript
new(file: string, doc: Base, embed?: boolean): FileSpec
```
Constructs a new FileSpec [Dictionary](./dictionary.md) with the file provided and attached to the [Document](./document.md).

## Properties

### name
The name of the file as `utf8`

## Methods

### getContents
```typescript
getContents(): Buffer
```
Get the contents of a file