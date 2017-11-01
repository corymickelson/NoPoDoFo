#include <napi.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <podofo/podofo.h>

class Signature : public Napi::ObjectWrap<Signature>
{
public:
  explicit Signature(const Napi::CallbackInfo& info);
  ~Signature()
  {
    if (key)
      EVP_PKEY_free(key);

    if (cert)
      X509_free(cert);

    delete cert;
    delete key;
    delete doc;
    delete signer;
    delete signatureField;
    delete annot;
  }
  static void Initialize(Napi::Env& env, Napi::Object& target);

  void LoadCert(const Napi::CallbackInfo&);
  void LoadPKey(const Napi::CallbackInfo&);
  Napi::Value Sign(const Napi::CallbackInfo&);
  Napi::Value HasExistingField(const Napi::CallbackInfo&);
  void SetPage(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetPage(const Napi::CallbackInfo&);
  void SetField(const Napi::CallbackInfo&, const Napi::Value&);
  Napi::Value GetField(const Napi::CallbackInfo&);
  void WritesTo(const Napi::CallbackInfo&);

private:
  X509* cert;
  EVP_PKEY* key;
  PoDoFo::PdfMemDocument* doc;
  PoDoFo::PdfSignOutputDevice* signer;
  PoDoFo::PdfSignatureField* signatureField = nullptr;
  PoDoFo::PdfAnnotation* annot = nullptr;
  PoDoFo::PdfRefCountedBuffer refBuffer;
  std::string reason;
  std::string password;
  int pageIndex;
  PoDoFo::pdf_int32 signatureSize;
};
