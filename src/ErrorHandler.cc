//
// Created by red on 9/10/17.
//

#include "ErrorHandler.h"

using namespace std;
using namespace Napi;
using namespace PoDoFo;

ErrorHandler::ErrorHandler() {}

ErrorHandler::ErrorHandler(PoDoFo::PdfError& err, const CallbackInfo& info)
{
  string msg = WriteMsg(err);
  stringstream eMsg;
  streambuf* out = std::cout.rdbuf(eMsg.rdbuf());
  eMsg << "PoDoFo error: " << msg << endl;
  err.PrintErrorMsg();
  throw Napi::Error::New(info.Env(), eMsg.str());
}

ErrorHandler::ErrorHandler(Error& err, const Napi::CallbackInfo& info)
{
  stringstream msg;
  msg << "JS error: " << err.Message() << endl;
  throw Napi::Error::New(info.Env(), msg.str());
}

string
ErrorHandler::WriteMsg(PoDoFo::PdfError& err)
{
  string msg = ParseMsgFromPdfError(err);
  return msg;
}

string
ErrorHandler::WriteMsg(Napi::Error &err)
{
  stringstream msg;
  msg << "JS error: " << err.Message() << endl;
  return msg.str();
}

string
ErrorHandler::ParseMsgFromPdfError(PoDoFo::PdfError& err)
{
  string text;
  PoDoFo::EPdfError e = static_cast<PoDoFo::EPdfError>(err.GetError());
  switch (e) {
    case PoDoFo::ePdfError_BrokenFile:
      text = "BrokenFile";
      break;
    case PoDoFo::ePdfError_ErrOk:
      text = "Ok";
      break;
    case PoDoFo::ePdfError_TestFailed:
      text = "TestFailed";
      break;
    case PoDoFo::ePdfError_InvalidHandle:
      text = "InvalidHandle";
      break;
    case PoDoFo::ePdfError_FileNotFound:
      text = "FileNotFound";
      break;
    case PoDoFo::ePdfError_InvalidDeviceOperation:
      text = "InvalidDeviceOperation";
      break;
    case PoDoFo::ePdfError_UnexpectedEOF:
      text = "UnexpectedEOF";
      break;
    case PoDoFo::ePdfError_OutOfMemory:
      text = "OutOfMemory";
      break;
    case PoDoFo::ePdfError_ValueOutOfRange:
      text = "ValueOutOfRange";
      break;
    case PoDoFo::ePdfError_InternalLogic:
      text = "InternalLogic";
      break;
    case PoDoFo::ePdfError_InvalidEnumValue:
      text = "InvalidEnumValue";
      break;
    case PoDoFo::ePdfError_PageNotFound:
      text = "PageNotFound";
      break;
    case PoDoFo::ePdfError_NoPdfFile:
      text = "NoPdfFile";
      break;
    case PoDoFo::ePdfError_NoXRef:
      text = "NoXRef";
      break;
    case PoDoFo::ePdfError_NoTrailer:
      text = "NoTrailer";
      break;
    case PoDoFo::ePdfError_NoNumber:
      text = "NoNumber";
      break;
    case PoDoFo::ePdfError_NoObject:
      text = "NoObject";
      break;
    case PoDoFo::ePdfError_NoEOFToken:
      text = "NoEOFToken";
      break;
    case PoDoFo::ePdfError_InvalidTrailerSize:
      text = "InvalidTrailerSize";
      break;
    case PoDoFo::ePdfError_InvalidLinearization:
      text = "InvalidLinearization";
      break;
    case PoDoFo::ePdfError_InvalidDataType:
      text = "InvalidDataType";
      break;
    case PoDoFo::ePdfError_InvalidXRef:
      text = "InvalidXRef";
      break;
    case PoDoFo::ePdfError_InvalidXRefStream:
      text = "InvalidXRefStream";
      break;
    case PoDoFo::ePdfError_InvalidXRefType:
      text = "InvalidXRefType";
      break;
    case PoDoFo::ePdfError_InvalidPredictor:
      text = "InvalidPredictor";
      break;
    case PoDoFo::ePdfError_InvalidStrokeStyle:
      text = "InvalidStrokeStyle";
      break;
    case PoDoFo::ePdfError_InvalidHexString:
      text = "InvalidHexString";
      break;
    case PoDoFo::ePdfError_InvalidStream:
      text = "InvalidStream";
      break;
    case PoDoFo::ePdfError_InvalidStreamLength:
      text = "InvalidStreamLength";
      break;
    case PoDoFo::ePdfError_InvalidKey:
      text = "InvalidKey";
      break;
    case PoDoFo::ePdfError_InvalidName:
      text = "InvalidName";
      break;
    case PoDoFo::ePdfError_InvalidEncryptionDict:
      text = "InvalidEncryptionDict";
      break;
    case PoDoFo::ePdfError_InvalidPassword:
      text = "InvalidPassword";
      break;
    case PoDoFo::ePdfError_InvalidFontFile:
      text = "InvalidFontFile";
      break;
    case PoDoFo::ePdfError_InvalidContentStream:
      text = "InvalidContentStream";
      break;
    case PoDoFo::ePdfError_UnsupportedFilter:
      text = "UnsupportedFilter";
      break;
    case PoDoFo::ePdfError_UnsupportedFontFormat:
      text = "UnsupportedFontFormat";
      break;
    case PoDoFo::ePdfError_ActionAlreadyPresent:
      text = "ActionAlreadyPresent";
      break;
    case PoDoFo::ePdfError_WrongDestinationType:
      text = "WrongDestinationType";
      break;
    case PoDoFo::ePdfError_MissingEndStream:
      text = "MissingEndStream";
      break;
    case PoDoFo::ePdfError_Date:
      text = "Date";
      break;
    case PoDoFo::ePdfError_Flate:
      text = "Flate";
      break;
    case PoDoFo::ePdfError_FreeType:
      text = "FreeType";
      break;
    case PoDoFo::ePdfError_SignatureError:
      text = "SignatureError";
      break;
    case PoDoFo::ePdfError_MutexError:
      text = "MutexError";
      break;
    case PoDoFo::ePdfError_UnsupportedImageFormat:
      text = "UnsupportedImageFormat";
      break;
    case PoDoFo::ePdfError_CannotConvertColor:
      text = "CannotConvertColor";
      break;
    case PoDoFo::ePdfError_NotImplemented:
      text = "NotImplemented";
      break;
    case PoDoFo::ePdfError_DestinationAlreadyPresent:
      text = "DestinationAlreadyPresent";
      break;
    case PoDoFo::ePdfError_ChangeOnImmutable:
      text = "ChangeOnImmutable";
      break;
    case PoDoFo::ePdfError_NotCompiled:
      text = "NotCompiled";
      break;
    case PoDoFo::ePdfError_OutlineItemAlreadyPresent:
      text = "OutlineItemAlreadyPresent";
      break;
    case PoDoFo::ePdfError_NotLoadedForUpdate:
      text = "NotLoadedForUpdate";
      break;
    case PoDoFo::ePdfError_CannotEncryptedForUpdate:
      text = "CannotEncryptedForUpdate";
      break;
    case PoDoFo::ePdfError_Unknown:
      text = "Unknown";
      break;
  }
  return text;
}
