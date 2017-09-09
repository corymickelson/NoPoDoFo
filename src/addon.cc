#include "pdfDocument.hpp"

void
init(Napi::Env env, Napi::Object exports, Napi::Object module)
{
  Document::Initialize(env, exports);
  Page::Initialize(env, exports);
}

NODE_API_MODULE(npdf, init);
