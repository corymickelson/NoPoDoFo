/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
 * Authors: Cory Mickelson, et al.
 *
 * NoPoDoFo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NoPoDoFo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NOPODOFO_DEFINES_H
#define NOPODOFO_DEFINES_H

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#define DEFER(fn) ScopeGuard CONCAT(__defer__, __LINE__) = [&]() { fn; }

#include "base/Color.h"
#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {

enum DocumentInputDevice
{
  Disk,
  Memory
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

#define SetNoPoDoFoColor(info, cb)                                             \
  {                                                                            \
    if (info.IsObject() &&                                                     \
        info.As<Napi::Object>().InstanceOf(Color::constructor.Value())) {      \
      std::cout << "Color" << std::endl;                                       \
      auto color = Color::Unwrap(info.As<Napi::Object>())->color;              \
      if (color->IsRGB()) {                                                    \
        cb(color->GetRed(), color->GetGreen(), color->GetBlue());              \
      } else if (color->IsGrayScale()) {                                       \
        cb(color->GetGrayScale());                                             \
      } else if (color->IsCMYK()) {                                            \
        cb(color->GetCyan(),                                                   \
           color->GetMagenta(),                                                \
           color->GetYellow(),                                                 \
           color->GetBlack());                                                 \
      }                                                                        \
    } else if (info.IsArray()) {                                               \
                                                                               \
      std::cout << "Array" << std::endl;                                       \
      auto js = info.As<Napi::Array>();                                        \
      if (js.Length() == 1) {                                                  \
        double gray = js.Get(static_cast<uint32_t>(0)).As<Napi::Number>();     \
        cb(gray);                                                              \
      } else if (js.Length() == 3) {                                           \
        double r = js.Get(static_cast<uint32_t>(0)).As<Napi::Number>();        \
        double g = js.Get(static_cast<uint32_t>(1)).As<Napi::Number>();        \
        double b = js.Get(static_cast<uint32_t>(2)).As<Napi::Number>();        \
        cb(r, g, b);                                                           \
      } else if (js.Length() == 4) {                                           \
        double c = js.Get(static_cast<uint32_t>(0)).As<Napi::Number>();        \
        double m = js.Get(static_cast<uint32_t>(1)).As<Napi::Number>();        \
        double y = js.Get(static_cast<uint32_t>(2)).As<Napi::Number>();        \
        double k = js.Get(static_cast<uint32_t>(3)).As<Napi::Number>();        \
        cb(c, m, y, k);                                                        \
      }                                                                        \
    }                                                                          \
  }

/**
 *https://oded.blog/2017/10/05/go-defer-in-cpp/
 */
class ScopeGuard
{
public:
  template<class Callable>
  ScopeGuard(Callable&& fn)
    : fn_(std::forward<Callable>(fn))
  {}

  ScopeGuard(ScopeGuard&& other) noexcept
    : fn_(std::move(other.fn_))
  {
    other.fn_ = nullptr;
  }

  ~ScopeGuard()
  {
    if (fn_)
      fn_();
  }

  ScopeGuard(const ScopeGuard&) = delete;

  void operator=(const ScopeGuard&) = delete;

private:
  std::function<void()> fn_;
};
}

#endif // NOPODOFO_DEFINES_H
