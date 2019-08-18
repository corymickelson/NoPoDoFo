/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2019
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

#include "XObject.h"
#include "../doc/Rect.h"
#include "Obj.h"
#include "Ref.h"
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

namespace NoPoDoFo {

FunctionReference XObject::Constructor; // NOLINT

XObject::XObject(const CallbackInfo& info)
  : ObjectWrap(info)
{
  Log = spdlog::get("Log");
  // create an xobject from an existing object (must be an xobject)
  if (info[0].IsExternal()) {
    if(Log != nullptr) Log->debug("XObject copy");
    auto copy = info[0].As<External<PdfXObject>>().Data();
    Self = new PdfXObject(*copy);
  }
  // create new XObject
  else if (info.Length() && info[0].IsObject() &&
           info[0].As<Object>().InstanceOf(Rect::constructor.Value()) &&
           info[1].IsExternal()) {
    auto rect = Rect::Unwrap(info[0].As<Object>());
    if(Log != nullptr) Log->debug("New XObject");
    PdfDocument* doc = info[1].As<External<PdfDocument>>().Data();
    Self = new PdfXObject(rect->GetRect(), doc);
  }
}
XObject::~XObject()
{
  if(Log != nullptr) Log->debug("XObject Cleanup");
  HandleScope scope(Env());
  delete Self;
}

void
XObject::Initialize(Napi::Env& env, Object& target)
{
  HandleScope scope(env);
  Function ctor = DefineClass(
    env,
    "XObject",
    { InstanceAccessor("contents", &XObject::GetContents, nullptr),
      InstanceAccessor("reference", &XObject::Reference, nullptr),
      InstanceAccessor(
        "contentsForAppending", &XObject::GetContentsForAppending, nullptr),
      InstanceAccessor("resource", &XObject::GetResources, nullptr),
      InstanceAccessor("pageMediaBox", &XObject::GetPageSize, nullptr) });
  Constructor = Napi::Persistent(ctor);
  Constructor.SuppressDestruct();
  target.Set("XObject", ctor);
}

Value
XObject::GetContents(const CallbackInfo& info)
{
  return Obj::Constructor.New(
    { External<PdfObject>::New(info.Env(), Self->GetContents()) });
}

Value
XObject::GetContentsForAppending(const CallbackInfo& info)
{
  return Obj::Constructor.New(
    { External<PdfObject>::New(info.Env(), Self->GetContentsForAppending()) });
}

Value
XObject::GetResources(const CallbackInfo& info)
{
  return Obj::Constructor.New(
    { External<PdfObject>::New(info.Env(), Self->GetResources()) });
}

Value
XObject::GetPageSize(const CallbackInfo& info)
{
  PdfRect rect = Self->GetPageSize();
  double left, bottom, width, height;
  left = rect.GetLeft();
  bottom = rect.GetBottom();
  width = rect.GetWidth();
  height = rect.GetHeight();

  return Rect::constructor.New({ Number::New(info.Env(), left),
                                 Number::New(info.Env(), bottom),
                                 Number::New(info.Env(), width),
                                 Number::New(info.Env(), height) });
}
Napi::Value
XObject::Reference(const Napi::CallbackInfo& info)
{
  auto r = Self->GetObject()->Reference();
  return Ref::Constructor.New({ External<PdfReference>::New(info.Env(), &r) });
}
}
