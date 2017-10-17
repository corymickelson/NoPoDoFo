"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var object_1 = require("./object");
var page_1 = require("./page");
var encrypt_1 = require("./encrypt");
exports.__mod = require('../build/Release/npdf.node');
/**
 * @class Document
 * @example
 * import {Document} from 'npdf'
 * const doc = new Document('/path/to/file')
 * const pageCount = doc.getPageCount()
 * const page = doc.getPage(0) // pages are zero indexed
 * const mergeDocs = doc.mergeDocument('/path/to/other/doc')
 * doc.setPassword('secret')
 * doc.write('/file/destination')
 * const fileBuffer = fs.readFile('/file/destination', (e,d) => e ? e : d)
 * // do something with the file or the file buffer
 */
var Document = /** @class */ (function () {
    /**
     * @constructor
     * @param {string} [file] - pdf file path (optional)
     * @param update
     * @returns void
     */
    function Document(file, update) {
        if (update === void 0) { update = false; }
        this._loaded = false;
        this._instance = new exports.__mod.Document();
        if (file) {
            this._instance.load(file, update);
            this._pageCount = this._instance.getPageCount();
            this._loaded = true;
        }
    }
    Object.defineProperty(Document.prototype, "pageCount", {
        get: function () {
            return this._pageCount;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Document.prototype, "password", {
        get: function () {
            throw Error("Can not get password from loaded document");
        },
        set: function (value) {
            this._password = value;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Document.prototype, "encrypt", {
        get: function () {
            var instance = this._instance.encrypt;
            return new encrypt_1.Encrypt(instance);
        },
        set: function (instance) {
            if (instance.option)
                this._instance.encrypt = instance.option;
            else {
                throw Error("Set document encrypt with an instance of Encrypt with the optional EncryptInitOption defined at construction");
            }
        },
        enumerable: true,
        configurable: true
    });
    /**
     * @description load pdf file
     * @param {string} file - pdf file path
     * @param update
     */
    Document.prototype.load = function (file, update) {
        if (update === void 0) { update = false; }
        this._instance.load(file, update);
        this._loaded = true;
    };
    Document.prototype.getPageCount = function () {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        return this._instance.getPageCount();
    };
    Document.prototype.getPage = function (pageN) {
        if (pageN > this.pageCount || pageN < 0) {
            throw new RangeError("pageN out of range");
        }
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        var page = this._instance.getPage(pageN);
        return new page_1.Page(page);
    };
    Document.prototype.getObjects = function () {
        var objects = this._instance.getObjects();
        return objects.map(function (value) {
            return new object_1.Obj(value);
        });
    };
    /**
     * @description Append doc to the end of the loaded doc
     * @param {string} doc - pdf file path
     * @param password
     */
    Document.prototype.mergeDocument = function (doc, password) {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        password !== null ? this._instance.mergeDocument(doc, password) : this._instance.mergeDocument(doc);
    };
    Document.prototype.deletePage = function (pageIndex) {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        if (this.pageCount < pageIndex || pageIndex < 0) {
            throw new RangeError('page index out of range');
        }
        this._instance.deletePage(pageIndex);
    };
    Document.prototype.getVersion = function () {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        return this._instance.getVersion();
    };
    Document.prototype.isLinearized = function () {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        return this._instance.isLinearized();
    };
    Document.prototype.write = function (file) {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method');
        }
        this._instance.write(file);
    };
    Document.prototype.createEncrypt = function (option) {
        var encryptInstance = this._instance.createEncrypt(option);
        return new encrypt_1.Encrypt(encryptInstance);
    };
    Document.prototype.getTrailer = function () {
        var objInit = this._instance.getTrailer();
        return new object_1.Obj(objInit);
    };
    Document.prototype.freeObjMem = function (target) {
        this._instance.freeObjMem(target._instance);
    };
    return Document;
}());
exports.Document = Document;
