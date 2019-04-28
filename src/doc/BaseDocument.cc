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

#include "BaseDocument.h"
#include "../Defines.h"
#include "../ErrorHandler.h"
#include "../ValidateArguments.h"
#include "../base/Names.h"
#include "../base/Obj.h"
#include "../base/Ref.h"
#include "../base/XObject.h"
#include "../doc/Rect.h"
#include "Document.h"
#include "Encrypt.h"
#include "FileSpec.h"
#include "Font.h"
#include "Form.h"
#include "Outline.h"
#include "Page.h"
#include <spdlog/spdlog.h>

using namespace Napi;
using namespace PoDoFo;

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using tl::nullopt;

namespace NoPoDoFo {

/**
 * @note JS Derived class instantiate new BaseDocument<T>(string: filePath,
 * opts: {version, writeMode, encrypt})
 *
 * @brief BaseDocument::BaseDocument
 * @param info
 * @param inMem
 */
BaseDocument::BaseDocument(const Napi::CallbackInfo &info, const bool inMem)
{
	DbgLog = spdlog::get("DbgLog");
	if (inMem) {
		Base = new PdfMemDocument();
		DbgLog->debug("New PdfMemDocument");
	} else {
		auto version = ePdfVersion_1_7;
		auto writeMode = ePdfWriteMode_Default;
		PdfEncrypt *encrypt = nullptr;

		if (info.Length() >= 2 && info[1].IsObject()) {
			const auto nObj = info[1].As<Object>();
			if (nObj.Has("version")) {
				version = static_cast<EPdfVersion>(
					nObj.Get("version").As<Number>().Uint32Value());
			}
			if (nObj.Has("writeMode")) {
				writeMode = static_cast<EPdfWriteMode>(
					nObj.Get("writeMode").As<Number>().Uint32Value());
			}
			if (nObj.Has("encrypt")) {
				const auto nEncObj = Encrypt::Unwrap(nObj.Get("encrypt").As<Object>());
				encrypt = PdfEncrypt::CreatePdfEncrypt(*nEncObj->Self);
			}
		}
		if (info.Length() > 0 && info[0].IsString()) {
			Output = info[0].As<String>().Utf8Value();
			Base =
				new PdfStreamedDocument(Output.c_str(), version, encrypt, writeMode);
			std::stringstream dbgMsg;
			dbgMsg << "New PdfStreamedDocument to " << Output.c_str() << endl;
			DbgLog->debug(dbgMsg.str());
		} else {
			StreamDocRefCountedBuffer = new PdfRefCountedBuffer(2048);
			StreamDocOutputDevice = new PdfOutputDevice(StreamDocRefCountedBuffer);
			Base = new PdfStreamedDocument(
				StreamDocOutputDevice, version, encrypt, writeMode);
			DbgLog->debug("New PdfStreamedDocument to Buffer");
		}
	}
}

BaseDocument::~BaseDocument()
{
	DbgLog->debug("BaseDocument Cleanup");
	for (auto c : Copies) {
		delete c;
	}
	delete Base;
	delete StreamDocOutputDevice;
	delete StreamDocRefCountedBuffer;
}

JsValue
BaseDocument::GetPageCount(const CallbackInfo &info)
{
	const auto pages = Base->GetPageCount();
	return Number::New(info.Env(), pages);
}
JsValue
BaseDocument::GetPage(const CallbackInfo &info)
{
	const int n = info[0].As<Number>();
	if (n < 0 || n > Base->GetPageCount()) {
		RangeError::New(info.Env(), "Page index out of range")
			.ThrowAsJavaScriptException();
		return {};
	}
	return Page::constructor.New(
		{External<PdfPage>::New(info.Env(), Base->GetPage(n))});
}

void
BaseDocument::SetHideMenubar(const Napi::CallbackInfo &)
{
	Base->SetHideMenubar();
}

JsValue
BaseDocument::GetPageMode(const CallbackInfo &info)
{
	return Number::New(info.Env(), static_cast<int>(Base->GetPageMode()));
}

void
BaseDocument::SetPageMode(const CallbackInfo &, const JsValue &value)
{
	int flag = value.As<Number>();
	const auto mode = static_cast<EPdfPageMode>(flag);
	Base->SetPageMode(mode);
}

void
BaseDocument::SetPageLayout(const CallbackInfo &, const JsValue &value)
{
	int flag = value.As<Number>();
	const auto mode = static_cast<EPdfPageLayout>(flag);
	Base->SetPageLayout(mode);
}

void
BaseDocument::SetUseFullScreen(const CallbackInfo &)
{
	Base->SetUseFullScreen();
}

void
BaseDocument::SetHideToolbar(const CallbackInfo &)
{
	Base->SetHideMenubar();
}

void
BaseDocument::SetHideWindowUI(const CallbackInfo &)
{
	Base->SetHideWindowUI();
}

void
BaseDocument::SetFitWindow(const CallbackInfo &)
{
	Base->SetFitWindow();
}

void
BaseDocument::SetCenterWindow(const CallbackInfo &)
{
	Base->SetCenterWindow();
}

void
BaseDocument::SetDisplayDocTitle(const CallbackInfo &)
{
	Base->SetDisplayDocTitle();
}

void
BaseDocument::SetPrintingScale(const CallbackInfo &, const JsValue &value)
{
	PdfName scale(value.As<String>().Utf8Value());
	Base->SetPrintScaling(scale);
}

void
BaseDocument::SetLanguage(const CallbackInfo &, const JsValue &value)
{
	Base->SetLanguage(value.As<String>().Utf8Value());
}

void
BaseDocument::AttachFile(const CallbackInfo &info)
{
	auto value = info[0].As<String>().Utf8Value();
	if (!FileAccess(value)) {
		Error::New(info.Env(), "File: " + value + " not found")
			.ThrowAsJavaScriptException();
		return;
	}
	auto embed = true;
	if (!Output.empty() || (StreamDocOutputDevice && StreamDocRefCountedBuffer)) {
		embed = false;
	}
	const PdfFileSpec attachment(value.c_str(), embed, Base);
	Base->AttachFile(attachment);
}

JsValue
BaseDocument::GetVersion(const CallbackInfo &info)
{
	const auto versionE = Base->GetPdfVersion();
	auto v = 0.0;
	switch (versionE) {
		case ePdfVersion_1_1:v = 1.1;
			break;
		case ePdfVersion_1_3:v = 1.3;
			break;
		case ePdfVersion_1_0:v = 1.0;
			break;
		case ePdfVersion_1_2:v = 1.2;
			break;
		case ePdfVersion_1_4:v = 1.4;
			break;
		case ePdfVersion_1_5:v = 1.5;
			break;
		case ePdfVersion_1_6:v = 1.6;
			break;
		case ePdfVersion_1_7:v = 1.7;
			break;
	}
	if (v == 0.0) {
		throw Error::New(info.Env(), "Failed to parse document. Pdf version unknown.");
	}
	return Number::New(info.Env(), v);
}

JsValue
BaseDocument::IsLinearized(const CallbackInfo &info)
{
	return Boolean::New(info.Env(), Base->IsLinearized());
}

JsValue
BaseDocument::GetWriteMode(const CallbackInfo &info)
{
	string writeMode;
	switch (Base->GetWriteMode()) {
		case ePdfWriteMode_Clean: {
			writeMode = "Clean";
			break;
		}
		case ePdfWriteMode_Compact: {
			writeMode = "Compact";
			break;
		}
	}
	return Napi::String::New(info.Env(), writeMode);
}

JsValue
BaseDocument::GetObjects(const CallbackInfo &info)
{
	try {
		auto js = Array::New(info.Env());
		uint32_t count = 0;
		for (auto item : *Base->GetObjects()) {
			if (item->IsReference()) {
				item = item->GetOwner()->GetObject(item->GetReference());
			}
			auto instance = External<PdfObject>::New(info.Env(), item);
			js[count] = Obj::Constructor.New({instance});
			++count;
		}
		return js;
	} catch (PdfError &err) {
		ErrorHandler(err, info);
	} catch (Error &err) {
		ErrorHandler(err, info);
	}
	return info.Env().Undefined();
}

JsValue
BaseDocument::GetObject(const CallbackInfo &info)
{
	PdfReference *ref = nullptr;
	if (info[0].IsArray()) {
		unsigned long g = info[0]
			.As<Array>()
			.Get(static_cast<uint32_t>(0))
			.As<Number>()
			.Uint32Value();
		const pdf_objnum o =
			info[0].As<Array>().Get(static_cast<uint32_t>(1)).As<Number>();
		const auto gen = static_cast<pdf_gennum>(g);
		PdfReference r(o, gen);
		ref = &r;
	} else if (info[0].IsObject() &&
		info[0].As<Object>().InstanceOf(Ref::Constructor.Value())) {
		ref = Ref::Unwrap(info[0].As<Object>())->Self;
	}
	if (!ref) {
		Error::New(info.Env(), "NoPoDoFo is unable to resolve null reference")
			.ThrowAsJavaScriptException();
		return {};
	}
	const auto target = Base->GetObjects()->GetObject(*ref);
	if (!target) {
		stringstream oss;
		oss << "NoPoDoFo is unable to resolve reference " << ref->ObjectNumber()
				<< " : " << ref->GenerationNumber();
		DbgLog->debug(oss.str());
		Error::New(info.Env(), oss.str()).ThrowAsJavaScriptException();
		return {};
	}
	return Obj::Constructor.New({External<PdfObject>::New(info.Env(), target)});
}

JsValue
BaseDocument::IsAllowed(const CallbackInfo &info)
{
	const auto allowed = info[0].As<String>().Utf8Value();
	bool is;
	if (allowed == "Print") {
		is = Base->IsPrintAllowed();
	} else if (allowed == "Edit") {
		is = Base->IsEditAllowed();
	} else if (allowed == "Copy") {
		is = Base->IsCopyAllowed();
	} else if (allowed == "EditNotes") {
		is = Base->IsEditNotesAllowed();
	} else if (allowed == "FillAndSign") {
		is = Base->IsFillAndSignAllowed();
	} else if (allowed == "Accessible") {
		is = Base->IsAccessibilityAllowed();
	} else if (allowed == "DocAssembly") {
		is = Base->IsDocAssemblyAllowed();
	} else if (allowed == "HighPrint") {
		is = Base->IsHighPrintAllowed();
	} else {
		throw Napi::Error::New(
			info.Env(), "Unknown argument. Please see definitions file for isAllowed args");
	}
	return Napi::Boolean::New(info.Env(), is);
}

JsValue
BaseDocument::CreateFont(const CallbackInfo &info)
{
	if (info.Length() < 1 || !info[0].IsObject()) {
		Error::New(info.Env(), "Invalid argument, expected an object")
			.ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}
	const auto font = CreateFontObject(info.Env(), info[0].As<Object>(), false);
	return Font::Constructor.New({External<PdfFont>::New(info.Env(), font)});
}
/**
 * @note Javascript args (doc:Document, pageN:number, atN:number)
 * @param info
 * @return
 */
JsValue
BaseDocument::InsertExistingPage(const CallbackInfo &info)
{
	auto memDoc = Document::Unwrap(info[0].As<Object>());
	const int memPageN = info[1].As<Number>();
	const int atN = info[2].As<Number>();
	if (atN < 0) {
		cout << "Appending page to the beginning of this document" << endl;
	}
	if (Base->GetPageCount() + 1 < atN) {
		RangeError::New(info.Env(), "at index out of range")
			.ThrowAsJavaScriptException();
		return {};
	}
	if (memPageN < 0 || memPageN > memDoc->Base->GetPageCount() - 1) {
		RangeError::New(info.Env(), "parameter document page range exception")
			.ThrowAsJavaScriptException();
		return {};
	}
	Base->InsertExistingPageAt(memDoc->GetDocument(), memPageN, atN);
	return Number::New(info.Env(), Base->GetPageCount());
}
JsValue
BaseDocument::GetInfo(const CallbackInfo &info)
{
	auto i = Base->GetInfo();
	auto infoObj = Object::New(info.Env());
	infoObj.Set("author", i->GetAuthor().GetStringUtf8());
	infoObj.Set("createdAt", i->GetCreationDate().GetTime());
	infoObj.Set("creator", i->GetCreator().GetStringUtf8());
	infoObj.Set("keywords", i->GetKeywords().GetStringUtf8());
	infoObj.Set("producer", i->GetProducer().GetStringUtf8());
	infoObj.Set("subject", i->GetSubject().GetStringUtf8());
	infoObj.Set("title", i->GetTitle().GetStringUtf8());
	return infoObj;
}
/**
 *
 * @param info
 * @return Obj - The outline object dictionary
 */
JsValue
BaseDocument::GetOutlines(const CallbackInfo &info)
{
	auto opts = AssertCallbackInfo(
		info, {{0, {nullopt, option(napi_boolean), option(napi_string)}}, {1, {nullopt, option(napi_string)}}});

	PdfOutlineItem *outlines = nullptr;
	// Create with default options
	if (opts[0] == 0) {
		outlines = Base->GetOutlines();
	} // Create with create option provided
	if (opts[0] == 1) {
		const bool create = info[0].As<Boolean>();
		outlines = Base->GetOutlines(create);
	}

	if (!outlines) {
		return info.Env().Null();
	}

	string root;
	if (opts[0] == 2) {
		root = info[0].As<String>().Utf8Value();
	}
	if (opts[1] == 1) {
		root = info[1].As<String>().Utf8Value();
	}
	if (!root.empty()) {
		dynamic_cast<PdfOutlines *>(outlines)->CreateRoot(PdfString(root));
	}

	return Outline::constructor.New(
		{External<PdfOutlineItem>::New(info.Env(), outlines), External<BaseDocument>::New(info.Env(), this)});
}
JsValue
BaseDocument::GetNamesTree(const CallbackInfo &info)
{
	auto names = Base->GetNamesTree(info[0].As<Boolean>());
	if (!names) {
		return info.Env().Null();
	}
	return Obj::Constructor.New(
		{External<PdfObject>::New(info.Env(), names->GetObject())});
}
JsValue
BaseDocument::CreatePage(const CallbackInfo &info)
{
	const auto r = Rect::Unwrap(info[0].As<Object>())->GetRect();
	const auto page = Base->CreatePage(r);
	return Page::constructor.New({External<PdfPage>::New(info.Env(), page)});
}
JsValue
BaseDocument::CreatePages(const Napi::CallbackInfo &info)
{
	const auto coll = info[0].As<Array>();
	vector<PdfRect> rects;
	for (uint32_t i = 0; i < coll.Length(); i++) {
		rects.emplace_back(
			Rect::Unwrap(coll.Get(static_cast<uint32_t>(i)).As<Object>())->GetRect());
	}
	Base->CreatePages(rects);
	return Number::New(info.Env(), Base->GetPageCount());
}
JsValue
BaseDocument::InsertPage(const Napi::CallbackInfo &info)
{
	const auto rect = Rect::Unwrap(info[0].As<Object>())->GetRect();
	const int index = info[1].As<Number>();
	Base->InsertPage(rect, index);
	return Page::constructor.New(
		{External<PdfPage>::New(info.Env(), Base->GetPage(index))});
}

void
BaseDocument::Append(const Napi::CallbackInfo &info)
{
	if (info.Length() == 1 && info[0].IsArray()) {
		const auto docs = info[0].As<Array>();
		for (unsigned int i = 0; i < docs.Length(); i++) {
			auto arg = docs.Get(i);
			if (arg.IsObject() &&
				arg.As<Object>().InstanceOf(Document::Constructor.Value())) {
				auto mergedDoc = Document::Unwrap(arg.As<Object>());
				Base->Append(mergedDoc->GetDocument());
			} else {
				TypeError::New(info.Env(), "Only Document's can be appended, StreamDocument not "
																	 "supported in append operation")
					.ThrowAsJavaScriptException();
				return;
			}
		}
	} else if (info.Length() == 1 && info[0].IsObject() &&
		info[0].As<Object>().InstanceOf(Document::Constructor.Value())) {
		auto mergedDoc = Document::Unwrap(info[0].As<Object>());
		Base->Append(mergedDoc->GetDocument());
	}
}

JsValue
BaseDocument::GetAttachment(const CallbackInfo &info)
{
	const string name = info[0].As<String>();
	if (Base->GetAttachment(name)) {
		return FileSpec::Constructor.New({External<PdfObject>::New(
			info.Env(), Base->GetAttachment(name)->GetObject())});
	}
	if (Base->GetNamesTree(false) == nullptr || Base->GetNamesTree(false)->GetObject() == nullptr
		|| !Base->GetNamesTree(false)->GetObject()->IsDictionary()
		|| !Base->GetNamesTree(false)->GetObject()->GetDictionary().HasKey(Name::EMBEDDED_FILES)) {
		DbgLog->debug("GetAttachment: PDF does not have any attachments\n");
		return info.Env().Null();
	}
	auto findAttachment = [&](PdfArray &arr) -> PdfObject *
	{
		for (auto &&ii : arr) {
			PdfObject *item = ii.IsReference() ?
												Base->GetObjects()->GetObject(ii.GetReference()) :
												&ii;
			PdfObject *fileSpec = nullptr;
			if (item->IsDictionary() && item->GetDictionary().HasKey(Name::TYPE) &&
				(item->GetDictionary().GetKey(Name::TYPE)->GetName() == Name::FILESPEC
					|| item->GetDictionary().GetKey(Name::TYPE)->GetName() == Name::F)) {
				fileSpec = item;

				if (fileSpec->GetDictionary().HasKey(Name::UF)) {
					PdfObject *uf = fileSpec->MustGetIndirectKey(Name::UF);
					DbgLog->debug("GetAttachment: PDF Attachment name:%s\n", uf->GetString().GetStringUtf8().c_str());
					if (uf->IsString() && uf->GetString().GetStringUtf8() == name) {
						fileSpec->GetDictionary().RemoveKey(PdfName(Name::TYPE));
						fileSpec->GetDictionary().AddKey(PdfName(Name::TYPE), PdfName(Name::FILESPEC));
						return fileSpec;
					}
				}
			}
		}
		return nullptr;
	};
	PdfObject *embeddedFiles =
		Base->GetNamesTree(false)->GetObject()->MustGetIndirectKey(
			Name::EMBEDDED_FILES);
	PdfObject *file;
	if (embeddedFiles->GetDictionary().HasKey(Name::NAMES) &&
		embeddedFiles->MustGetIndirectKey(Name::NAMES)->IsArray()) {
		PdfObject *names = nullptr;
		names = embeddedFiles->MustGetIndirectKey(Name::NAMES);
		if ((file = findAttachment(names->GetArray())) != nullptr) {
			return FileSpec::Constructor.New(
				{External<PdfObject>::New(info.Env(), file)});
		}
	}
	if (embeddedFiles->GetDictionary().HasKey(Name::KIDS)) {
		for (auto &i : embeddedFiles->MustGetIndirectKey(Name::KIDS)->GetArray()) {
			PdfObject *names = nullptr;
			PdfObject *kid = i.IsReference() ?
											 Base->GetObjects()->GetObject(i.GetReference()) :
											 &i;
			if (kid->GetDictionary().HasKey(Name::NAMES) ||
				!kid->MustGetIndirectKey(Name::NAMES)->IsArray()) {
				names = kid->MustGetIndirectKey(Name::NAMES);
			} else {
				break;
			}
			if ((file = findAttachment(names->GetArray())) != nullptr) {
				return FileSpec::Constructor.New(
					{External<PdfObject>::New(info.Env(), file)});
			}
		}
	}
	return info.Env().Null();
}
void
BaseDocument::AddNamedDestination(const Napi::CallbackInfo &info)
{
	auto page = Page::Unwrap(info[0].As<Object>())->page;
	const auto fit =
		static_cast<EPdfDestinationFit>(info[1].As<Number>().Int32Value());
	const auto name = info[2].As<String>().Utf8Value();
	const auto destination = new PdfDestination(&page, fit);
	Base->AddNamedDestination(*destination, name);
}

JsValue
BaseDocument::CreateXObject(const CallbackInfo &info)
{
	if (info.Length() != 1 || !info[0].IsObject() ||
		!info[0].As<Object>().InstanceOf(Rect::constructor.Value())) {
		TypeError::New(
			info.Env(), "CreateXObject requires an NoPoDoFo::Rect instance as the first argument")
			.ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}
	return XObject::Constructor.New(
		{info[0].As<Object>(), External<PdfDocument>::New(info.Env(), Base)});
}
JsValue
BaseDocument::GetForm(const CallbackInfo &info)
{
	const auto instance =
		Form::Constructor.New({External<BaseDocument>::New(info.Env(), this), Boolean::New(info.Env(), true)});
	return instance;
}
JsValue
BaseDocument::CreateFontSubset(const Napi::CallbackInfo &info)
{
	if (info.Length() < 1 || !info[0].IsObject()) {
		Error::New(info.Env(), "Invalid argument, expected an object")
			.ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}
	const auto font = CreateFontObject(info.Env(), info[0].As<Object>(), true);
	return Font::Constructor.New({External<PdfFont>::New(info.Env(), font)});
}
PdfFont *
BaseDocument::CreateFontObject(napi_env env, Napi::Object opts, bool subset)
{
	HandleScope scope(env);
	if (!opts.Has("fontName")) {
		TypeError::New(env, "Requires fontName").ThrowAsJavaScriptException();
	}
	const auto fontName = opts.Get("fontName").As<String>().Utf8Value();
	const auto bold = opts.Has("bold") ? opts.Get("bold").As<Boolean>() : false;
	const auto italic = opts.Has("italic") ? opts.Get("italic").As<Boolean>() : false;
	const auto embed = opts.Has("embed") ? opts.Get("embed").As<Boolean>() : false;
	const PdfEncoding *encoding = nullptr;
	const auto filename =
		opts.Has("fileName") ? opts.Get("fileName").As<String>().Utf8Value() : "";
#ifndef PODOFO_HAVE_FONTCONFIG
	std::cout << "This build does not include fontconfig. To load a font you "
							 "must provide the full path to the font file"
						<< std::endl;
	if (filename == "") {
		Error::New(env,
							 "This build requires any font creation to pass the full path to "
							 "the font file")
			.ThrowAsJavaScriptException();
	}
#endif
	const auto n = opts.Has("encoding") ? opts.Get("encoding").As<Number>() : 1;
	switch (n) {
		case 1:encoding = PdfEncodingFactory::GlobalWinAnsiEncodingInstance();
			break;
		case 2:encoding = PdfEncodingFactory::GlobalStandardEncodingInstance();
			break;
		case 3:encoding = PdfEncodingFactory::GlobalPdfDocEncodingInstance();
			break;
		case 4:encoding = PdfEncodingFactory::GlobalMacRomanEncodingInstance();
			break;
		case 5:encoding = PdfEncodingFactory::GlobalMacExpertEncodingInstance();
			break;
		case 6:encoding = PdfEncodingFactory::GlobalSymbolEncodingInstance();
			break;
		case 7:encoding = PdfEncodingFactory::GlobalZapfDingbatsEncodingInstance();
			break;
		case 8:encoding = PdfEncodingFactory::GlobalWin1250EncodingInstance();
			break;
		case 9:encoding = PdfEncodingFactory::GlobalIso88592EncodingInstance();
			break;
		default:encoding = PdfEncodingFactory::GlobalIdentityEncodingInstance();
	}
	try {
		PdfFont *font;
		if (subset) {
			font =
				Base->CreateFontSubset(fontName.c_str(), bold, italic, false, encoding, filename.empty() ? nullptr : filename
					.c_str());
		} else {
			font = Base->CreateFont(fontName.c_str()
															, bold
															, italic
															, false
															, encoding
															, PdfFontCache::eFontCreationFlags_AutoSelectBase14
															, embed
															, filename.empty() ? nullptr : filename.c_str());
		}
		return font;
	} catch (PdfError &err) {
		Error::New(env, err.what()).ThrowAsJavaScriptException();
		return {};
	}
}
}
