"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const mod = require('../src/npdf');
class Document {
    constructor(file) {
        this._instance = new mod.Document();
    }
    load(file) {
        this._instance.load(file);
    }
    getPageCount() {
        return this._instance.getPageCount();
    }
    getPage(pageN) {
        const page = this._instance.getPage(pageN);
        return page;
    }
    mergeDocument(doc) {
        this._instance.mergeDocument(doc);
    }
    deletePage(pageIndex) {
        this._instance.deletePage(pageIndex);
    }
    getVersion() {
        return this._instance.getVersion();
    }
    isLinearized() {
        return this._instance.isLinearized();
    }
    setPassword(password) {
        this._instance.setPassword(password);
    }
    write(file) {
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
class Painter {
    constructor() {
        this._instance = new mod.Painter();
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
        this._instance = new mod.Rect(page);
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
}
exports.Image = Image;
class Signature {
}
exports.Signature = Signature;
class Annotation {
}
exports.Annotation = Annotation;
