#include "pdfDocument.h"

void init(Napi::Env env, Napi::Object exports, Napi::Object module) {
  pdfDocument::Initialize(env, exports);
}

NODE_API_MODULE(npdf, init);
