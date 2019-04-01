import {AsyncTest, Expect, TestCase, TestFixture} from 'alsatian'
import {nopodofo, ProtectionOption} from '../../'
import {join} from "path";

@TestFixture("Encryption")
export class EncryptionSpec {
    @AsyncTest("Create encryption object")
    @TestCase('secret', 'secret', 40, ['Edit', 'FillAndSign'], 'rc4v1')
    @TestCase('secret', 'secret', 128, ['Edit', 'FillAndSign', 'Copy'], 'aesv2')
    @TestCase('secret', 'secret', 56, ['Edit', 'Accessible'], 'rc4v2')
    @TestCase('secret', 'secret', 256, ['Edit', 'HighPrint'], 'aesv3')
    @TestCase('secret', 'secret', 128, ['EditNotes', 'Copy', 'HighPrint', 'DocAssembly'], 'rc4v2')

    public async createEncryptSpec(userPassword: string,
                                   ownerPassword: string,
                                   keyLength: number /* 40 56 80 96 128 256*/,
                                   protection: Array<ProtectionOption>,
                                   algorithm: 'rc4v1' | 'rc4v2' | 'aesv2' | 'aesv3'
    ) {
        return new Promise((resolve, reject) => {
            try {
                const subject = nopodofo.Encrypt.createEncrypt({
                    ownerPassword,
                    userPassword,
                    keyLength,
                    protection,
                    algorithm
                }) as nopodofo.Encrypt
                const doc = new nopodofo.Document()
                doc.load(join(__dirname, '../test-documents/test.pdf'), err => {
                    if (err) Expect.fail(err.message)
                    doc.encrypt = subject
                    doc.write((e, data) => {
                        if (e) Expect.fail(e.message)
                        const secured = new nopodofo.Document()
                        secured.load(data, {password: 'secret'}, err => {
                            if (err) Expect.fail(err.message)
                            Expect(secured.encrypt).toBeDefined()
                            Expect(secured.encrypt.keyLength).toEqual(keyLength)
                            for (let prop in secured.encrypt.protections) {
                                if (protection.includes(prop as any)) {
                                    Expect(secured.encrypt.protections[prop as ProtectionOption]).toBeTruthy()
                                }
                            }
                            return resolve()
                        })
                    })
                })
            } catch (e) {
                reject(e)
            }
        })
    }

    @AsyncTest("Password validation")
    @TestCase('secret', 'secret')
    @TestCase('secret')
    public async passwordValidationSpec(owner: string, user?: string) {
        const doc = new nopodofo.Document()
        return new Promise((resolve, reject) => {
            doc.load(join(__dirname, '../test-documents/test.pdf'), err => {
                if (err) Expect.fail(err.message)
                const opts = Object.assign({
                    keyLength: 128,
                    protection: ['Edit'],
                    algorithm: 'aesv2'
                }, user ? {userPassword: user, ownerPassword: owner}
                    : {ownerPassword: owner})
                doc.encrypt = nopodofo.Encrypt.createEncrypt(opts) as nopodofo.Encrypt
                doc.write(async (err, data) => {
                    if (err) Expect.fail(err.message)
                    const child = new nopodofo.Document()
                    if (user) { // Expect a failure since a user pwd has been provided
                        await Expect(() =>
                            new Promise((resolve, reject) => {
                                child.load(data, err => err ? reject(err) : resolve())
                            })
                        ).toThrowErrorAsync(Error, "Password required to modify this document")
                    } else { // Expect no failure since a user pwd was not set
                        await new Promise(resolve => {
                            child.load(data, err => {
                                if (err) Expect.fail(err.message)
                                return resolve()
                            })
                        })

                    }
                    return resolve()
                })
            })
        })
    }
}
