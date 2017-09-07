//
// Created by red on 9/6/17.
//

#ifndef NPDF_LIBRARY_HPP
#define NPDF_LIBRARY_HPP

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace std;

#define DECLARE_NAPI_METHOD(name, fn) { name, fn, 0, 0, 0, napi_defualt, 0 }

#endif //NPDF_LIBRARY_HPP
