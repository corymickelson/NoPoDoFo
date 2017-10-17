"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var field_1 = require("./field");
var rect_1 = require("./rect");
var object_1 = require("./object");
var annotation_1 = require("./annotation");
var Page = /** @class */ (function () {
    function Page(_instance) {
        this._instance = _instance;
    }
    Object.defineProperty(Page.prototype, "trimBox", {
        get: function () {
            var trimBoxRect = this._instance.trimBox;
            return new rect_1.Rect([trimBoxRect.left, trimBoxRect.bottom, trimBoxRect.width, trimBoxRect.height]);
        },
        set: function (rect) {
            Page.assertRect(rect);
            this._instance.trimBox = rect;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Page.prototype, "number", {
        get: function () {
            return this._instance.number;
        },
        set: function (n) {
            throw Error("Can not change page number. Use Document.deletePage() to adjust page order.");
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Page.prototype, "width", {
        get: function () {
            return this._instance.width;
        },
        set: function (value) {
            if (Number.isNaN(value)) { }
            this._instance.width = value;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Page.prototype, "height", {
        get: function () {
            return this._instance.height;
        },
        set: function (value) {
            if (Number.isNaN(value)) { }
            this._instance.height = value;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Page.prototype, "rotation", {
        get: function () {
            return this._instance.rotation;
        },
        set: function (degree) {
            if (degree < 0 || degree > 270) {
                throw Error('rotation must be one of: 0, 90, 180, 270');
            }
            this._instance.rotation = degree;
        },
        enumerable: true,
        configurable: true
    });
    Page.prototype.getContents = function (append) {
        var objInstance = this._instance.getContents(append);
        return new object_1.Obj(objInstance);
    };
    Page.prototype.getResources = function () {
        var objInstance = this._instance.getResources();
        return new object_1.Obj(objInstance);
    };
    Page.prototype.getMediaBox = function () {
        var mediaBoxPositions = this._instance.getMediaBox();
        var mediaBoxRect = new rect_1.Rect([mediaBoxPositions.left, mediaBoxPositions.bottom, mediaBoxPositions.width, mediaBoxPositions.height]);
        return mediaBoxPositions;
    };
    Page.prototype.getBleedBox = function () {
        var positions = this._instance.getBleedBox();
        return new rect_1.Rect([positions.left, positions.bottom, positions.width, positions.height]);
    };
    Page.prototype.getArtBox = function () {
        var positions = this._instance.getArtBox();
        return new rect_1.Rect([positions.left, positions.bottom, positions.width, positions.height]);
    };
    Page.prototype.getNumFields = function () {
        return this._instance.getNumFields();
    };
    Page.prototype.getFieldsInfo = function () {
        return this._instance.getFieldsInfo();
    };
    Page.prototype.getFieldIndex = function (fieldName) {
        return this._instance.getFieldIndex(fieldName);
    };
    Page.prototype.getField = function (index) {
        return new field_1.Field(this, index);
    };
    Page.prototype.getFields = function () {
        var fields = [];
        var count = this.getNumFields(), i = 0;
        for (; i < count; i++) {
            fields[i] = new field_1.Field(this, i);
        }
        return fields;
    };
    Page.prototype.createAnnotation = function (type, rect) {
        var instance = this._instance.createAnnotation(type, rect._instance);
        return new annotation_1.Annotation(instance);
    };
    Page.prototype.getAnnotation = function (index) {
        var instance = this._instance.getAnnotation(index);
        return new annotation_1.Annotation(instance);
    };
    Page.prototype.getNumAnnots = function () {
        return this._instance.getNumAnnots();
    };
    Page.prototype.getAnnotations = function () {
        var count = this.getNumAnnots();
        var output = [];
        for (var i = 0; i < count; i++) {
            try {
                var item = this.getAnnotation(i);
                output.push(item);
            }
            catch (e) {
                throw e;
            }
        }
        return output;
    };
    Page.prototype.deleteAnnotation = function (index) {
        this._instance.deleteAnnotation(index);
    };
    Page.assertRect = function (rect) {
        if (rect.bottom === null ||
            rect.height === null ||
            rect.left === null ||
            rect.width === null) {
            throw Error("Rect must be initialized before use.");
        }
    };
    return Page;
}());
exports.Page = Page;
