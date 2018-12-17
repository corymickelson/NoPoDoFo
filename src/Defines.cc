#include "Defines.h"

#ifdef __APPLE__
#include <unistd.h>
#elif defined _WIN32 || defined _WIN64
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#elif defined __linux__
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

namespace NoPoDoFo {
int
FileAccess(std::string& file)
{
  int found = 0;
#ifdef __APPLE__
  if (access(file.c_str(), F_OK) == -1) {
    found = 0;
  }
  found = 1;
#elif defined _WIN32 || defined _WIN64 || defined __linux__
  fs::path p(file.c_str());
  if (fs::exists(p)) {
    found = 1;
  }
#endif
  return found;
}
}