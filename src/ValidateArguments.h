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

#include <napi.h>
#include <sstream>
#include <string>
#include <vector>

using namespace Napi;
using namespace std;

static void
AssertFunctionArgs(const CallbackInfo& info,
                   int expectedCount,
                   const vector<napi_valuetype>& expectedTypes)
{
  stringstream eMsg;
  if (info.Length() < static_cast<size_t>(expectedCount)) {
    eMsg << "Expected " << expectedCount << " but received " << info.Length()
         << endl;
    throw Napi::Error::New(info.Env(), eMsg.str());
  }
  for (size_t i = 0; i < info.Length(); ++i) {
    if (expectedTypes.size() - 1 == i)
      break;
    napi_valuetype expected = expectedTypes[i];
    napi_valuetype actual = info[i].Type();
    if (actual != expected) {
      eMsg << "Expected parameter[" << i << "] to be of type: " << expected
           << " but received: " << actual << endl;
      throw Napi::Error::New(info.Env(), eMsg.str());
    }
  }
}

#endif // NPDF_VALIDATEARGUMENTS_H
