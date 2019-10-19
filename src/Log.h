//
// Created by cmickelson on 5/3/19.
//

#ifndef CONFIGURE_H
#define CONFIGURE_H


#include <napi.h>
#include "spdlog/spdlog.h"

using JsValue = Napi::Value;

namespace NoPoDoFo {

class Log : public Napi::ObjectWrap<Log>{
public:
	static Napi::FunctionReference Constructor;
	static void Initialize(Napi::Env& env, Napi::Object& target);
	explicit Log(const Napi::CallbackInfo&);
	explicit Log(const NoPoDoFo::Log&info) = delete;
	const Log&operator=(const Log&) = delete;
	void InitLog(const Napi::CallbackInfo& info);
	void SetLevel(const Napi::CallbackInfo&, const Napi::Value&);
	JsValue GetLevel(const Napi::CallbackInfo&);
	static void IntervalFlush(const Napi::CallbackInfo&);
private:
	std::shared_ptr<spdlog::logger> Instance;
};

}


#endif //CONFIGURE_H
