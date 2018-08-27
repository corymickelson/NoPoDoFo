# API Documentation for Outline

* [Properties](#properties)
  * [prev](#prev)
  * [next](#next)
  * [first](#first)
  * [last](#last)
  * [destination](#destination)
  * [action](#action)
  * [title](#title)
  * [textFormat](#textformat)
  * [textColor](#textcolor)
* [Methods](#methods)
  * [createChild](#createchild)
  * [createNext](#createnext)
  * [insertItem](#insertitem)
  * [getParent](#getparent)
  * [erase](#erase)

## NoPoDoFo Outline

An Outline is a tree type doubly linked list data structure providing an interactive view of the [Document](./document.md). Each level of the tree
may contain child items that also form a doubly linked list. Navigating the tree is done through the [first](#first), [last](#last), [next](#next) and
[prev](#prev) properties of an Outline instance. See the [Bookmark Cookbook](./cookbook/bookmark.md) for examples.

```typescript
class Outline {
  readonly prev?: Outline
  readonly next?: Outline
  readonly first?: Outline
  readonly last?: Outline
  destination: Destination
  action: Action
  title: string
  textFormat: number
  textColor: NPDFColor
  createChild(name: string, value: Destination): Outline
  createNext(name: string, value: Destination | Action): Outline
  insertItem(item: Object): void
  getParent(): Outline
  erase(): void
}
```

## Constructor

NoPoDoFo does not support a default constructor for creating Outline objects. In order to begin an outline you must start at the root.
Creating a root is accomplished by [Document.getOutline](./document.md#getoutline), subsequent nodes are created using the [createChild](#createchild)
and [createNext](#createnext) methods.

## Properties

### prev

Get the previous outline item or null

### next

Get the next outline item or null

### first

Get the first outline item that is a child of this item

### last

Get the last outline item that is a child of this item

### destination

Outline [Destination](./destination.md) accessor, get and set the destination of the outline item

### action

Outline [Action](./action.md) accessor, get and set the action of the outline item

### title

Get and set the title of the outline item

### textFormat

Get and set the format of the title of the outline item

### textColor

Get and set the text color as NPDFColor of the title of the outline item

## Methods

### createChild

```typescript
createChild(name: string, value: Destination): Outline
```

Create a child of this outlien item

### createNext

```typescript
createNext(name: string, value: Destination | Action): Outline
```

Create the next item in the doubly linked list

### insertItem

```typescript
insertItem(item: Object): void
```

Insert a new outline item as a child of this item. The new item can not be in the same tree as this item as that would
break the tree structure.

### getParent

```typescript
getParent(): Outline
```

Get the parent Outline item of this item

### erase

```typescript
erase(): void
```

Erase this item and children of this item.
