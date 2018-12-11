# Appearance Stream (AP) Cookbook

## Text Field Appearance Stream

To set the appearance of an interactive form field; the Portable Document Format offers two means of defining how
an interactive value may be shown by a pdf reader. The first and easiest is to utilize the default appearance.
The default appearance for NoPoDoFo is 12pt black ariel. To set your own custom text field styles please follow the
below template.

```typescript
const doc = new nopodofo.Document()
doc.load('/path/to/doc.pdf', e => {
  if (e) {
    // handle error
  }
  // get the text field(s)
  const p1 = doc.getPage(0)
  const value = 'TESTING'
  const tfield = p1.getField<nopodofo.TextField>(0)
})
```