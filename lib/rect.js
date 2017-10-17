"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var page_1 = require("./page");
var document_1 = require("./document");
var Rect = /** @class */ (function () {
    function Rect(position) {
        if (Array.isArray(position)) {
            this._instance = new document_1.__mod.Rect(position[0], position[1], position[2], position[3]);
        }
        else if (position instanceof page_1.Page) {
            this._instance = new document_1.__mod.Rect(position);
        }
        else {
            this._instance = new document_1.__mod.Rect();
        }
    }
    Object.defineProperty(Rect.prototype, "bottom", {
        get: function () {
            return this._instance.bottom;
        },
        set: function (value) {
            this._instance.bottom = value;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Rect.prototype, "left", {
        get: function () {
            return this._instance.left;
        },
        set: function (value) {
            this._instance.left = value;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Rect.prototype, "width", {
        get: function () {
            return this._instance.width;
        },
        set: function (value) {
            this._instance.width = value;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Rect.prototype, "height", {
        get: function () {
            return this._instance.height;
        },
        set: function (value) {
            this._instance.height = value;
        },
        enumerable: true,
        configurable: true
    });
    Rect.prototype.intersect = function (rect) {
        this._instance.intersect(rect);
    };
    return Rect;
}());
exports.Rect = Rect;
