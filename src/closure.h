//
// Created by red on 9/10/17.
//

#ifndef NPDF_CLOSURE_HPP
#define NPDF_CLOSURE_HPP

#include "doc/Document.h"
#include <napi.h>
#include <sys/user.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

typedef struct closure_t
{
  Napi::FunctionReference *ptr;
  Napi::Function fn;
  unsigned len;
  unsigned max_len;
  Document *document;
  uint8_t *data;
} closure_t;

bool
InitClosure(closure_t *closure, Document *document)
{
  closure->len = 0;
  closure->document = document;
  closure->data = (uint8_t *) malloc(closure->max_len = PAGE_SIZE);
  return closure->data != nullptr;
}

void
DestroyClosure(closure_t *closure)
{
  if (closure->len)
  {
    free(closure->data);
  }
}
#endif // NPDF_CLOSURE_HPP
