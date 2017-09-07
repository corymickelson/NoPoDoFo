//
// Created by red on 9/6/17.
//

#ifndef NPDF_PDFMEMDOCUMENT_H
#define NPDF_PDFMEMDOCUMENT_H

#include "library.hpp"


class pdfDocument: public Napi::ObjectWrap<pdfDocument> {
public:
	explicit pdfDocument( const CallbackInfo &callbackInfo ); //constructor
	static void Initialize(Env& env, Object& target) {
		Function constructor = DefineClass(env, "pdfDocument", {
				InstanceAccessor("page", &GetPage, &SetPage),
				InstanceMethod("load", &Load)
		}, nullptr );
		target.Set("pdfDocument", constructor);
	}
	Value Load(const CallbackInfo&);
	Value GetPageCount(const CallbackInfo&);
	Value GetPage(const CallbackInfo&);
	void Write(const CallbackInfo&);
	void SetPage(const CallbackInfo&, const Value&);
};

/*class PdfMemDocument {
public:
	void Init(napi_env env, napi_value exports);
	static void Destructor( napi_env env, void *obj, void *finalize );

private:
	explicit PdfMemDocument(const string &file);
	~PdfMemDocument( );

	static napi_value New(napi_env, napi_callback_info);
	static napi_value Load(napi_env, napi_callback_info);
	static napi_value GetPageCount(napi_env, napi_callback_info);
	static napi_value GetPage(napi_env, napi_callback_info);
	static napi_value Write(napi_env, napi_callback_info);

	static napi_ref constructor;
	string original;
	napi_env env_;
	napi_ref wrapper_;
};*/


#endif //NPDF_PDFMEMDOCUMENT_H
