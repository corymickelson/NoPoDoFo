//
// Created by cory on 7/6/18.
//

#ifndef NOPODOFO_DEFINES_H
#define NOPODOFO_DEFINES_H

#include <napi.h>
#include <podofo/podofo.h>

namespace NoPoDoFo {

enum DocumentStorageDevice
{
  InMemory,
  StreamToDisk
};

enum DocumentInputDevice
{
  Disk,
  Memory
};

struct OptionalArgument
{
  bool emptyOk = false;
  napi_valuetype T;
};

#define TryLoad(doc, file, buffer, pwd, forUpdate, typeE)                      \
  {                                                                            \
    try {                                                                      \
      switch (typeE) {                                                         \
        case DocumentInputDevice::Disk:                                        \
          (doc).Load((file).c_str(), forUpdate);                               \
          break;                                                               \
        case DocumentInputDevice::Memory:                                      \
          (doc).LoadFromDevice(buffer, forUpdate);                             \
          break;                                                               \
      }                                                                        \
    } catch (PdfError & e) {                                                   \
      if (e.GetError() == ePdfError_InvalidPassword) {                         \
        if ((pwd).empty()) {                                                   \
          SetError("Password required to modify this document");               \
        } else {                                                               \
          try {                                                                \
            (doc).SetPassword(pwd);                                            \
          } catch (...) {                                                      \
            SetError("Password Invalid");                                      \
          }                                                                    \
        }                                                                      \
      } else {                                                                 \
        SetError(ErrorHandler::WriteMsg(e));                                   \
      }                                                                        \
    }                                                                          \
  }
}
#endif // NOPODOFO_DEFINES_H
