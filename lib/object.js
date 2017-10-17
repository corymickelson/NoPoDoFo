"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var reference_1 = require("./reference");
var dictionary_1 = require("./dictionary");
var arr_1 = require("./arr");
/**
 * @desc This class represents a PDF indirect Object in memory
 *      It is possible to manipulate the stream which can be appended to the object(if the object is of underlying type dictionary)
 *      A PdfObject is uniquely identified by an object number and generation number
 *      The object can easily be written to a file using the write function
 *
 * @todo New instance object not yet supported. Objects can only be instantiated from an existing object
 */
var Obj = /** @class */ (function () {
    function Obj(instance) {
        this._instance = instance;
    }
    Object.defineProperty(Obj.prototype, "reference", {
        get: function () {
            return this._instance.reference;
        },
        set: function (value) {
            throw Error("Reference may not be set manually");
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Obj.prototype, "owner", {
        get: function () {
            return this._instance.owner;
        },
        set: function (value) {
            this._instance.owner = value;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Obj.prototype, "length", {
        get: function () {
            return this._instance.length;
        },
        set: function (value) {
            throw Error("Object Length may not be set manually");
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Obj.prototype, "stream", {
        get: function () {
            return this._instance.stream;
        },
        set: function (value) {
            throw Error("Object stream may not be set manually");
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Obj.prototype, "type", {
        get: function () {
            return this._instance.type;
        },
        set: function (value) {
            throw Error('Can not set type');
        },
        enumerable: true,
        configurable: true
    });
    Obj.prototype.hasStream = function () {
        return this._instance.hasStream();
    };
    /**
     * @desc Calculates the byte offset of key from the start of the object if the object was written to disk at the moment of calling the function
     *      This function is very calculation intensive!
     *
     * @param {string} key object dictionary key
     * @returns {number} - byte offset
     */
    Obj.prototype.getOffsetSync = function (key) {
        return this._instance.getOffsetSync(key);
    };
    Obj.prototype.getOffset = function (key, cb) {
        this._instance.getOffset(key, cb);
    };
    /**
     * @desc Write the complete object to a file
     */
    Obj.prototype.writeSync = function (output) {
        try {
            this._instance.writeSync(output);
        }
        catch (error) {
            throw error;
        }
    };
    Obj.prototype.write = function (output, cb) {
        this._instance.write(output, cb);
    };
    /**
     * @desc This function compresses any currently set stream using the FlateDecode algorithm.
     *  JPEG compressed streams will not be compressed again using this function.
     *  Entries to the filter dictionary will be added if necessary.
     */
    Obj.prototype.flateCompressStream = function () {
        this._instance.flateCompressStream();
    };
    /**
     * @desc Dynamically load this object and any associated stream from a PDF file
     */
    Obj.prototype.delayedStreamLoad = function () {
        this._instance.delayedStreamLoad();
    };
    Obj.prototype.asBool = function () {
        return this._instance.getBool();
    };
    Obj.prototype.asString = function () {
        return this._instance.getString();
    };
    Obj.prototype.asName = function () {
        return this._instance.getName();
    };
    Obj.prototype.asReal = function () {
        return this._instance.getReal();
    };
    Obj.prototype.asNumber = function () {
        return this._instance.getNumber();
    };
    Obj.prototype.asArray = function () {
        var i = this._instance.getArray();
        return new arr_1.Arr(i);
    };
    Obj.prototype.asDictionary = function () {
        return new dictionary_1.Dictionary(this);
    };
    Obj.prototype.asReference = function () {
        var i = this._instance.getReference();
        return new reference_1.Ref(i);
    };
    Obj.prototype.asBuffer = function () {
        throw Error("unimplmented");
    };
    return Obj;
}());
exports.Obj = Obj;
