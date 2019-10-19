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

#include "ValidateArguments.h"
#include "Defines.h"
#include "spdlog/spdlog.h"
#include <sstream>

using std::endl;
using std::map;
using std::stringstream;
using std::vector;

namespace NoPoDoFo {

std::vector<int>
AssertCallbackInfo(const Napi::CallbackInfo& info,
                   const std::map<int, std::vector<option>>& vars)
{
  vector<int> argIndex;
  for (auto item : vars) {
    auto valid = false;
    if (static_cast<int>(info.Length()) - 1 < item.first) {
      for (size_t i = 0; i < item.second.size(); i++) {
        if (!item.second[i].has_value()) {
          valid = true;
          argIndex.emplace_back(i);
          break;
        }
      }
      if (!valid) {
        stringstream eMsg;
        eMsg << "Expected " << vars.size() << " parameters but received "
             << info.Length() << endl;
        Logger("Log", spdlog::level::err, eMsg.str().c_str());
        Napi::Error::New(info.Env(), eMsg.str()).ThrowAsJavaScriptException();
        return {};
      }
    } else {
      const auto t = info[static_cast<size_t>(item.first)].Type();
      for (size_t i = 0; i < static_cast<size_t>(item.second.size()); i++) {
        auto opt = item.second[i];
        if (opt.has_value() && opt.value() == t) {
          valid = true;
          argIndex.emplace_back(i);
          break;
        }
      }
      if (!valid) {
        stringstream eMsg;
        eMsg << "Invalid function argument at index " << item.first << endl;
        Logger("Log", spdlog::level::err, eMsg.str().c_str());
        Napi::TypeError::New(info.Env(), eMsg.str())
          .ThrowAsJavaScriptException();
        return {};
      }
    }
  }
  return argIndex;
}
}
