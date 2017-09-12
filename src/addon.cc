#include "Document.h"
#include "Field.h"
#include "TextField.h"
#include "Image.h"
#include "Annotation.h"
#include "Signature.h"
#include "Rect.h"

void
init(Napi::Env env, Napi::Object exports, Napi::Object module)
{
  Document::Initialize(env, exports);
  Page::Initialize(env, exports);
  Field::Initialize(env, exports);
  TextField::Initialize(env, exports);
  Image::Initialize(env, exports);
  Signature::Initialize(env, exports);
  Annotation::Initialize(env, exports);
  Rect::Initialize(env, exports);
}

NODE_API_MODULE(npdf, init);
