//
// Created by red on 9/10/17.
//

#ifndef NPDF_VALIDATEARGUMENTS_H
#define NPDF_VALIDATEARGUMENTS_H

#include <napi.h>
#include <vector>
#include <string>
#include <sstream>

using namespace Napi;
using namespace std;

static void
AssertFunctionArgs(const CallbackInfo &info,
                   int expectedCount,
                   const vector<napi_valuetype> &expectedTypes)
{
  stringstream eMsg;
  if (info.Length() < expectedCount)
  {
    eMsg << "Expected " << expectedCount << " but received " << info.Length() << endl;
    throw Napi::Error::New(info.Env(), eMsg.str());
  }
  for (int i = 0; i < info.Length(); ++i)
  {
    if (expectedTypes.size() - 1 == i)
      break;
    napi_valuetype expected = expectedTypes[i];
    napi_valuetype actual = info[i].Type();
    if (actual != expected)
    {
      eMsg << "Expected parameter[" << i << "] to be of type: " << expected << " but received: " << actual << endl;
      throw Napi::Error::New(info.Env(), eMsg.str());
    }
  }
}

#endif //NPDF_VALIDATEARGUMENTS_H
