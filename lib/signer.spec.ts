import {writeFile, writeFileSync} from 'fs'
import {Document} from './document'
import {SignatureField} from './field'
import {Annotation, NPDFAnnotation, NPDFAnnotationFlag} from './annotation'
import {Rect} from './rect'
import {join} from 'path'
import {Form} from "./form";

import * as forge from 'node-forge'
import {Signer} from "./signer";


signatureBuffer()

function signatureBuffer(): any {
    const pki = forge.pki,
        cert = (pki as any).createCertificate()
    pki.rsa.generateKeyPair({bits: 2048}, (e: Error, keys: any) => {

        cert.publicKey = keys.privateKey
        cert.serialNumber = '01'
        cert.validity.notBefore = new Date()
        cert.validity.notAfter = new Date()
        cert.validity.notAfter.setFullYear(cert.validity.notBefore.getFullYear() + 1);
        const attrs = [{
            name: 'commonName',
            value: 'example.org'
        }, {
            name: 'countryName',
            value: 'US'
        }, {
            shortName: 'ST',
            value: 'Virginia'
        }, {
            name: 'localityName',
            value: 'Blacksburg'
        }, {
            name: 'organizationName',
            value: 'Test'
        }, {
            shortName: 'OU',
            value: 'Test'
        }];
        cert.setSubject(attrs);
// alternatively set subject from a csr
//cert.setSubject(csr.subject.attributes);
        cert.setIssuer(attrs);
        cert.setExtensions([{
            name: 'basicConstraints',
            cA: true
        }, {
            name: 'keyUsage',
            keyCertSign: true,
            digitalSignature: true,
            nonRepudiation: true,
            keyEncipherment: true,
            dataEncipherment: true
        }, {
            name: 'extKeyUsage',
            serverAuth: true,
            clientAuth: true,
            codeSigning: true,
            emailProtection: true,
            timeStamping: true
        }, {
            name: 'nsCertType',
            client: true,
            server: true,
            email: true,
            objsign: true,
            sslCA: true,
            emailCA: true,
            objCA: true
        }, {
            name: 'subjectAltName',
            altNames: [{
                type: 6, // URI
                value: 'http://example.org/webid#me'
            }, {
                type: 7, // IP
                ip: '127.0.0.1'
            }]
        }, {
            name: 'subjectKeyIdentifier'
        }]);
        /* alternatively set extensions from a csr
        var extensions = csr.getAttribute({name: 'extensionRequest'}).extensions;
        // optionally add more extensions
        extensions.push.apply(extensions, [{
          name: 'basicConstraints',
          cA: true
        }, {
          name: 'keyUsage',
          keyCertSign: true,
          digitalSignature: true,
          nonRepudiation: true,
          keyEncipherment: true,
          dataEncipherment: true
        }]);
        cert.setExtensions(extensions);
        */
// self-sign certificate
        cert.sign(keys.privateKey);

        // cert.md.messageLength
        // fulfill(cert.signature)
// convert a Forge certificate to PEM
//     var pem = pki.certificateToPem(cert);

// convert a Forge certificate from PEM
//     var cert = pki.certificateFromPem(pem);

// convert an ASN.1 X.509x3 object to a Forge certificate
//     var cert = pki.certificateFromAsn1(obj);

// convert a Forge certificate to an ASN.1 X.509v3 object
        const asn1Cert = (pki as any).certificateToAsn1(cert);
        const der = forge.asn1.toDer(asn1Cert)

        const doc = new Document(join(__dirname, '../test-documents/test.pdf'), true)
        doc.on('ready', async e => {
            if (e instanceof Error) throw e
            try {
                // 1.) handle form
                // 2.) handle signture field
                // 3.) get signature binary
                // 4.) create Signer
                // 5.) sign => output signed pdf

                let data = der
                // check for form
                let form = new Form(doc),
                    formDict = form.getObject().asDictionary()
                if (!formDict.hasKey('SigFlags') ||
                    formDict.getKey('SigFlags').type !== 'Number' ||
                    formDict.getKey('SigFlags').asNumber() !== 3) {
                    if (formDict.hasKey('SigFlags'))
                        formDict.removeKey('SigFlags')
                    formDict.addKey('SigFlags', 3)
                }
                if (form.needAppearances)
                    form.needAppearances = false

                // create signature field
                const rect = new Rect([0, 0, 10, 10]),
                    page = doc.getPage(doc.pageCount - 1),
                    annot = page.createAnnotation(NPDFAnnotation.Widget, rect)
                annot.flag = NPDFAnnotationFlag.Hidden | NPDFAnnotationFlag.Invisible
                const field = new SignatureField(annot, form, doc)
                field.setReason('test')
                field.setLocation('here')
                field.setCreator('me')
                field.setDate()

                let signer = new Signer(doc, field)
                let what = signer.sign(Buffer.from(der.data))
                    // .then(what => {
                console.log(what)
                        if (what) {
                            writeFileSync('/tmp/test.pdf', what)
                            console.log('ok')
                        }
                    // })
                    // .catch(e => console.error(e))

                // let working = signer.signWithCertificateAndKey(join(__dirname, '../certificate.pem'), join(__dirname, '../key.pem'))

            }
            catch
                (e) {
                console.error(e)
            }
        })

        // console.log(asn1Cert)
    })

}
