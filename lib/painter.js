"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var document_1 = require("./document");
var Painter = /** @class */ (function () {
    function Painter(page) {
        this._instance = new document_1.__mod.Painter();
        if (page)
            this._instance.setPage(page);
    }
    Object.defineProperty(Painter.prototype, "page", {
        get: function () { return this._instance.page; },
        set: function (value) { this._instance.page = value._instance; },
        enumerable: true,
        configurable: true
    });
    Painter.prototype.finishPage = function () {
        this._instance.finishPage();
    };
    Painter.prototype.drawText = function () {
        throw new Error("Method not implemented.");
    };
    /**
     *
     * @param {IImage} img - an instance of Image
     * @param {number} x - x coordinate (bottom left position of image)
     * @param {number} y - y coordinate (bottom position of image)
     * @param {{width:number, heigth:number}} scale - optional scaling
     */
    Painter.prototype.drawImage = function (img, x, y, scale) {
        scale ?
            this._instance.drawImage(img._instance, x, y, scale.width, scale.height) :
            this._instance.drawImage(img._instance, x, y);
    };
    Painter.prototype.getPrecision = function () {
        return this._instance.getPrecision();
    };
    return Painter;
}());
exports.Painter = Painter;
