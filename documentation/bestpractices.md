# NoPoDoFo Best Practices

NoPoDoFo was built for the purpose of providing a low level api for PDF creation and manipulation.
While performance is important, performance is not the primary goal of NoPoDoFo.

Their is a trade off in the grainularity of the api and performance. Calling into NoPoDoFo must cross the
Javascript | C++ barrier which adds overhead to each NoPoDoFo method invokation. The overhead of most NoPoDoFo api's
are typically negligable but should be considered if performance is your primary objective.

Performance overhead comes in the form of copying data from one runtime to the other, this copy takes place in both directions,
from Javascript to C++ and from C++ back to Javascript.

NoPoDoFo attempts to limit this copy overhead by keeping as much of the PDF as possible in external memory; memory outside v8.
NoPoDoFo api's where applicable do not return a value to the Javascript runtime(v8) but accept an argument(value) from the Javascript
runtime and apply that value to the PDF held in external memory. For example, when setting the stroking width:

```typescript
painter.setStokingWidth(0.5)
```

The value `0.5` is passed to a method on the C++ NoPoDoFo::Painter object which converts the Javascript number to a C++ float and applies
that number to the PoDoFo::Painter instance in memory, no value is returned from this method.

There are however a handful of NoPoDoFo operations that can be somewhat expensive in terms of copy overhead, one such method being
`Document.body`. The Document body accessor iterates each object of the PDF Document creating a new NoPoDoFo::Object and returning
all NoPoDoFo::Objects as a new Javascript Array.

The NoPoDoFo low level api's, specifically [Dictionary](./dictionary.md) and [Array](./array.md) expose methods for iterating the
values of the containter. When iterating the keys of an array, or the key(s)/value(s) of a dictionary the caller must check
that the return value has been resolved, if the value has not been resolved a [Ref](./ref.md) will be returned. The caller then
can take this ref and pass it to [document.getObject](./document.md#getobject) to get the actual object value. Examples of this can be
seen in unit tests Image, and FileAttachment.

## Where NoPoDoFo breaks from Javascript

NoPoDoFo is a C++ library, and as such it is responsible for the management of it's own memory. NoPoDoFo follows the
Aggregation pattern, in this pattern the "parent" object may hold ownership of it's child object's resources.
The effect that this pattern has on NoPoDoFo is that a child class of a Document/StreamDocument, ex a Page, is bound to 
the lifetime of the parent, the child is not responsible for it's resource allocation(s) or destruction,
when the parent goes out of scope(allowing the instance to be gc'd) the child will be destroyed along with the parent,
trying to access the instance of Page after the Document has gone out of scope(and been collected) will result in an Error. 
