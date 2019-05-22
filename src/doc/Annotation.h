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

#ifndef NPDF_ANNOTATION_H
#define NPDF_ANNOTATION_H

#include <napi.h>
#include <podofo/podofo.h>
#include <spdlog/logger.h>

using namespace Napi;

using JsValue = Napi::Value;

namespace NoPoDoFo {

class Annotation : public ObjectWrap<Annotation>
{
public:
  explicit Annotation(const CallbackInfo& callbackInfo);
  explicit Annotation(const Annotation&) = delete;
  const Annotation& operator=(const Annotation&) = delete;
  ~Annotation();
  static FunctionReference Constructor;
  static void Initialize(Napi::Env& env, Napi::Object& target);
  JsValue HasAppearanceStream(const CallbackInfo&);
  void SetAppearanceStream(const CallbackInfo&);
  void SetRect(const CallbackInfo&, const JsValue&);
  JsValue GetRect(const CallbackInfo&);
  void SetFlags(const CallbackInfo&, const JsValue&);
  JsValue GetFlags(const CallbackInfo&);
  void SetBorderStyle(const CallbackInfo&);
  void SetTitle(const CallbackInfo&, const JsValue&);
  JsValue GetTitle(const CallbackInfo&);
  void SetContent(const CallbackInfo&, const JsValue&);
  JsValue GetContent(const CallbackInfo&);
  void SetDestination(const CallbackInfo&, const JsValue&);
  JsValue GetDestination(const CallbackInfo&);
  void SetAction(const CallbackInfo&, const JsValue&);
  JsValue GetAction(const CallbackInfo&);
  void SetOpen(const CallbackInfo&, const JsValue&);
  JsValue GetOpen(const CallbackInfo&);
  void SetColor(const CallbackInfo&, const JsValue&);
  JsValue GetColor(const CallbackInfo&);
  JsValue GetType(const CallbackInfo&);
  void SetQuadPoints(const CallbackInfo&, const JsValue&);
  JsValue GetQuadPoints(const CallbackInfo&);
  JsValue GetAttachment(const CallbackInfo&);
  void SetAttachment(const CallbackInfo&, const JsValue&);

  PoDoFo::PdfAnnotation& GetAnnotation() const { return Self; }

private:
  PoDoFo::PdfAnnotation& Self;
  std::shared_ptr<spdlog::logger> DbgLog;
};
}
#endif // NPDF_ANNOTATION_H
