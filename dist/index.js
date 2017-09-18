"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const mod = require('../src/npdf');
var NPdfAnnotation;
(function (NPdfAnnotation) {
    NPdfAnnotation[NPdfAnnotation["Text"] = 0] = "Text";
    NPdfAnnotation[NPdfAnnotation["Link"] = 1] = "Link";
    NPdfAnnotation[NPdfAnnotation["FreeText"] = 2] = "FreeText";
    NPdfAnnotation[NPdfAnnotation["Line"] = 3] = "Line";
    NPdfAnnotation[NPdfAnnotation["Square"] = 4] = "Square";
    NPdfAnnotation[NPdfAnnotation["Circle"] = 5] = "Circle";
    NPdfAnnotation[NPdfAnnotation["Polygon"] = 6] = "Polygon";
    NPdfAnnotation[NPdfAnnotation["PolyLine"] = 7] = "PolyLine";
    NPdfAnnotation[NPdfAnnotation["Highlight"] = 8] = "Highlight";
    NPdfAnnotation[NPdfAnnotation["Underline"] = 9] = "Underline";
    NPdfAnnotation[NPdfAnnotation["Squiggly"] = 10] = "Squiggly";
    NPdfAnnotation[NPdfAnnotation["StrikeOut"] = 11] = "StrikeOut";
    NPdfAnnotation[NPdfAnnotation["Stamp"] = 12] = "Stamp";
    NPdfAnnotation[NPdfAnnotation["Caret"] = 13] = "Caret";
    NPdfAnnotation[NPdfAnnotation["Ink"] = 14] = "Ink";
    NPdfAnnotation[NPdfAnnotation["Popup"] = 15] = "Popup";
    NPdfAnnotation[NPdfAnnotation["FileAttachement"] = 16] = "FileAttachement";
    NPdfAnnotation[NPdfAnnotation["Sound"] = 17] = "Sound";
    NPdfAnnotation[NPdfAnnotation["Movie"] = 18] = "Movie";
    NPdfAnnotation[NPdfAnnotation["Widget"] = 19] = "Widget";
    NPdfAnnotation[NPdfAnnotation["Screen"] = 20] = "Screen";
    NPdfAnnotation[NPdfAnnotation["PrinterMark"] = 21] = "PrinterMark";
    NPdfAnnotation[NPdfAnnotation["TrapNet"] = 22] = "TrapNet";
    NPdfAnnotation[NPdfAnnotation["Watermark"] = 23] = "Watermark";
    NPdfAnnotation[NPdfAnnotation["_3D"] = 24] = "_3D";
    NPdfAnnotation[NPdfAnnotation["RichMedia"] = 25] = "RichMedia";
    NPdfAnnotation[NPdfAnnotation["WebMedia"] = 26] = "WebMedia";
})(NPdfAnnotation = exports.NPdfAnnotation || (exports.NPdfAnnotation = {}));
/**
 * @class Document
 * @description A Pdf document
 */
