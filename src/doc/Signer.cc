/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
 * Authors: Cory Mickelson, et al.
 *
 * NoPoDoFo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NoPoDoFo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Signer.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Names.h"
#include "Document.h"
#include "SignatureField.h"
#include <map>
#include <sstream>
#include <vector>

using namespace Napi;
using namespace PoDoFo;

using std::string;
using std::stringstream;
using tl::nullopt;

#if defined(_WIN64)
#define fseeko _fseeki64
#define ftello _ftelli64
#else
#define fseeko fseek
#define ftello ftell
#endif

namespace NoPoDoFo {

FunctionReference Signer::Constructor; // NOLINT

/**
 * todo: StreamDocument support
 * @note JS new Signer(doc: Document, output?: string)
 * @param info
 */
Signer::Signer(const Napi::CallbackInfo &info)
	: ObjectWrap(info), Doc(Document::Unwrap(info[0].As<Object>())->GetDocument())
{
	DbgLog = spdlog::get("DbgLog");
	if (info.Length() < 1) {
		Error::New(info.Env(),
							 "Document required to construct Signer")
			.ThrowAsJavaScriptException();
		return;
	}
	if (!info[0].IsObject()) {
		TypeError::New(info.Env(),
									 "Document required to construct Signer")
			.ThrowAsJavaScriptException();
		return;
	}
	if (info.Length() >= 2 && info[1].IsString()) {
		Output = info[1].As<String>().Utf8Value();
	}
}

Signer::~Signer()
{
	DbgLog->debug("Signer Cleanup");
	HandleScope
	scope(Env());
	if (Cert != nullptr) {
		X509_free(Cert);
	}
	if (Pkey) {
		EVP_PKEY_free(Pkey);
	}
}

void
Signer::Initialize(Napi::Env &env,
									 Napi::Object &target)
{
	HandleScope scope(env);
	Function ctor = DefineClass(
		env,
		"Signer",
		{
			InstanceAccessor("signatureField", &Signer::GetField, &Signer::SetField),
			InstanceMethod("write", &Signer::SignWorker),
			InstanceMethod("loadCertificateAndKey", &Signer::LoadCertificateAndKey),
			InstanceMethod("getSigningContent", &Signer::GetSigningContent)
		});
	Constructor = Persistent(ctor);
	Constructor.SuppressDestruct();
	target.Set("Signer",
						 ctor);
}

void
Signer::SetField(const CallbackInfo &info,
								 const Napi::Value &value)
{
	Field = SignatureField::Unwrap(value.As<Object>())->GetField();
	try {
		Field->EnsureSignatureObject();
	} catch (PdfError &err) {
		ErrorHandler(err,
								 info);
	}
}

Value
Signer::GetField(const CallbackInfo &info)
{
	return SignatureField::Constructor.New(
		{External<PdfSignatureField>::New(info.Env(),
																			Field.get())});
}

class SignAsync: public AsyncWorker
{
public:
	SignAsync(Function &cb,
						Signer &self,
						pdf_int32 minSigSize)
		: AsyncWorker(cb), Self(self), MinSigSize(minSigSize)
	{}

private:
	Signer &Self;
	PdfRefCountedBuffer Buffer;
	pdf_int32 MinSigSize;

