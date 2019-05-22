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
#ifndef NPDF_VALIDATEARGUMENTS_H
#define NPDF_VALIDATEARGUMENTS_H

#include <map>
#include <napi.h>
#include <optional/optional.hpp>
#include <vector>

namespace NoPoDoFo {

using option = tl::optional<napi_valuetype>;
/**
 * Assert caller argument(s). Since arguments can be variable the assertion
 * checks the type for each index and returns an int array of matching values.
 *
 * @example
 *      std::map validOptions;
 *      validOptions.insert(std::make_pair(0, {napi_valuetype::napi_external,
 *          napi_valuetype::napi_object}))
 *      validOptions.insert(std::make_pair(1,
 *          {napi_valuetype::napi_string, napi_valuetype::napi_function}))
 *      validOptions.insert(std::make_pair(2, {napi_valuetype::napi_null,
 *          napi_valuetype::napi_function}))
 *      vector<int> opt = AssertCallbackInfo(info,
 *          validOptions) // {0,0,0} == napi_external, napi_string, napi_null
 *
 * @param info
 * @param vars
 * @return
 */
std::vector<int>
AssertCallbackInfo(const Napi::CallbackInfo& info,
                   const std::map<int, std::vector<option>>& vars);
}

#endif // NPDF_VALIDATEARGUMENTS_H
