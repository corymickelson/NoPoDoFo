//
// Created by cmickelson on 5/3/19.
//

#ifndef CONFIGURE_H
#define CONFIGURE_H


#include <napi.h>
#include "spdlog/spdlog.h"

using JsValue = Napi::Value;

namespace NoPoDoFo {

class Configure : public Napi::ObjectWrap<Configure>{
public:
	static Napi::FunctionReference Constructor;
	static void Initialize(Napi::Env& env, Napi::Object& target);
	explicit Configure(const Napi::CallbackInfo&);
	explicit Configure(const NoPoDoFo::Configure &info) = delete;
	const Configure&operator=(const Configure&) = delete;
	void InitLog(const Napi::CallbackInfo& info);
	void SetLevel(const Napi::CallbackInfo&, const Napi::Value&);
	JsValue GetLevel(const Napi::CallbackInfo&);
	void IntervalFlush(const Napi::CallbackInfo&);
private:
	std::shared_ptr<spdlog::logger> Log;
};

}


#endif //CONFIGURE_H
