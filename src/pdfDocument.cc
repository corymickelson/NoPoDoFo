//
// Created by red on 9/6/17.
//

#include "pdfDocument.h"

/*napi_ref PdfMemDocument::constructor;

PdfMemDocument::PdfMemDocument( const string &file )
		: original( file ), env_( nullptr ), wrapper_( nullptr ) { }

PdfMemDocument::~PdfMemDocument( ) { napi_delete_reference( env_, wrapper_ ); }

void PdfMemDocument::Destructor( napi_env env, void *obj, void *finalize ) {
	reinterpret_cast<PdfMemDocument *>(obj)->~PdfMemDocument( );
}

void PdfMemDocument::Init( napi_env env, napi_value exports ) {
	napi_status status;
	napi_property_descriptor properties[] = {

	};
	napi_value cons;
	if((status = napi_define_class(env, "PdfMemDocument", New, nullptr,0,properties, &cons)) != napi_ok) {}
	if((status = napi_create_reference(env, cons, 1, &constructor)) != napi_ok) {}
	if((status = napi_set_named_property(env, exports, "PdfMemDocument", cons)) != napi_ok) {}
}

napi_value PdfMemDocument::New( napi_env env, napi_callback_info info) {
	napi_status status;
	bool isConstructor;
	if((status = napi_is_construct_call(env, info, &isConstructor)) != napi_ok) {}
	// invoke as constructor
	if(isConstructor) {
		size_t argc = 1;
		napi_value args[argc];
		napi_value js;

		if((status = napi_get_cb_info(env, info, &argc, args, &js, nullptr)) != napi_ok) {}
		char* originValue = nullptr;
		napi_valuetype vType;
		size_t originValueResultOut;
		if((status = napi_typeof(env,args[0],&vType)) != napi_ok) {}
		if(vType == napi_undefined) {}
		if((status = napi_get_value_string_utf8( env, args[0], originValue, static_cast<size_t>(-1), &originValueResultOut)))
	}
}*/

pdfDocument::pdfDocument( const CallbackInfo &callbackInfo ) : ObjectWrap( callbackInfo ) { }

void pdfDocument::Initialize( Env &env, Object &target ) {
	Function ctor = DefineClass(env, "pdfDocument", {
			InstanceAccessor("page", &GetPage, &SetPage),
			InstanceMethod("load", &Load)
	}, nullptr);
}