	// AsyncWorker interface
protected:
	void
	Execute() override
	{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		ERR_load_crypto_strings();
		ERR_load_PEM_strings();
		ERR_load_ASN1_strings();
		ERR_load_EVP_strings();
#else
		OPENSSL_init_ssl(0,
										 nullptr);
		OPENSSL_init();
#endif
		try {
			PdfObject *acroform = Self.Doc.GetAcroForm(false)->GetObject();
			size_t sigBuffer = 65535, sigBufferLen;
			int rc;
			char *sigData;
			char *outBuffer;
			long outBufferLen;
			BIO *memory;
			BIO *out;
			PKCS7 *p7;

			// Set SigFlags in AcroForm as Signed in AppendOnly mode (3)

			if (acroform->GetDictionary().HasKey(PdfName(Name::SIG_FLAGS))) {
				acroform->GetDictionary().RemoveKey(PdfName(Name::SIG_FLAGS));
			}
			pdf_int64 signedAppendModeFlag = 3;
			acroform->GetDictionary().AddKey(PdfName(Name::SIG_FLAGS),
																			 PdfObject(signedAppendModeFlag));

			// Create an output device for the signed document
			PdfOutputDevice outputDevice =
				Self.Output.empty() ?
				PdfOutputDevice(&Buffer)
														:
				PdfOutputDevice(Self.Output.c_str(),
												true);
			PdfSignOutputDevice signer(&outputDevice);

			// minimally ensure signature field name property is filled, defaults to
			// "NoPoDoFo.SignatureField"
			if (Self.Field->GetFieldName().GetStringUtf8().empty()) {
				Self.Field->SetFieldName("NoPoDoFo.SignatureField");
			}

			if (!Self.Field->GetSignatureObject()->GetDictionary().HasKey(Name::M)) {
				PdfDate now;
				PdfString str;
				now.ToString(str);
				Self.DbgLog
						->debug("Signer::SignAsync::Execute SignatureField Date Null, setting to now: {}", str.GetStringUtf8());
				Self.Field->SetSignatureDate(now);
			}

			// Set output device to write signature to designated area.
			signer.SetSignatureSize(static_cast<size_t>(MinSigSize));

			Self.Field->SetSignature(*signer.GetSignatureBeacon());
			Self.Doc.WriteUpdate(&signer,
													 true);

			if (!signer.HasSignaturePosition()) {
				SetError("Cannot find signature position in the document data");
				return;
			}

			signer.AdjustByteRange();
			signer.Seek(0);

			// Create signature
			while (static_cast<void>(sigData = reinterpret_cast<char *>(
				podofo_malloc(sizeof(char) * sigBuffer))),
				!sigData) {
				sigBuffer = sigBuffer / 2;
				if (!sigBuffer) {
					break;
				}
			}
			if (!sigData) {
				SetError("PdfError: Out of Memory.");
				return;
			}

			memory = BIO_new(BIO_s_mem());
			if (!memory) {
				podofo_free(sigData);
				SetError("Failed to create input BIO");
				return;
			}
			p7 = PKCS7_sign(
				Self.Cert,
				Self.Pkey,
				nullptr,
				memory,
				PKCS7_DETACHED | PKCS7_BINARY);
			if (!p7) {
				BIO_free(memory);
				podofo_free(sigData);
				SetError("PKCS7 Sign failed");
				return;
			}
			if (Self.DbgLog->level() == spdlog::level::debug) {
				stringstream dbgMsg;
				while (sigBufferLen = signer.ReadForSignature(sigData,
																											sigBuffer),
					sigBufferLen > 0) {
					dbgMsg << sigData;
					rc = BIO_write(memory,
												 sigData,
												 static_cast<int>(sigBufferLen));
					if (static_cast<unsigned int>(rc) != sigBufferLen) {
						PKCS7_free(p7);
						BIO_free(memory);
						podofo_free(sigData);
						SetError("BIO write failed");
						return;
					}
				}
				Self.DbgLog->debug("Signing contents from signing worker = {}", dbgMsg.str());
			} else {

				while (sigBufferLen = signer.ReadForSignature(sigData,
																											sigBuffer),
					sigBufferLen > 0) {
					rc = BIO_write(memory,
												 sigData,
												 static_cast<int>(sigBufferLen));
					if (static_cast<unsigned int>(rc) != sigBufferLen) {
						PKCS7_free(p7);
						BIO_free(memory);
						podofo_free(sigData);
						SetError("BIO write failed");
						return;
					}
				}

			}
			podofo_free(sigData);
			if (PKCS7_final(p7,
											memory,
											PKCS7_DETACHED | PKCS7_BINARY) <= 0) {
				PKCS7_free(p7);
				BIO_free(memory);
				SetError("pkcs7 final failed");
				return;
			}

			out = BIO_new(BIO_s_mem());
			if (!out) {
				PKCS7_free(p7);
				BIO_free(memory);
				SetError("Failed to create output bio");
				return;
			}
			i2d_PKCS7_bio(memory,
										p7);
			outBufferLen = BIO_get_mem_data(memory,
																			&outBuffer);
			if (outBufferLen > 0 && outBuffer) {
				if (static_cast<size_t>(outBufferLen) > signer.GetSignatureSize()) {
					PKCS7_free(p7);
					BIO_free(memory);
					BIO_free(out);
					SetError("Signature value out of prescribed range");
					return;
				}
				PdfData signature(outBuffer,
													static_cast<size_t>(outBufferLen));
				signer.SetSignature(signature);
				signer.Flush();
				return;
			} else {
				SetError("Invalid Signature was generated");
				return;
			}
		} catch (PdfError &err) {
			SetError(ErrorHandler::WriteMsg(err));
		} catch (Error &err) {
			SetError(ErrorHandler::WriteMsg(err));
		}
	}
	void
	OnOK() override
	{
		HandleScope
		scope(Env());
		if (!Self.Output.empty()) {
			if (FILE *file = fopen(Self.Output.c_str(),
														 "rb")) {
				fclose(file);
				Callback().Call({Env().Undefined()});
			} else {
				stringstream msg;
				msg << "Failed to write to: " << Self.Output << endl;
				Callback().Call({String::New(Env(),
																		 msg.str())});
			}
		} else {
			Callback().Call(
				{Env().Undefined(),
				 Napi::Buffer<char>::Copy(Env(),
																	Buffer.GetBuffer(),
																	Buffer.GetSize())});
		}
	}
};

/**
 * @note JS sign(signatureSize: int, cb: Function)
 * @param info
 * @return
 */
Value
Signer::SignWorker(const CallbackInfo &info)
{
	pdf_int32 minSigSize = info[0].As<Number>();
	Function cb = info[1].As<Function>();
	auto *worker = new SignAsync(cb,
															 *this,
															 minSigSize);
	worker->Queue();
	return info.Env().Undefined();
}

/**
 * @brief The LoadCertificateAndKeyWorker class
 * Load the private key and certificate and set as properties of the Signer
 * object as EVP_PKEY and X509 respectively.
 */
class LoadCertificateAndKeyWorker: public AsyncWorker
{
public:
	Signer &Self;
	string CertFile{};
	string KeyFile{};
	string Pwd{};
	char *CertBuffer = nullptr;
	char *KeyBuffer = nullptr;
	size_t CertLength = 0;
	size_t KeyLength = 0;
	pdf_int32 MinSigSize = 0;

