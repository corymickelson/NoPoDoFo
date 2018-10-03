import {Expect, Test, TestFixture, Setup} from 'alsatian'
import {nopodofo as npdf, NPDFName} from '../../'

@TestFixture("New Forms Operations")
export class FormsSpec {
    public doc?: npdf.Base
    public page?: npdf.Page

    @Setup
    public setupTests() {
        this.doc = new npdf.StreamDocument()
        this.page = this.doc.createPage(new npdf.Rect(0,0,612,792))
    }

    @Test("Create StreamDocument Form")
    public createStreamDocumentForm() {
        Expect((this.doc as npdf.StreamDocument).form).toBeDefined()
        Expect(((this.doc as npdf.StreamDocument).form as any) instanceof npdf.Form).toBeTruthy()
    }

    @Test("Instance Accessors")
    public formInstanceAccessors() {
        const keys = (this.doc as npdf.StreamDocument).form.dictionary.getKeys()
        Expect(keys.includes(NPDFName.DA)).toBeDefined()
        Expect(keys.includes(NPDFName.DR)).toBeDefined()
        Expect(keys.includes(NPDFName.FIELDS)).toBeDefined()
        Expect(keys.includes(NPDFName.SIG_FLAGS)).toBeDefined()
    }

    @Test("Form Defaults")
    public formDefaults() {
        Expect(((this.doc as npdf.StreamDocument).form.DA as string).includes('0 0 0 rg')).toEqual(true)
        Expect(((this.doc as npdf.StreamDocument).form.DA as string).includes('12')).toEqual(true)
        Expect((this.doc as npdf.StreamDocument).form.needAppearances).toBe(false)
    }

    @Test("Form Resource Dictionary")
    public formResourceDictionary() {
        let dr = (this.doc as npdf.StreamDocument).form.DR as npdf.Dictionary
        Expect(dr).toBeDefined()
        Expect(dr.hasKey(NPDFName.FONT)).toBeTruthy()
        Expect(dr.getKey(NPDFName.FONT)).toBeDefined()
    }
}