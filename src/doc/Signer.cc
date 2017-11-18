#include "Signer.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"

using namespace Napi;
using namespace PoDoFo;
namespace NoPoDoFo {
FunctionReference Signer::constructor;

Signer::Signer(const Napi::CallbackInfo&info)
		: ObjectWrap(info) {
	AssertFunctionArgs(
			info, 2, {napi_valuetype::napi_object, napi_valuetype::napi_object});
	doc = Document::Unwrap(info[0].As<Object>());
	if (!doc->LoadedForIncrementalUpdates()) {
		throw Napi::Error::New(info.Env(),
		                       "Please reload Document with forUpdates = true.");
	}
	field = SignatureField::Unwrap(info[1].As<Object>());

	if (info.Length() == 3 && info[2].IsString()) {
		device =
				new PdfOutputDevice(info[2].As<String>().Utf8Value().c_str(), true);
		signer = new PdfSignOutputDevice(device);
		buffer = nullptr;
	} else {
		buffer = new PdfRefCountedBuffer();
		device = new PdfOutputDevice(buffer);
		signer = new PdfSignOutputDevice(device);
	}
}

Signer::~Signer() {
	HandleScope scope(Env());
	doc = nullptr;
	signer = nullptr;
	buffer = nullptr;
	field = nullptr;
	device = nullptr;
}

void
Signer::Initialize(Napi::Env&env, Napi::Object&target) {
	HandleScope scope(env);
	Function ctor = DefineClass(
			env,
			"Signer",
			{InstanceMethod("sign", &Signer::Sign),
			 InstanceMethod("signWithCertificateAndKey", &Signer::PoDoFoSign)});
	constructor = Persistent(ctor);
	constructor.SuppressDestruct();
	target.Set("Signer", ctor);
}

class SignAsync: public AsyncWorker {
public:
	SignAsync(Function&cb, Signer *self, Buffer<char>&data)
			: AsyncWorker(cb), self(self), input(data) {}

private:
	Signer *self;
	bool outBuffer = false;
	Buffer<char> input;

	// AsyncWorker interface
protected:
	void Execute() override {
		try {
			self->signer->SetSignatureSize(input.Length());
			PdfData signature(input.Data());
			self->field->GetField()->SetSignature(
					*self->signer->GetSignatureBeacon());
			self->doc->GetDocument()->WriteUpdate(self->signer, true);

			if (!self->signer->HasSignaturePosition())
				PODOFO_RAISE_ERROR_INFO(
						ePdfError_SignatureError,
						"Cannot find signature position in the document data");

			self->signer->AdjustByteRange();
			self->signer->Seek(0);
			self->signer->SetSignature(input.Data());
			self->signer->Flush();
			if (self->buffer)
				outBuffer = true;
		} catch (PdfError&err) {
			SetError(ErrorHandler::WriteMsg(err));
		}
	}

	void OnOK() override {
		HandleScope scope(Env());
		auto value = Buffer<char>::Copy(
				Env(), self->buffer->GetBuffer(), self->buffer->GetSize());
		Callback().Call({Env().Null(), value});
	}
};

Napi::Value
Signer::PoDoFoSign(const CallbackInfo&info) {
	if (info.Length() < 2 || !info[0].IsFunction() || !info[1].IsBuffer()) {
		throw Error::New(info.Env(),
		                 "SetSignature requires a single argument of type buffer");
	}
	auto sig = info[1].As<Buffer<char>>();
	auto cb = info[0].As<Function>();
	auto worker = new SignAsync(cb, this, sig);
	worker->Queue();
	return info.Env().Undefined();
}

Napi::Value
Signer::Sign(const CallbackInfo&info) {
	try {

		string sigStr;
		pdf_long sigStrLength = 0;
		Buffer<char> sigBuffer;
		if (info[0].IsString()) {
			sigStr = info[0].As<String>().Utf8Value();
			sigStrLength = sigStr.size();
		} else if (info[0].IsBuffer()) {
			sigBuffer = info[0].As<Buffer<char>>();
		}
		PdfSignatureField *pSignField = field->GetField();
		PdfPage *page = doc->GetDocument()->GetPage(0);
		PdfRect rect(0.0, 0.0, 0.0, 0.0);
		PdfAnnotation *pAnnot = page->CreateAnnotation(ePdfAnnotation_Widget, rect);

		pSignField = new PdfSignatureField(
				pAnnot, doc->GetDocument()->GetAcroForm(), doc->GetDocument());
		pSignField->SetFieldName("test");
		pSignField->SetSignatureReason(
				PdfString(reinterpret_cast<const pdf_utf8 *>("reason")));


		auto document = doc->GetDocument();
		PdfRefCountedBuffer r;
		PdfOutputDevice outputDevice("/tmp/test.pdf", true /*&r*/);
		PdfSignOutputDevice signer(&outputDevice);
		if (sigStrLength == 0) {
			signer.SetSignatureSize(sigBuffer.Length());
		} else {
			signer.SetSignatureSize(static_cast<size_t>(sigStrLength));
		}

		pSignField->SetSignatureDate(PdfDate());
		pSignField->SetSignature(*signer.GetSignatureBeacon());
		document->WriteUpdate(&signer, true);

		if (!signer.HasSignaturePosition())
			throw Error::New(info.Env(),
			                 "Cannot find signature position in the document data");

		signer.AdjustByteRange();
		signer.Seek(0);

		PdfData *signature;
		if (sigStrLength == 0) {
			signature = new PdfData(sigBuffer.Data(), sigBuffer.Length());
		} else {
			signature =
					new PdfData(sigStr.c_str(), static_cast<size_t>(sigStrLength));
		}
		signer.SetSignature(*signature);

		signer.Flush();
		return info.Env().Undefined();
	} catch (PdfError&err) {
		ErrorHandler(err, info);
	} catch (Error&err) {
		ErrorHandler(err, info);
	}
}
}
