# API Documentation for Date

* [Properties](#properties)
* [Methods](#methods)
  * [toString](#tostring)

## NoPoDoFo Date
The Date data type in a PDF must follow a specific format.
```typescript
class Date {
  new(): Date
  new(timestamp: string): Date

  toString(): string
}
```



## Constructors

Create a new PdfDate object with a timestamp of now
```typescript
new(): Date
```
Create an existing timestamp from a string. The string must be formatted in the following: (D:YYYYMMDDHHmmSSOHH'mm')
```typescript
new(timestamp: string): Date
```


## Properties

## Methods

### toString
Returns in the the following format: (D:YYYYMMDDHHmmSSOHH'mm').