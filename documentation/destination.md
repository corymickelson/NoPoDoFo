# API Documentation for Destination

- [API Documentation for Destination](#api-documentation-for-destination)
  - [NoPoDoFo Destination](#nopodofo-destination)
  - [Constructors](#constructors)
  - [Properties](#properties)
    - [page](#page)
    - [type](#type)
  - [Methods](#methods)

## NoPoDoFo Destination

A destination can be either a [Page](./page.md) or an [Action](./action.md).

```typescript
class Destination {
  new(page: Page, fit: NPDFDestinationFit): Destination
  new(page: Page, fit: NPDFDestinationFit, fitArg: number): Destination
  new(page: Page, left: number, top: number, zoom: number): Destination
  readonly page: Page
  readonly type: NPDFDestinationType
}
```

## Constructors
--------------

Create a new destination. The destination is to the [Page](./page.md) with a fit mode as NPDFDestinationFit.

```typescript
new(page: Page, fit: NPDFDestinationFit): Destination
```

Create a new destination. The destination is to the [Page](./page.md) with a fit mode as NPDFDestinationFit and fit args.

```typescript
new(page: Page, fit: NPDFDestinationFit, fitArg: number): Destination
```

Create a new destination. The destination is to the [Page](./page.md) with specific left and top coordinates and a zoom factor.

```typescript
new(page: Page, left: number, top: number, zoom: number): Destination
```

## Properties
-------------

### page

The page which is the destination.

### type

NPDFDestinationType; the type of destination.

## Methods
------------

