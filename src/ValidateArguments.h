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
#ifndef NPDF_VALIDATEARGUMENTS_H
#define NPDF_VALIDATEARGUMENTS_H

#include <iostream>
#include <napi.h>
#include <sstream>

using std::endl;
using std::stringstream;


/**
 * @note Called with a list of all possible argument configurations, and returns
 * the index of the matching configuration.
 * @example AssertFunctionArgs(info, 1, [[napi_function], [napi_string,
 * napi_function]]) if called with JS whatever('foo', () => {}), than
 * AssertFunctionArgs would return 1, since it's matching the 1st index. An
 * invalid result == -1
 * @param info
 * @param minimumArgCount
 * @param expectedTypes
 * @return
 */
static int
AssertFunctionArgs(
  const Napi::CallbackInfo& info,
  size_t minimumArgCount,
  const std::vector<std::vector<napi_valuetype>>& expectedTypes,
  const char* msg)
{
  stringstream eMsg;
  int index = -1;
  if (info.Length() < minimumArgCount) {
    eMsg << "Expected " << minimumArgCount << " but received " << info.Length()
         << endl;
    msg = eMsg.str().c_str();
  } else {
    for (const auto& opt : expectedTypes) {
      for (size_t i = 0; i < opt.size(); ++i) {
        if (info[i].Type() != opt[i]) {
          break;
        } else {
          if (i == info.Length() - 1) { // last iteration
            index = static_cast<int>(i);
          }
        }
      }
    }
  }
  Napi::Object::IsBuffer();
  if (index == -1) {
    Napi::Error::New(info.Env(), msg).ThrowAsJavaScriptException();
  }
  return index;
}

#endif // NPDF_VALIDATEARGUMENTS_H
