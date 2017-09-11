#include "Document.h"
#include "Field.h"
#include "TextField.h"

void
init(Napi::Env env, Napi::Object exports, Napi::Object module)
{
  Document::Initialize(env, exports);
  Page::Initialize(env, exports);
  Field::Initialize(env, exports);
  TextField::Initialize(env, exports);
}

NODE_API_MODULE(npdf, init);
