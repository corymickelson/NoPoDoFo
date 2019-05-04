#include "Configure.h"

using namespace Napi;

namespace NoPoDoFo {

FunctionReference Configure::Constructor; // NOLINT

void
Configure::Initialize(Napi::Env &env,
											Napi::Object &target)
{
	HandleScope scope(env);
	const char* klass = "Configure";
	auto ctor = DefineClass(
		env,
		klass,
		{
			InstanceAccessor("enableDebugLogging",
											 &Configure::GetDebugLogging,
											 &Configure::EnableDebugLogging)});
	Constructor = Persistent(ctor);
	Constructor.SuppressDestruct();
	target.Set(klass,
						 ctor);
}
Configure::Configure(const Napi::CallbackInfo &info)
	: ObjectWrap(info)
{
	DbgLog = spdlog::get("DbgLog");
}
void
Configure::EnableDebugLogging(const CallbackInfo &info,
															const JsValue &value)
{
	if (value.IsBoolean() && value.As<Boolean>() == true) {
		DbgLog->set_level(spdlog::level::debug);
		DbgLog->flush_on(spdlog::level::debug);
	} else {
		DbgLog->set_level(spdlog::level::off);
	}
}
JsValue
Configure::GetDebugLogging(const Napi::CallbackInfo &info)
{
	bool enabled = false;
	if (DbgLog->level() == spdlog::level::debug) {
		enabled = true;
	}
	return Boolean::New(info.Env(),
											enabled);
}

}
