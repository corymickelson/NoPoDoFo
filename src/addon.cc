#include "Annotation.h"
#include "CheckBox.h"
//#include "Dictionary.h"
#include "Document.h"
#include "Field.h"
#include "FileSpec.h"
#include "Form.h"
#include "Image.h"
#include "Painter.h"
#include "Rect.h"
#include "TextField.h"

Napi::Object
init(Napi::Env env, Napi::Object exports)
{
  Document::Initialize(env, exports);
  Page::Initialize(env, exports);
  Field::Initialize(env, exports);
  TextField::Initialize(env, exports);
  Image::Initialize(env, exports);
  //  Signature::Initialize(env, exports);
  Annotation::Initialize(env, exports);
  Rect::Initialize(env, exports);
  Painter::Initialize(env, exports);
  CheckBox::Initialize(env, exports);
  Form::Initialize(env, exports);
  Dictionary::Initialize(env, exports);
  FileSpec::Initialize(env, exports);
  NObject::Initialize(env, exports);
  return exports;
}

NODE_API_MODULE(npdf, init);
