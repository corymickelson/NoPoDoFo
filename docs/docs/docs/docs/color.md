# API Documentation for Color

- [API Documentation for Color](#api-documentation-for-color)
    - [NoPoDoFo Color](#nopodofo-color)
    - [Constructors](#constructors)
    - [Properties](#properties)
    - [Methods](#methods)
        - [isRGB](#isrgb)
        - [isCMYK](#iscmyk)
        - [isGreyScale](#isgreyscale)
        - [convertToGreyScale](#converttogreyscale)
        - [convertToCMYK](#converttocmyk)
        - [convertToRGB](#converttorgb)
        - [getGrey](#getgrey)
        - [getRed](#getred)
        - [getGreen](#getgreen)
        - [getBlue](#getblue)
        - [getCyan](#getcyan)
        - [getMagenta](#getmagenta)
        - [getYellow](#getyellow)
        - [getBlack](#getblack)
    
## NoPoDoFo Color

The Color class represents all color formats supported by NoPoDoFo.
All color values must be between 0.0 and 1.0

```typescript
class Color {
    constructor(grey: number)
    constructor(red:number, green: number, blue: number)
    constructor(cyan: number, magenta: number, yellow: number, black: number)
    isRGB(): boolean
    isCMYK(): boolean
    isGreyScale(): boolean
    convertToGreyScale(): Color
    convertToCMYK(): Color
    convertToRGB(): Color
    getGrey(): number
    getRed(): number
    getGreen(): number
    getBlue(): number
    getCyan(): number
    getMagenta(): number
    getYellow(): number
    getBlack(): number
}
```

## Constructors
--------------

```typescript
constructor(grey: number)
```

Create a new GreyScale Color

```typescript
constructor(red:number, green: number, blue: number)
```

Create a new RGB Color

```typescript
constructor(cyan: number, magenta: number, yellow: number, black: number)
```

Create a new CMYK Color

## Properties
--------------

## Methods
--------------

### isRGB

```typescript
isRGB(): boolean
```

Is the color in RGB format

### isCMYK

```typescript
isCMYK(): boolean
```

Is the color in CMYK format

### isGreyScale

```typescript
isGreyScale(): boolean
```

Is the color in GreyScale format

### convertToGreyScale

```typescript
convertToGreyScale(): Color
```

Get the current color converted to GreyScale

### convertToCMYK
```typescript
convertToCMYK(): Color
```

Get the current color converted to CMYK

### convertToRGB
```typescript
convertToRGB(): Color
```

Get the current color converted to RGB

### getGrey
```typescript
getGrey(): number
```

Get grey value

### getRed

```typescript
getRed(): number
```

Get red value

### getGreen

```typescript
getGreen(): number
```

Get green value

### getBlue
```typescript
getBlue(): number
```

Get blue value

### getCyan
```typescript
getCyan(): number
```

Get cyan value

### getMagenta
```typescript
getMagenta(): number
```

Get magenta value

### getYellow
```typescript
getYellow(): number
```

Get yellow value

### getBlack
```typescript
getBlack(): number
```

Get black value
