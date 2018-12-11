# DataTypes

This is an overview of some of the data types used in NoPoDoFo PDF programming. Most data types in NoPoDoFo
will be familiar to you, some however do not fit well within the NodeJs / Javascript language and are presented here with
examples as well as why and how they are used.

## External

The `external` type represents a pointer to an object outside v8 memory. This is not a javascript type and can not be
inspected by javascript utilities. The primary use case for `external` is for passing a POCO item(s) to/from native methods via a pointer
to the external object.