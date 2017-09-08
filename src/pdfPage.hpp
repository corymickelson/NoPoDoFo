//
// Created by red on 9/7/17.
//

#ifndef NPDF_PDFPAGE_HPP
#define NPDF_PDFPAGE_HPP

#include <napi.h>
#include <podofo/podofo.h>

using namespace Napi;
using namespace PoDoFo;
using namespace std;

class pdfPage : public ObjectWrap<pdfPage> {
public:
	explicit pdfPage( const CallbackInfo &callbackInfo );

	~pdfPage( ) { }

	static void Initialize( Env &env, Object &target ) { }

	inline PdfPage *page( ) { return _page; }

private:
	PdfPage *_page;
};


#endif //NPDF_PDFPAGE_HPP
