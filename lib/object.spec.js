"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var document_1 = require("../lib/document");
var test = require("tape");
var path_1 = require("path");
var fs_1 = require("fs");
var dictionary_1 = require("../lib/dictionary");
var filePath = path_1.join(__dirname, '../test.pdf'), outFile = './test.out.pdf', outTxtFile = './test.out.txt';
var doc = new document_1.Document(filePath), obj = doc.getObjects()[0];
test('obj write sync', function (t) {
    obj.writeSync(outTxtFile);
    t.ok(fs_1.existsSync(outTxtFile), 'write to destination successful');
    fs_1.unlinkSync(outTxtFile);
    t.end();
});
test('obj write async', function (t) {
    try {
        t.plan(3);
        obj.write(outTxtFile, function (e, v) {
            t.assert(e === null, 'error is null');
            t.assert(v === outTxtFile, 'returns destination file path');
            t.ok(fs_1.existsSync(outTxtFile), 'write to destination successful');
            fs_1.unlinkSync(outTxtFile);
            t.end();
        });
    }
    catch (e) {
        console.log(e);
        t.fail();
    }
});
test('obj getOffset', function (t) {
    t.plan(2);
    var dict = new dictionary_1.Dictionary(obj), keys = dict.getKeys(), sync = obj.getOffsetSync(keys[0]);
    obj.getOffset(keys[0], function (e, v) {
        t.assert(e === null, 'error is null');
        t.assert(v === sync, 'sync and async return same value');
        t.end();
    });
});
