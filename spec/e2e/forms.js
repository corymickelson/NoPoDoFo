"use strict";
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
Object.defineProperty(exports, "__esModule", { value: true });
const alsatian_1 = require("alsatian");
const __1 = require("../../");
let FormsSpec = class FormsSpec {
    setupTests() {
        this.doc = new __1.nopodofo.StreamDocument();
        this.page = this.doc.createPage(new __1.nopodofo.Rect(0, 0, 612, 792));
    }
    createStreamDocumentForm() {
        alsatian_1.Expect(this.doc.form).toBeDefined();
        alsatian_1.Expect(this.doc.form instanceof __1.nopodofo.Form).toBeTruthy();
    }
    formInstanceAccessors() {
        const keys = this.doc.form.dictionary.getKeys();
        alsatian_1.Expect(keys.includes(__1.NPDFName.DA)).toBeDefined();
        alsatian_1.Expect(keys.includes(__1.NPDFName.DR)).toBeDefined();
        alsatian_1.Expect(keys.includes(__1.NPDFName.FIELDS)).toBeDefined();
        alsatian_1.Expect(keys.includes(__1.NPDFName.SIG_FLAGS)).toBeDefined();
    }
    formDefaults() {
        alsatian_1.Expect(this.doc.form.DA.includes('0 0 0 rg')).toEqual(true);
        alsatian_1.Expect(this.doc.form.DA.includes('12')).toEqual(true);
        alsatian_1.Expect(this.doc.form.needAppearances).toBe(false);
    }
    formResourceDictionary() {
        let dr = this.doc.form.DR;
        alsatian_1.Expect(dr).toBeDefined();
        alsatian_1.Expect(dr.hasKey(__1.NPDFName.FONT)).toBeTruthy();
        alsatian_1.Expect(dr.getKey(__1.NPDFName.FONT)).toBeDefined();
    }
};
__decorate([
    alsatian_1.Setup
], FormsSpec.prototype, "setupTests", null);
__decorate([
    alsatian_1.Test("Create StreamDocument Form")
], FormsSpec.prototype, "createStreamDocumentForm", null);
__decorate([
    alsatian_1.Test("Instance Accessors")
], FormsSpec.prototype, "formInstanceAccessors", null);
__decorate([
    alsatian_1.Test("Form Defaults")
], FormsSpec.prototype, "formDefaults", null);
__decorate([
    alsatian_1.Test("Form Resource Dictionary")
], FormsSpec.prototype, "formResourceDictionary", null);
FormsSpec = __decorate([
    alsatian_1.TestFixture("New Forms Operations")
], FormsSpec);
exports.FormsSpec = FormsSpec;
//# sourceMappingURL=forms.js.map