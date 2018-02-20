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

#ifndef NPDF_ANNOTATION_H
#define NPDF_ANNOTATION_H

#include "Document.h"
#include "Page.h"
#include "Rect.h"
#include <napi.h>
#include <podofo/podofo.h>
#include <vector>

using namespace std;
using namespace PoDoFo;

namespace NoPoDoFo {
class Annotation : public ObjectWrap<Annotation>
{
public:
  explicit Annotation(const CallbackInfo& callbackInfo);
  ~Annotation();

  static FunctionReference constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target)
  {
    HandleScope scope(env);
    Function ctor = DefineClass(
      env,
      "Annotation",
      { InstanceAccessor("flags", &Annotation::GetFlags, &Annotation::SetFlags),
        InstanceMethod("hasAppearanceStream", &Annotation::HasAppearanceStream),
        InstanceMethod("setBorderStyle", &Annotation::SetBorderStyle),
        InstanceAccessor("title", &Annotation::GetTitle, &Annotation::SetTitle),
        InstanceAccessor(
          "content", &Annotation::GetContent, &Annotation::SetContent),
        InstanceAccessor("destination",
                         &Annotation::GetDestination,
                         &Annotation::SetDestination),
        InstanceMethod("hasDestination", &Annotation::HasDestination),
        InstanceMethod("hasAction", &Annotation::HasAction),
        InstanceMethod("setAction", &Annotation::SetAction),
        InstanceMethod("getAction", &Annotation::GetAction),
        InstanceAccessor("open", &Annotation::GetOpen, &Annotation::SetOpen),
        InstanceMethod("getType", &Annotation::GetType),
        InstanceAccessor("color", &Annotation::GetColor, &Annotation::SetColor),
        InstanceAccessor(
          "quadPoints", &Annotation::GetQuadPoints, &Annotation::SetQuadPoints),
        InstanceMethod("setFileAttachment", &Annotation::SetFileAttachment),
        InstanceMethod("hasFileAttachment", &Annotation::HasFileAttachment) });

    constructor = Persistent(ctor);
    constructor.SuppressDestruct();
    target.Set("Annotation", ctor);
  }
  Napi::Value HasAppearanceStream(const CallbackInfo&);
  void SetFlags(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetFlags(const CallbackInfo&);
  void SetBorderStyle(const CallbackInfo&);
  void SetTitle(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetTitle(const CallbackInfo&);
  void SetContent(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetContent(const CallbackInfo&);
  void SetDestination(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetDestination(const CallbackInfo&);
  Napi::Value HasDestination(const CallbackInfo&);
  void SetAction(const CallbackInfo&);
  Napi::Value GetAction(const CallbackInfo&);
  Napi::Value HasAction(const CallbackInfo&);
  void SetOpen(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetOpen(const CallbackInfo&);
  void SetColor(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetColor(const CallbackInfo&);
  Napi::Value GetType(const CallbackInfo&);
  void SetQuadPoints(const CallbackInfo&, const Napi::Value&);
  Napi::Value GetQuadPoints(const CallbackInfo&);
  void SetFileAttachment(const CallbackInfo&);
  Napi::Value HasFileAttachment(const CallbackInfo&);

  PdfAnnotation* GetAnnotation() { return annot; }

private:
  PdfAnnotation* annot;
  //  Document* doc;
};
}
#endif // NPDF_ANNOTATION_H
