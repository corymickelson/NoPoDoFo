#include "base/Array.h"
#include "base/Dictionary.h"
#include "base/Obj.h"
#include "base/Ref.h"
#include "base/Stream.h"
#include "doc/Annotation.h"
#include "doc/CheckBox.h"
#include "doc/Document.h"
#include "doc/Encoding.h"
#include "doc/Encrypt.h"
#include "doc/ExtGState.h"
#include "doc/Field.h"
#include "doc/FileSpec.h"
#include "doc/Font.h"
#include "doc/Form.h"
#include "doc/Image.h"
#include "doc/ListField.h"
#include "doc/Painter.h"
#include "doc/Rect.h"
#include "doc/SignatureField.h"
#include "doc/Signer.h"
#include "doc/TextField.h"
#include "crypto/Signature.h"
#include "base/Vector.h"
#include <napi.h>

Napi::Object
init(Napi::Env env, Napi::Object exports)
{
  NoPoDoFo::Document::Initialize(env, exports);
  NoPoDoFo::Page::Initialize(env, exports);
  NoPoDoFo::Field::Initialize(env, exports);
  NoPoDoFo::TextField::Initialize(env, exports);
  NoPoDoFo::Image::Initialize(env, exports);
  NoPoDoFo::Annotation::Initialize(env, exports);
  NoPoDoFo::Rect::Initialize(env, exports);
  NoPoDoFo::Painter::Initialize(env, exports);
  NoPoDoFo::CheckBox::Initialize(env, exports);
  NoPoDoFo::Form::Initialize(env, exports);
  NoPoDoFo::Dictionary::Initialize(env, exports);
  NoPoDoFo::FileSpec::Initialize(env, exports);
  NoPoDoFo::Obj::Initialize(env, exports);
  NoPoDoFo::Ref::Initialize(env, exports);
  NoPoDoFo::Array::Initialize(env, exports);
  NoPoDoFo::Stream::Initialize(env, exports);
  NoPoDoFo::Encrypt::Initialize(env, exports);
  NoPoDoFo::ListField::Initialize(env, exports);
  NoPoDoFo::Font::Initialize(env, exports);
  NoPoDoFo::Encoding::Initialize(env, exports);
  NoPoDoFo::ExtGState::Initialize(env, exports);
  NoPoDoFo::Signer::Initialize(env, exports);
  NoPoDoFo::SignatureField::Initialize(env, exports);
  NoPoDoFo::Vector::Initialize(env, exports);

  exports["signature"] = Function::New(env, NPDFSignatureData);

  return exports;
}

NODE_API_MODULE(npdf, init);