	LoadCertificateAndKeyWorker(Function &cb,
															Signer &signer,
															string cert,
															string key,
															string pwd)
		: AsyncWorker(cb),
			Self(signer),
			CertFile(std::move(cert)),
			KeyFile(std::move(key)),
			Pwd(std::move(pwd))
	{}
	LoadCertificateAndKeyWorker(Function &cb,
															Signer &signer,
															char *cert,
															char *key,
															size_t certLength,
															size_t keyLength,
															string pwd)
		: AsyncWorker(cb, "load-certificate-and-key"),
			Self(signer),
			CertBuffer(cert),
			KeyBuffer(key),
			CertLength(certLength),
			KeyLength(keyLength),
			Pwd(std::move(pwd))
	{}


protected:
	pdf_int32
	minimumSignerSize(FILE *file)
	{
		pdf_int32 size = 0;
		if (fseeko(file,
							 0,
							 SEEK_END) != -1) {
			size += ftello(file);
		} else {
			size += 3072;
		}
		return size;
	}

	void
	Execute() override
	{
		FILE *file = nullptr;
		BIO *buffer = nullptr;

		// Load Certificate
		if (CertLength > 0) {
			buffer = BIO_new_mem_buf(CertBuffer, CertLength);
			Self.Cert = PEM_read_bio_X509(buffer, nullptr, nullptr, nullptr);
			MinSigSize += CertLength;
		} else {
			if (!(file = fopen(CertFile.c_str(),
												 "rb"))) {
				SetError("Failed to open Certificate file");
				return;
			}
			Self.Cert = PEM_read_X509(file,
																nullptr,
																nullptr,
																nullptr);
			MinSigSize += minimumSignerSize(file);
		}

		if (!Self.Cert) {
			SetError("Failed to decode Certificate file");
		}
		if (file != nullptr) {
			fclose(file);
		} else {
			BIO_free(buffer);
		}

		// Load Private key
		auto pkeyCallback = [](char *buf,
													 int bufSize,
													 int PODOFO_UNUSED_PARAM(rwflag),
													 void *userData)
		{
			const char *password = static_cast<char *>(userData);
			if (!password) {
				return 0;
			}
			auto res = static_cast<int>(strlen(password));
			if (res > bufSize) {
				res = bufSize;
			}
			memcpy(buf,
						 password,
						 static_cast<size_t>(res));
			return res;
		};
		if (KeyLength > 0) {
			buffer = BIO_new(BIO_s_mem());
			int l = BIO_write(buffer, KeyBuffer, KeyLength);
			buffer = BIO_new_mem_buf(KeyBuffer, KeyLength);
			Self.Pkey = PEM_read_bio_PrivateKey(buffer,
																					nullptr,
																					pkeyCallback,
																					static_cast<void *>(&Pwd));
			MinSigSize += KeyLength;
		} else {
			if (!(file = fopen(KeyFile.c_str(),
												 "rb"))) {
				SetError("Failed to open Private key file");
				X509_free(Self.Cert);
				return;
			}
			Self.Pkey =
				PEM_read_PrivateKey(file,
														nullptr,
														pkeyCallback,
														static_cast<void *>(&Pwd));
			MinSigSize += minimumSignerSize(file);
		}

		if (!Self.Pkey) {
			SetError("Failed to decode Private key file");
			X509_free(Self.Cert);
			return;
		}
		if (file != nullptr) {
			fclose(file);
		} else {
			BIO_free(buffer);
		}
	}
	void
	OnOK() override
	{
		HandleScope
		scope(Env());
		Callback().Call({Env().Undefined(),
										 Number::New(Env(),
																 MinSigSize)});
	}
};

Value
Signer::LoadCertificateAndKey(const CallbackInfo &info)
{
	string certFile, keyFile, pwd;
	Napi::Buffer<char> certBuffer, keyBuffer;
	auto useBuffers = false;

	Function cb;

	vector<int> optsIndices =
		AssertCallbackInfo(info,
											 {{0, {option(napi_string), option(napi_object)}},
												{1, {option(napi_string), option(napi_object)}},
												{2, {option(napi_string), option(napi_function)}},
												{3, {nullopt,             option(napi_function)}}});

	if (optsIndices[0] == 0) {
		certFile = info[0].As<String>().Utf8Value();
	} else if (info[0].IsBuffer()) {
		certBuffer = info[0].As<Buffer<char>>();
	} else {
		TypeError::New(info.Env(), "A string(filepath) or buffer is required").ThrowAsJavaScriptException();
		return {};
	}
	if (optsIndices[1] == 0) {
		keyFile = info[1].As<String>().Utf8Value();
	} else if (info[1].IsBuffer()) {
		keyBuffer = info[1].As<Buffer<char>>();
		useBuffers = true;
	} else {
		TypeError::New(info.Env(), "A string(filepath) or buffer is required").ThrowAsJavaScriptException();
		return {};
	}
	pwd = optsIndices[2] == 0 ? info[2].As<String>().Utf8Value() : std::string();
	cb = optsIndices[3] == 0 ? info[2].As<Function>() : info[3].As<Function>();

	AsyncWorker *worker;
	if (!certFile.empty() && !keyFile.empty()) {
		worker = new LoadCertificateAndKeyWorker(cb,
																						 *this,
																						 certFile,
																						 keyFile,
																						 pwd);
	} else if (useBuffers) {
		worker = new LoadCertificateAndKeyWorker(cb,
																						 *this,
																						 certBuffer.Data(),
																						 keyBuffer.Data(),
																						 certBuffer.Length(),
																						 keyBuffer.Length(),
																						 pwd);
	} else {
		Error::New(info.Env(),
							 "Parameters do not match function signature. Please view signer.loadCertAndKey docs").ThrowAsJavaScriptException();
		return {};
	}

	worker->Queue();
	return info.Env().Undefined();
}

class SigningContentWorker: public AsyncWorker
{
public:
	SigningContentWorker(Function cb,
											 Signer &self)
		: AsyncWorker(cb,
									"signing-content-async"), Self(self)
	{}
protected:
	void
	Execute() override
	{
		size_t sigBuffer = 65535 * 2, sigBufferLen;
		int rc;
		char *sigData;
		char *outBuffer;
		long outBufferLen;
		PdfRefCountedBuffer buffer;
		PdfOutputDevice device(&buffer);
		PdfSignOutputDevice signer(&device);
		signer.SetSignatureSize(4096);
		Self.Field->SetSignature(*signer.GetSignatureBeacon());
		Self.Doc.WriteUpdate(&signer);
		signer.AdjustByteRange();
		signer.Seek(0);
		Self.Doc.WriteUpdate(&signer,
												 true);

		if (!signer.HasSignaturePosition()) {
			SetError("Cannot find signature position in the document data");
			return;
		}

		signer.AdjustByteRange();
		signer.Seek(0);

		while (static_cast<void>(sigData = reinterpret_cast<char *>(
			podofo_malloc(sizeof(char) * sigBuffer))),
			!sigData) {
			sigBuffer = sigBuffer / 2;
			if (!sigBuffer) {
				break;
			}
		}
		if (!sigData) {
			SetError("PdfError: Out of Memory.");
			return;
		}

		while (sigBufferLen = signer.ReadForSignature(sigData,
																									sigBuffer),
			sigBufferLen > 0) {
			Output << sigData;
		}
	}
	void
	OnOK() override
	{
		HandleScope scope(Env());
		Self.DbgLog->debug("Signer::GetSigningContent = {}", Output.str());
		Callback().Call({Env().Undefined(),
										 String::New(Env(), Output.str())});
	}
private:
	Signer &Self;
	stringstream Output;
};
void
Signer::GetSigningContent(const Napi::CallbackInfo &info)
{
	AssertCallbackInfo(info,
										 {
											 {0, {option(napi_function)}}
										 });
	auto cb = info[0].As<Function>();
	auto worker = new SigningContentWorker(cb,
																				 *this);
	worker->Queue();
}

}
