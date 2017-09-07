//
// Created by red on 9/6/17.
//

#ifndef NPDF_LIBRARY_HPP
#define NPDF_LIBRARY_HPP

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace std;

#define DECLARE_NAPI_METHOD(name, fn)                                          \
  { name, fn, 0, 0, 0, napi_defualt, 0 }

void CheckInfoArgsLength(const CallbackInfo &info, const int expectedLength) {
  if (info.Length() < static_cast<size_t>(expectedLength)) {
    stringstream ss;
    ss << "Expected " << to_string(expectedLength) << " args but got "
       << to_string(info.Length()) << endl;
    throw Napi::Error::New(info.Env(), ss.str());
  }
}

#endif // NPDF_LIBRARY_HPP
