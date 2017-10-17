"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var document_1 = require("./document");
var object_1 = require("./object");
var Dictionary = /** @class */ (function () {
    function Dictionary(obj) {
        if (obj === null) {
            throw Error("Can not instantiate Dictionary without valid NPdf Object");
        }
        this._instance = new document_1.__mod.Dictionary(obj._instance);
    }
    Dictionary.prototype.getKey = function (key) {
        var instance = this._instance.getKey(key);
        return new object_1.Obj(instance);
    };
    Dictionary.prototype.getKeys = function () {
        return this._instance.getKeys();
    };
    Dictionary.prototype.getKeyAs = function (type, key) {
        return this._instance.getKeyAs(type, key);
    };
    Dictionary.prototype.hasKey = function (key) {
        return this._instance.hasKey(key);
    };
    Dictionary.prototype.addKey = function (key, value) {
        this._instance.addKey(key, value._instance);
    };
    Dictionary.prototype.removeKey = function (key) {
        return this._instance.removeKey(key);
    };
    Dictionary.prototype.clear = function () {
        this._instance.clear();
    };
    Dictionary.prototype.write = function (output, cb) {
        this._instance.write(output, cb);
    };
    Dictionary.prototype.writeSync = function (output) {
        this._instance.writeSync(output);
    };
    /**
     * WARNING!!! Use this only for key lookup. Modifications made to this object will not persist
     * back to the underlying pdf object
     * @returns Object
     */
    Dictionary.prototype.toObject = function () {
        var init = this._instance.toObject();
        for (var prop in init) {
            init[prop] = new object_1.Obj(init[prop]);
        }
        return init;
    };
    return Dictionary;
}());
exports.Dictionary = Dictionary;
