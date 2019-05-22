#include "Defines.h"
#include "spdlog/spdlog.h"
#include <iostream>

namespace NoPoDoFo {
int
FileAccess(std::string& file)
{
	auto dbgLog = spdlog::get("DbgLog");
	std::stringstream msg;
	msg << "Attempting to access file " << file;
	if(dbgLog != nullptr) dbgLog->debug(msg.str());
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
  if(found == 0) {
  	if(dbgLog != nullptr)dbgLog->debug("file not found");
  } else {
  	if(dbgLog != nullptr)dbgLog->debug("file found");
  }
  return found;
}
/**
 * https://oded.blog/2017/10/05/go-defer-in-cpp/
 * Scope Guard, based off of go's defer
 */
class ScopeGuard
{
public:
  template<class Callable>
  explicit ScopeGuard(Callable&& fn)
    : fn_(std::forward<Callable>(fn))
  {}

  ScopeGuard(ScopeGuard&& other) noexcept
    : fn_(std::move(other.fn_))
  {
    other.fn_ = nullptr;
  }

  ~ScopeGuard()
  {
    if (fn_)
      fn_();
  }

  ScopeGuard(const ScopeGuard&) = delete;

  void operator=(const ScopeGuard&) = delete;

private:
  std::function<void()> fn_;
};
}
