#include "base/Arr.h"
#include "base/Dictionary.h"
#include "base/Obj.h"
#include "base/Ref.h"
#include "base/Stream.h"
#include "doc/Annotation.h"
#include "doc/CheckBox.h"
#include "doc/Document.h"
#include "doc/Encrypt.h"
#include "doc/Field.h"
#include "doc/FileSpec.h"
#include "doc/Form.h"
#include "doc/Image.h"
#include "doc/Painter.h"
#include "doc/Rect.h"
#include "doc/TextField.h"
#include <napi.h>

Napi::Object
init(Napi::Env env, Napi::Object exports)
{
  Document::Initialize(env, exports);
  Page::Initialize(env, exports);
  Field::Initialize(env, exports);
  TextField::Initialize(env, exports);
  Image::Initialize(env, exports);
  Annotation::Initialize(env, exports);
  Rect::Initialize(env, exports);
  Painter::Initialize(env, exports);
  CheckBox::Initialize(env, exports);
  Form::Initialize(env, exports);
  Dictionary::Initialize(env, exports);
  FileSpec::Initialize(env, exports);
  Obj::Initialize(env, exports);
  Ref::Initialize(env, exports);
  Arr::Initialize(env, exports);
  Stream::Initialize(env, exports);
  Encrypt::Initialize(env, exports);
  return exports;
}

NODE_API_MODULE(npdf, init);
