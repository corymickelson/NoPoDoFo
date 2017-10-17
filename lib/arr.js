"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var object_1 = require("./object");
/**
 * @class Arr wraps PdfArray
 * The underlying pdf array is using vector<PdfObject>
 * The methods declared on Arr are methods PoDoFo has optimized for
 * Pdf array's.
 * If you would like to use Arr as an instance of a javascript array call ToArray()
 */
var Arr = /** @class */ (function () {
    function Arr(_instance) {
        this._instance = _instance;
    }
    Object.defineProperty(Arr.prototype, "length", {
        get: function () {
            return this._instance.length;
        },
        set: function (value) {
            throw Error("Can not set length");
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Converts PdfArray to javascript array.
     * Any modifications to javascript array will NOT be persisted to PdfArray.
     * Use for lookup only
     * @returns {Array<IObj>}
     */
    Arr.prototype.toArray = function () {
        var init = this._instance.toArray();
        this._array = init.map(function (i) { return new object_1.Obj(i); });
        var proxy = new Proxy(this, {
            get: function (target, prop) {
                if (prop instanceof Number || typeof prop === 'number') {
                    if (prop > -1 && prop < target._array.length)
                        return target._array[prop];
                }
                else
                    return target._array;
            },
            set: function (target, prop, value) {
                var success = false;
                target.push(value);
                target._array.push(value);
                return success;
            }
        });
        return proxy._array;
    };
    Arr.prototype.push = function (value) {
        this._instance.push(value);
    };
    /**
     * Search PdfArray for key
     * @param {string} key
     * @returns {boolean}
     */
    Arr.prototype.contains = function (key) {
        return this._instance.contains(key);
    };
    /**
     * Find index of key
     * @param {string} key
     * @returns {number}
     */
    Arr.prototype.indexOf = function (key) {
        return this._instance.indexOf(key);
    };
    /**
     * Writes the array to a file. Useful for debugging
     * @param {string} output
     */
    Arr.prototype.write = function (output) {
        this._instance.write(output);
    };
    /**
     * Retrieve the PdfObject at index
     * @param {number} index
     * @returns {IObj}
     */
    Arr.prototype.at = function (index) {
        var item = this._instance.getIndex(index);
        return new object_1.Obj(item);
    };
    return Arr;
}());
exports.Arr = Arr;
