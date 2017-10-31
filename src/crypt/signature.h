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
  ~Signature() {}
  static void Initialize(Napi::Env& env, Napi::Object& target);

private:
  X509* cert;
  EVP_PKEY* key;
  PoDoFo::PdfMemDocument* doc;
  PoDoFo::PdfSignOutputDevice* signer;
};