class Document {
    /**
     * @constructor
     * @param {string} [file] - pdf file path (optional)
     * @returns void
     */
    constructor(file) {
        this._loaded = false;
        this._instance = new mod.Document();
        if (file) {
            this._instance.load(file);
            this._pageCount = this._instance.getPageCount();
            this._loaded = true;
        }
    }
    get pageCount() {
        return this._pageCount;
    }
    /**
     * @description load pdf file
     * @param {string} file - pdf file path
     */
    load(file) {
        this._instance.load(file);
        this._loaded = true;
    }
    getPageCount() {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        return this._instance.getPageCount();
    }
    getPage(pageN) {
        if (this.pageCount > pageN || this.pageCount < 0) {
            throw new RangeError("pageN out of range");
        }
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        const page = this._instance.getPage(pageN);
        return page;
    }
    /**
     * @description Append doc to the end of the loaded doc
     * @param {string} doc - pdf file path
     */
    mergeDocument(doc) {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        this._instance.mergeDocument(doc);
    }
    deletePage(pageIndex) {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        if (this.pageCount < pageIndex || pageIndex < 0) {
            throw new RangeError('page index out of range');
        }
        this._instance.deletePage(pageIndex);
    }
    getVersion() {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        return this._instance.getVersion();
    }
    isLinearized() {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        return this._instance.isLinearized();
    }
    setPassword(password) {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        this._instance.setPassword(password);
    }
    write(file) {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        this._instance.write(file);
    }
}
exports.Document = Document;
class Field {
    constructor(page, fieldIndex) {
        this._instance = new mod.Field(page, fieldIndex);
    }
    getType() {
        return this._instance.getType();
    }
    getFieldName() {
        return this._instance.getFieldName();
    }
    getAlternateName() {
        return this._instance.getAlternateName();
    }
    getMappingName() {
        return this._instance.getMappingName();
    }
    isRequired() {
        return this._instance.isRequired();
    }
    setRequired(required) {
        this._instance.setRequired(required);
    }
    setAlternateName(name) {
        this._instance.setAlternateName(name);
    }
    setMappingName(name) {
        this._instance.setMappingName(name);
    }
}
exports.Field = Field;
class TextField {
    constructor(field) {
        this._instance = new mod.TextField(field);
    }
    getText() {
        return this._instance.getText();
    }
    setText(value) {
        return this._instance.setText(value);
    }
}
exports.TextField = TextField;
/**
 * @class Painter
 * @description Draw text / image to page
 */
class Painter {
    constructor(page) {
        this._instance = new mod.Painter();
        if (page)
            this._instance.setPage(page);
    }
    setPage(page) {
        this._instance.setPage(page);
    }
    finishPage() {
        this._instance.finishPage();
    }
    drawText() {
        throw new Error("Method not implemented.");
    }
    drawImage(imgFile, x, y, width, height) {
        this._instance.drawImage(imgFile, x, y, width, height);
    }
    getPrecision() {
        return this._instance.getPrecision();
    }
}
exports.Painter = Painter;
class Rect {
    constructor(page) {
        if (page)
            this._instance = new mod.Rect(page);
        else
            this._instance = new mod.Rect();
    }
    getBottom() {
        return this._instance.getBottom();
    }
    setBottom(value) {
        this._instance.setBottom(value);
    }
    getLeft() {
        return this._instance.getLeft();
    }
    setLeft(value) {
        this._instance.setLeft(value);
    }
    getWidth() {
        return this._instance.getWidth();
    }
    setWidth(value) {
        this._instance.setWidth(value);
    }
    getHeight() {
        return this._instance.getHeight();
    }
    setHeight(value) {
        this._instance.setHeight(value);
    }
}
exports.Rect = Rect;
class Image {
    constructor(_doc, data) {
        this._doc = _doc;
        if (data) {
            this._instance = new mod.Image(_doc, data);
        }
        else {
            this._instance = new mod.Image(_doc);
        }
    }
    getWidth() {
        return this._instance.getWidth();
    }
    getHeight() {
        return this._instance.getHeight();
    }
    setFile(file) {
        this._instance.setFile(file);
    }
    /**
     * @description Embeds data as an image on the document. To use this image pass to Painter.drawImage.
     *      New instances will reuse embedded img object if passed the same image reference.
     * @throws {TypeError} - throw type error on invalid arguments.
     * @param data img data
     * @returns void
     */
    setData(data) {
        if (Buffer.isBuffer(data))
            this._instance.setData(data.toString('utf8'));
        else if (typeof data === 'string' || data instanceof String)
            this._instance.setData(data);
        else
            throw new TypeError("Image.setData requires a single argument of type string | Buffer");
    }
    isLoaded() {
        return this._instance.isLoaded();
    }
}
exports.Image = Image;
class Signature {
}
exports.Signature = Signature;
class Annotation {
}
exports.Annotation = Annotation;
