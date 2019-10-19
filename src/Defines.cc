#include "Defines.h"
#include "spdlog/spdlog.h"
#include <iostream>

namespace NoPoDoFo {
int
FileAccess(std::string& file)
{
  auto Log = spdlog::get("Log");
  std::stringstream msg;
  msg << "Attempting to access file " << file;
  if (Log != nullptr)
    Log->debug(msg.str());
  auto found = 0;
#ifdef __APPLE__
  if (access(file.c_str(), F_OK) == -1) {
    found = 0;
  }
  found = 1;
#elif defined _WIN32 || defined _WIN64 || defined __linux__
  const fs::path p(file.c_str());
  if (fs::exists(p)) {
    found = 1;
  }
#endif
  if (found == 0) {
    if (Log != nullptr)
      Log->debug("file not found");
  } else {
    if (Log != nullptr)
      Log->debug("file found");
  }
  return found;
}


}
