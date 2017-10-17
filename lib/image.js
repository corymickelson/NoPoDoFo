"use strict";
// const mod = require('../src/npdf')
Object.defineProperty(exports, "__esModule", { value: true });
var document_1 = require("./document");
var Image = /** @class */ (function () {
    /**
     *
     * @param {IDocument} _doc - document to embed image in
     * @param {string | Buffer} data - image file path, or image buffer (buffer not yet implemented)
     */
    function Image(_doc, data) {
        this._doc = _doc;
        if (data) {
            this._instance = new document_1.__mod.Image(_doc._instance, data);
        }
        else {
            this._instance = new document_1.__mod.Image(_doc._instance);
        }
    }
    Image.prototype.getWidth = function () {
        return this._instance.getWidth();
    };
    Image.prototype.getHeight = function () {
        return this._instance.getHeight();
    };
    Image.prototype.loadFromFile = function (file) {
        this._instance.setFile(file);
    };
    /**
     * @description Embeds data as an image on the document. To use this image pass to Painter.drawImage.
     *      New instances will reuse embedded img object if passed the same image reference.
     * @throws {TypeError} - throw type error on invalid arguments.
     * @param data img data
     * @returns void
     */
    Image.prototype.loadFromBuffer = function (data) {
        if (Buffer.isBuffer(data))
            this._instance.setData(data.toString('utf8'));
        else if (typeof data === 'string' || data instanceof String)
            this._instance.setData(data);
        else
            throw new TypeError("Image.setData requires a single argument of type string | Buffer");
    };
    Image.prototype.isLoaded = function () {
        return this._instance.isLoaded();
    };
    Image.prototype.setInterpolate = function (value) {
        this._instance.setInterpolate(value);
    };
    return Image;
}());
exports.Image = Image;
