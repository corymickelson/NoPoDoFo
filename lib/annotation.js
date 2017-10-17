"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var NPdfAnnotationType;
(function (NPdfAnnotationType) {
    NPdfAnnotationType["Text"] = "Text";
    NPdfAnnotationType["Link"] = "Link";
    NPdfAnnotationType["FreeText"] = "FreeText";
    NPdfAnnotationType["Line"] = "Line";
    NPdfAnnotationType["Square"] = "Square";
    NPdfAnnotationType["Circle"] = "Circle";
    NPdfAnnotationType["Polygon"] = "Polygon";
    NPdfAnnotationType["PolyLine"] = "PolyLine";
    NPdfAnnotationType["Highlight"] = "Highlight";
    NPdfAnnotationType["Underline"] = "Underline";
    NPdfAnnotationType["Squiggly"] = "Squiggly";
    NPdfAnnotationType["StrikeOut"] = "StrikeOut";
    NPdfAnnotationType["Stamp"] = "Stamp";
    NPdfAnnotationType["Caret"] = "Caret";
    NPdfAnnotationType["Ink"] = "Ink";
    NPdfAnnotationType["Popup"] = "Popup";
    NPdfAnnotationType["FileAttachment"] = "FileAttachment";
    NPdfAnnotationType["Sound"] = "Sound";
    NPdfAnnotationType["Movie"] = "Movie";
    NPdfAnnotationType["Widget"] = "Widget";
    NPdfAnnotationType["Screen"] = "Screen";
    NPdfAnnotationType["PrinterMark"] = "PrinterMark";
    NPdfAnnotationType["TrapNet"] = "TrapNet";
    NPdfAnnotationType["Watermark"] = "Watermark";
    NPdfAnnotationType["_3D"] = "3D";
    NPdfAnnotationType["RichMedia"] = "RichMedia";
    NPdfAnnotationType["WebMedia"] = "WebMedia";
})(NPdfAnnotationType = exports.NPdfAnnotationType || (exports.NPdfAnnotationType = {}));
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
var NpdfAnnotationFlag;
(function (NpdfAnnotationFlag) {
    NpdfAnnotationFlag[NpdfAnnotationFlag["Invisible"] = 1] = "Invisible";
    NpdfAnnotationFlag[NpdfAnnotationFlag["Hidden"] = 2] = "Hidden";
    NpdfAnnotationFlag[NpdfAnnotationFlag["Print"] = 4] = "Print";
    NpdfAnnotationFlag[NpdfAnnotationFlag["NoZoom"] = 8] = "NoZoom";
    NpdfAnnotationFlag[NpdfAnnotationFlag["NoRotate"] = 16] = "NoRotate";
    NpdfAnnotationFlag[NpdfAnnotationFlag["NoView"] = 32] = "NoView";
    NpdfAnnotationFlag[NpdfAnnotationFlag["ReadOnly"] = 64] = "ReadOnly";
    NpdfAnnotationFlag[NpdfAnnotationFlag["Locked"] = 128] = "Locked";
    NpdfAnnotationFlag[NpdfAnnotationFlag["ToggleNoView"] = 256] = "ToggleNoView";
    NpdfAnnotationFlag[NpdfAnnotationFlag["LockedContents"] = 512] = "LockedContents";
})(NpdfAnnotationFlag = exports.NpdfAnnotationFlag || (exports.NpdfAnnotationFlag = {}));
var NpdfAction;
(function (NpdfAction) {
    NpdfAction[NpdfAction["GoTo"] = 0] = "GoTo";
    NpdfAction[NpdfAction["GoToR"] = 1] = "GoToR";
    NpdfAction[NpdfAction["GoToE"] = 2] = "GoToE";
    NpdfAction[NpdfAction["Launch"] = 3] = "Launch";
    NpdfAction[NpdfAction["Thread"] = 4] = "Thread";
    NpdfAction[NpdfAction["URI"] = 5] = "URI";
    NpdfAction[NpdfAction["Sound"] = 6] = "Sound";
    NpdfAction[NpdfAction["Movie"] = 7] = "Movie";
    NpdfAction[NpdfAction["Hide"] = 8] = "Hide";
    NpdfAction[NpdfAction["Named"] = 9] = "Named";
    NpdfAction[NpdfAction["SubmitForm"] = 10] = "SubmitForm";
    NpdfAction[NpdfAction["ResetForm"] = 11] = "ResetForm";
    NpdfAction[NpdfAction["ImportData"] = 12] = "ImportData";
    NpdfAction[NpdfAction["JavaScript"] = 13] = "JavaScript";
    NpdfAction[NpdfAction["SetOCGState"] = 14] = "SetOCGState";
    NpdfAction[NpdfAction["Rendition"] = 15] = "Rendition";
    NpdfAction[NpdfAction["Trans"] = 16] = "Trans";
    NpdfAction[NpdfAction["GoTo3DView"] = 17] = "GoTo3DView";
    NpdfAction[NpdfAction["RichMediaExecute"] = 18] = "RichMediaExecute";
})(NpdfAction = exports.NpdfAction || (exports.NpdfAction = {}));
var Annotation = /** @class */ (function () {
    function Annotation(_instance) {
        this._instance = _instance;
    }
    Object.defineProperty(Annotation.prototype, "quadPoints", {
        get: function () {
            throw Error("unimplemented");
        },
        set: function (value) {
            throw Error("unimplemented");
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Annotation.prototype, "title", {
        get: function () {
            return this._instance.title;
        },
        set: function (value) {
            this._instance.title = value;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Annotation.prototype, "flag", {
        get: function () {
            return this._instance.flag;
        },
        set: function (value) {
            this._instance.flag = value;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Annotation.prototype, "color", {
        get: function () {
            return this._instance.color;
        },
        set: function (value) {
            var rgbErr = Error("RGB value must be an integer >= 0 || <= 256");
            if (value.length !== 3) {
                throw rgbErr;
            }
            value.forEach(function (e) {
                if (Number.isInteger(e) === false) {
                    throw rgbErr;
                }
                if (Number.isNaN(e)) {
                    throw rgbErr;
                }
            });
            this._instance.color = value;
        },
        enumerable: true,
        configurable: true
    });
    Annotation.prototype.hasAppearanceStream = function () {
        return this._instance.hasAppearanceStream();
    };
    Annotation.prototype.setBorderStyle = function (horizontalRadius, verticalRadius, width) {
        return this._instance.setBorderStyle(horizontalRadius, verticalRadius, width);
    };
    Annotation.prototype.hasDestination = function () {
        return this._instance.hasDestination();
    };
    Annotation.prototype.hasAction = function () {
        return this._instance.hasAction();
    };
    Annotation.prototype.getType = function () {
        return this._instance.getType();
    };
    Annotation.prototype.setFileAttachment = function () {
        throw new Error("Method not implemented.");
    };
    Annotation.prototype.hasFileAttachment = function () {
        return this._instance.hasFileAttachment();
    };
    return Annotation;
}());
exports.Annotation = Annotation;
