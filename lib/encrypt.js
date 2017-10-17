"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var Encrypt = /** @class */ (function () {
    function Encrypt(_instance, option) {
        this._instance = _instance;
        this.option = option;
        this._complete = true;
        if (_instance === null && option instanceof Object) {
            this._complete = false;
        }
    }
    Encrypt.prototype.isAllowed = function (action) {
        if (!this._complete) { }
        return this._instance.isAllowed(action);
    };
    /**
     *
     * @param pwd - user password -OR- owner password
     * @returns {boolean} - true / false is password correct
     */
    Encrypt.prototype.authenticate = function (pwd) {
        if (!this._complete) { }
        return this._instance.authenticate(pwd);
    };
    return Encrypt;
}());
exports.Encrypt = Encrypt;
