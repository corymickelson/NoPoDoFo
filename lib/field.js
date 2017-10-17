"use strict";
// const mod = require('bindings')('npdf')
Object.defineProperty(exports, "__esModule", { value: true });
var document_1 = require("./document");
var Field = /** @class */ (function () {
    function Field(page, fieldIndex) {
        this._instance = new document_1.__mod.Field(page._instance, fieldIndex);
    }
    Field.prototype.getType = function () {
        return this._instance.getType();
    };
    Field.prototype.getFieldName = function () {
        return this._instance.getFieldName();
    };
    Field.prototype.getAlternateName = function () {
        return this._instance.getAlternateName();
    };
    Field.prototype.getMappingName = function () {
        return this._instance.getMappingName();
    };
    Field.prototype.isRequired = function () {
        return this._instance.isRequired();
    };
    Field.prototype.setRequired = function (required) {
        this._instance.setRequired(required);
    };
    Field.prototype.setAlternateName = function (name) {
        this._instance.setAlternateName(name);
    };
    Field.prototype.setMappingName = function (name) {
        this._instance.setMappingName(name);
    };
    return Field;
}());
exports.Field = Field;
var TextField = /** @class */ (function () {
    function TextField(field) {
        if (field.getType() !== 'TextField') {
            throw Error('field parameter must be a field of type TextField');
        }
        this._instance = new document_1.__mod.TextField(field._instance);
    }
    Object.defineProperty(TextField.prototype, "text", {
        get: function () {
            return this._instance.text;
        },
        set: function (value) {
            this._instance.text = value;
        },
        enumerable: true,
        configurable: true
    });
    return TextField;
}());
exports.TextField = TextField;
var CheckBox = /** @class */ (function () {
    function CheckBox(field) {
        if (field.getType() !== 'CheckBox') {
            throw Error('must be of type CheckBox');
        }
        this._instance = new document_1.__mod.CheckBox(field._instance);
    }
    Object.defineProperty(CheckBox.prototype, "checked", {
        get: function () {
            return this._instance.checked;
        },
        set: function (value) {
            this._instance.checked = value;
        },
        enumerable: true,
        configurable: true
    });
    return CheckBox;
}());
exports.CheckBox = CheckBox;
