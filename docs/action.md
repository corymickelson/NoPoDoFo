# API Documentation for Action

* [Properties](#properties)
  * [type](#type)
  * [uri](#uri)
  * [script](#script)
* [Methods](#methods)
  * [getObject](#getobject)
  * [addToDictionary](#addtodictionary)

## NoPoDoFo Action

A Document action. An action is used in conjunction with an [Annotation](./annotations.md) or a [Outline](./outline.md). PDF supports a variety
of action types; NPDFActions. See [Bookmarks](./cookbook/bookmark.md) for action examples.

```typescript
export class Action {
    new(type: NPDFActions, doc: Document): Action
    readonly type: NPDFActions
    uri?: string
    script?: string

    getObject(): Object
    addToDictionary(dictionary: Dictionary): void
}
```

## Constructors

```typescript
new Action(t: NPDFActions, doc: Document): Action
```
Create a new action of type NPDFActions

## Properties

### type
This is a readonly property returning the NPDFActions type. An actions `Type` must be defined at the time of creation.

### uri
This property gets or sets the `URI` property of the action [Dictionary](./dictionary.md). This is a required field when the NPDFActions type is
a `URI` action. 

### script
This is a required property of a `JavaScript` type action. This script will be executed when the action is activated. Please familiarize yourself with the Adobe Acrobat JavaScript API Reference  Materials.

## Methods

### getObject
Get the underlying action [Object](./object.md).

### addToDictionary
Handles properly adding the action to a [Dictionary](./dictionary.md).
