import {access, unlinkSync} from 'fs'
import {join} from 'path'
import * as tap from 'tape'
import {Document, FontEncoding} from './document'
import {F_OK} from "constants";
import {v4} from 'uuid'
import {Test} from "tape";
import {Obj} from "./object";

const filePath = join(__dirname, '../test-documents/test.pdf'),
    pwdDoc = join(__dirname, '../test-documents/pwd.pdf')

export const end = (...tests: Test[]) => tests.forEach(t => t.end())

tap('Document Api', sub => {
    sub.test('unprotected documents', standard => {
        const doc = new Document(filePath)
        doc.on('ready', (pdf: Document) => {

            standard.test('loaded successfully and emitted ready event', t => {
                if (pdf instanceof Error) t.fail('Failed to load document')
                t.assert(pdf.loaded === true, 'pdf successfully loaded')
                end(t)
            })

            standard.test('document properties getter/setter (s)', t => {
                t.assert(pdf.getPageCount() > 0, 'Page count exists')
                t.throws(() => pdf.password, 'Passwords are kept secret')
                t.doesNotThrow(() => pdf.encrypt, 'Encrypt should not throw, if not exits return null')
                t.throws(() => pdf.getPage(10), 'Range Error')
                end(t)
            })

            standard.test('document trailer, catalog, objects', t => {
                let objs = pdf.getObjects();
                t.assert(objs instanceof Array, 'Gets all objects as an array')
                t.assert(objs[0] instanceof Obj, 'array items instance of Obj')
                let trailer = pdf.getTrailer(),
                    catalog = pdf.getCatalog()
                t.assert(trailer instanceof Obj, 'get trailer')
                t.assert(trailer.type === 'Dictionary', 'trailer is a dictionary')
                t.assert(catalog instanceof Obj, 'get catalog')
                t.assert(catalog.type === 'Dictionary', 'catalog is a dictionary')
                t.end()
            })

            standard.test('document create font', t => {
                let font = pdf.createFont({fontName: 'monospace', encoding: FontEncoding.WinAnsi})
                const m = font.getMetrics()
                t.ok(m)
                end(t)
            })

            standard.test('document delete page object by index', t => {
                const beforeCount = pdf.getPageCount()
                pdf.deletePage(0)
                const afterCount = pdf.getPageCount()
                t.assert(beforeCount - 1 === afterCount, 'Page removed and page count decremented')
                end(t)
            })

            standard.test('document information', t => {
                t.false(pdf.isLinearized(), 'test.pdf is a non-linearized document')
                t.true(pdf.getVersion() === 1.7, 'test.pdf PDF version is 1.7')
                end(t)
            })

            standard.test('can write', t => {
                const outFile = `${v4()}.pdf`
                pdf.write((e, v) => {
                    if (e instanceof Error) t.fail('Failed to write document')
                    access(outFile, F_OK, err => {
                        if (err) t.fail('Failed to write document')
                        else {
                            unlinkSync(outFile)
                            t.pass("Modified file written successfully")
                            end(t)
                        }
                    })
                }, outFile)
            })
            standard.test('is allowed', t => {
                t.ok(pdf.isAllowed('Copy'), 'Copy protection not defined. Can get ProtectionProperties')
                t.end()
            })
        })

        sub.test('document exceptions', standard => {
            standard.test('password required, password setter', t => {
                t.plan(1)
                const doc = new Document(pwdDoc)
                doc.on('ready', () => {
                    t.fail('Should have thrown password required error')
                })
                    .on('error', e => {
                        if (e.message === "Password required to modify this document") {
                            try {
                                doc.password = 'secret'
                                t.pass('password required and password set')
                            } catch (pwdErr) {
                                t.fail(pwdErr.message)
                            }
                        }
                    })
            })
            standard.test('password required', t => {
                const doc = new Document(pwdDoc)
                doc.on('ready', () => {
                    t.fail('Should have thrown password required error')
                })
                    .on('error', e => {
                        t.ok(e instanceof Error, 'should bubble up error')
                        t.assert(e.message === "Password required to modify this document")
                        end(t)
                    })
            })
            standard.test('file not found', t => {
                let doc = new Document('/bad/path')
                doc.on('ready', e => {
                    t.fail('should have thrown "file not found" error.')
                })
                    .on('error', (e: Error) => {
                        t.assert(e.message === 'file not found')
                        end(t)
                    })
            })
        })

        sub.test('document merger', standard => {
            const doc = new Document(filePath),
                outFile = `${v4()}.pdf`
            doc.on('ready', () => {
                let originalPC = doc.getPageCount()
                doc.mergeDocument(filePath)
                doc.write(e => {
                    if (e) standard.fail(e.message)
                    const doc2 = new Document(outFile)
                    doc2.on('ready', () => {
                        let doc2PC = doc2.getPageCount()
                        standard.assert(originalPC * 2 === doc2PC, "merged document")
                        unlinkSync(outFile)
                        standard.end()
                    })
                }, outFile)
            })
        })

        sub.test('write buffer', standard => {
            const doc = new Document(filePath)
            doc.on('ready', (pdf: Document) => {
                pdf.write((e, d) => {
                    if (e) standard.fail(e.message)
                    else {
                        standard.ok(d)
                        standard.assert(d instanceof Buffer)
                        standard.assert((d as Buffer).length > 0)
                        end(standard)
                    }
                })
            })
        })

        sub.test('gc', standard => {
            const output = `/tmp/${v4()}.pdf`
            Document.gc(filePath, "", output, e => {
                if (e instanceof Error) standard.fail()
                else {
                    access(output, F_OK, err => {
                        if (err) standard.fail()
                        else {
                            standard.pass('Gc doc written to output location')
                            standard.end()
                        }
                    })
                }
            })
        })
    })

})