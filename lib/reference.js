"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var Ref = /** @class */ (function () {
    function Ref(instance) {
        this._instance = instance;
    }
    Object.defineProperty(Ref.prototype, "object", {
        get: function () {
            return this._instance.object;
        },
        set: function (value) {
            this._instance.object = value;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Ref.prototype, "generation", {
        get: function () {
            return this._instance.generation;
        },
        enumerable: true,
        configurable: true
    });
    Ref.prototype.set = function (value) {
        this._instance.generation = value;
    };
    Ref.prototype.write = function (output) {
        if (output === null)
            throw Error('output must be a valid output file path');
        this._instance.write(output);
    };
    Ref.prototype.isIndirect = function () {
        return this._instance.isIndirect();
    };
    Ref.prototype.toString = function () {
        return this._instance.toString();
    };
    return Ref;
}());
exports.Ref = Ref;
