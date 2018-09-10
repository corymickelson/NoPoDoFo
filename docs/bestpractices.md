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
runtime and apply that value to the PDF held in external memory. For example, when setting the background color:

```typescript
painter.setColor([0.0, 0.0, 0.0])
```

The value `[0.0, 0.0, 0.0]` is passed to a method on the C++ NoPoDoFo::Painter object which converts the array to a PDF Color and applies
that color the PoDoFo::Painter instance in memory, no value is returned from this method.

There are however a handful of NoPoDoFo operations that can be somewhat expensive in terms of copy overhead, one such method being
`Document.body`. The Document body accessor iterates each object of the PDF Document creating a new NoPoDoFo::Object and returning
all NoPoDoFo::Objects as a new Javascript Array.
