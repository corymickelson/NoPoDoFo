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
template<typename... Ts>
void
Logger(std::shared_ptr<spdlog::logger> logger,
       spdlog::level::level_enum level,
       const std::string& msg,
       Ts... ts)
{
  if (logger != nullptr) {
    logger->log(level, msg, msgArg(ts...));
  }
}
template<typename... Ts>
void
Logger(const std::string& logger,
       spdlog::level::level_enum level,
       const std::string& msg,
       Ts... ts)
{
  if (spdlog::get(logger) != nullptr) {
    spdlog::get(logger)->log(level, msg, msgArg(ts...));
  }
}
template<typename... Ts>
void
Logger(std::shared_ptr<spdlog::logger> logger,
       spdlog::level::level_enum level,
       Napi::Env env,
       const std::string& msg,
       Ts... ts)
{
  if (logger != nullptr) {
    logger->log(level, msg, msgArg(ts...));
    if (level == spdlog::level::err) {
      Error::New(env, msg).ThrowAsJavaScriptException();
    }
  }
}
template<typename... Ts>
void
Logger(const std::string& logger,
       spdlog::level::level_enum level,
       Napi::Env env,
       const std::string& msg,
       Ts... ts)
{
  if (spdlog::get(logger) != nullptr) {
    spdlog::get(logger)->log(level, msg, msgArg(ts...));
    if (level == spdlog::level::err) {
      Error::New(env, msg).ThrowAsJavaScriptException();
    }
  }
}
template<typename T>
double
msgArg(T t)
{
  return t;
}
template<typename T>
float
msgArg(T t)
{
  return t;
}
template<typename T>
int
msgArg(T t)
{
  return t;
}
template<typename T>
uint
msgArg(T t)
{
  return t;
}
template<typename T>
bool
msgArg(T t)
{
  return t;
}
template<typename T>
std::string
msgArg(T t)
{
  return t;
}

}
