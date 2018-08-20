# API Documentation for Data

* [Properties](#properties)
  * [value](#value)
* [Methods](#methods)
  * [write](#write)

## NoPoDoFo Data
The Data class is for writing arbitrary data to the [Document](./document.md). It is the user's responsibility
to ensure the data written to the [Document](./document.md) is valid data for a PDF.
```typescript
class Data {
  new(value: string | Buffer): Data
  readonly value: string
  write(output: string): void
}
```

## Constructors

Instantiate and returns a new instance of Data.
```typescript
new(value: string | Buffer): Data
```

## Properties

### value
The actual data to be written to a [Document](./document.md). This value must be a string or Buffer.

## Methods

### write
Write the Data object to the output value, output must be a valid path on disk.

