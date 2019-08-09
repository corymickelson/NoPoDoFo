# API Documentation for Image

- [API Documentation for Image](#api-documentation-for-image)
  - [NoPoDoFo Image](#nopodofo-image)
  - [Constructor](#constructor)
  - [Properties](#properties)
    - [width](#width)
    - [height](#height)
  - [Methods](#methods)
    - [setICCProfile](#seticcprofile)
    - [setSoftMask](#setsoftmask)
    - [setChromaKeyMask](#setchromakeymask)
    - [setColorSpace](#setcolorspace)
    - [setInterpolate](#setinterpolate)

## NoPoDoFo Image

Create and embed the image in the [Document](./document.md), the image need only be created once, after it's been created and embedded
in the document it can be referenced and [Painted](./painter.md) on any [Page](./page.md) any number of times. See the [Image Cookbook](./cookbook/images.md)
for examples.

```typescript
class Image {
  new(doc: Base, source: string | Buffer, format?: NPDFImageFormat): Image
  readonly width: number
  readonly height: number
  setICCProfile(input: Buffer, colorComponent: number, alt: NPDFColorSpace): void
  setSoftMask(img: Image): void
  setChromaKeyMask(r: number, g: number, b: number, threshold: number): void
  setColorSpace(colorSpace: NPDFColorSpace): void
  setInterpolate(v: boolean): void
}
```

## Constructor
--------------

```typescript
new(doc: Base, source: string | Buffer, format?: NPDFImageFormat): Image
```

Create a new Image object from the source provided, to be embedded in the [Document](./document.md) provided as one of NPDFImageFormat format options.

## Properties
-------------

### width
Readonly, get the width of the image

### height
Readonly, get the height of the image

## Methods
-----------

### setICCProfile

```typescript
setICCProfile(input: Buffer, colorComponent: number, alt: NPDFColorSpace): void
```

Set an ICC profile for this image. The ICC profile data is read in as the [input], the number of color components
is set by the [colorComponent], and a color space to use in case the ICC profile cannot be used is set by [alt]

### setSoftMask

```typescript
setSoftMask(img: Image): void
```

Set a soft mask for this image. The image to be set as the soft mask must be an 8 bit greyscale.

### setColorSpace

```typescript
setColorSpace(colorSpace: NPDFColorSpace): void
```

Set the color space of the image, the default is `DeviceRGB`.

### setChromaKeyMask

```typescript
setChromaKeyMask(r: number, g: number, b: number, threshold: number): void
```

Set the color chroma key mask on the image.
The mask color will not be painted, i.e masked as transparent.

### setInterpolate

```typescript
setInterpolate(v: boolean): void
```

Enable or disable interpolation. Apply an interpolation if the source resolution is lower than the resolution of the output device.