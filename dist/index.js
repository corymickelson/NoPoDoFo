"use strict";
const mod = require('../src/npdf');
class Document {
    load(file) {
        throw new Error("Method not implemented.");
    }
    getPageCount() {
        throw new Error("Method not implemented.");
    }
    getPage(pageN) {
        throw new Error("Method not implemented.");
    }
    mergeDocument(doc) {
        throw new Error("Method not implemented.");
    }
    deletePage(pageIndex) {
        throw new Error("Method not implemented.");
    }
    getVersion() {
        throw new Error("Method not implemented.");
    }
    isLinearized() {
        throw new Error("Method not implemented.");
    }
    setPassword(password) {
        throw new Error("Method not implemented.");
    }
    write(file) {
        throw new Error("Method not implemented.");
    }
    constructor(file) {
        this._instance = new mod.Document();
    }
}
class Page {
    getRotation() {
        throw new Error("Method not implemented.");
    }
    getNumFields() {
        throw new Error("Method not implemented.");
    }
    getField(index) {
        throw new Error("Method not implemented.");
    }
    setRotation(degree) {
        throw new Error("Method not implemented.");
    }
    getBottom() {
        throw new Error("Method not implemented.");
    }
    setBottom(value) {
        throw new Error("Method not implemented.");
    }
    getLeft() {
        throw new Error("Method not implemented.");
    }
    setLeft(value) {
        throw new Error("Method not implemented.");
    }
    getWidth() {
        throw new Error("Method not implemented.");
    }
    setWidth(value) {
        throw new Error("Method not implemented.");
    }
    getHeight() {
        throw new Error("Method not implemented.");
    }
    setHeight(value) {
        throw new Error("Method not implemented.");
    }
    getFields() {
        throw new Error("Method not implemented.");
    }
    getFieldIndex(fieldName) {
        throw new Error("Method not implemented.");
    }
}
