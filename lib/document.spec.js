"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var fs_1 = require("fs");
var path_1 = require("path");
var test = require("tape");
var document_1 = require("../lib/document");
var encrypt_1 = require("../lib/encrypt");
var dictionary_1 = require("../lib/dictionary");
var filePath = path_1.join(__dirname, '../test.pdf'), outFile = './test.out.pdf', uPwd = 'secret', pwdDoc = path_1.join(__dirname, '../pwd.pdf');
test('Can load a pdf', function (t) {
    var doc;
    try {
        doc = new document_1.Document(filePath);
    }
    catch (e) {
        t.fail();
    }
    t.end();
});
test('document requires password', function (t) {
    var doc;
    try {
        doc = new document_1.Document(pwdDoc);
    }
    catch (e) {
        t.ok(e instanceof Error, 'should throw an error');
        t.assert(e.message.startsWith("Password"), 'password required error');
        t.end();
    }
});
test('throws JS error on file not found', function (t) {
    var doc;
    try {
        doc = new document_1.Document('/bad/path');
    }
    catch (e) {
        t.ok(e instanceof Error, 'should throw error');
        t.end();
    }
});
test('encryption: user password', function (t) {
    var doc = new document_1.Document(filePath), encryptionOption = {
        ownerPassword: 'secret',
        keyLength: 40,
        protection: ['Edit', 'FillAndSign'],
        algorithm: 'aesv3'
    }, secureDoc = path_1.join(__dirname, '../secure.pdf');
    doc.encrypt = new encrypt_1.Encrypt(null, encryptionOption);
    doc.write(secureDoc);
    var sDoc = new document_1.Document(secureDoc), trailer = sDoc.getTrailer();
    var trailerDict = new dictionary_1.Dictionary(trailer);
    if (!doc.encrypt) {
        t.fail();
    }
    var fillAndSign = doc.encrypt.isAllowed('FillAndSign'), edit = doc.encrypt.isAllowed('Edit');
    t.assert(trailerDict.hasKey("Encrypt") === true, 'trailer has an encryption object');
    t.end();
});
test('document get page count', function (t) {
    var doc = new document_1.Document(filePath);
    var pc = doc.getPageCount();
    t.ok(pc);
    t.assert(pc > 0);
    t.end();
});
test('document delete page', function (t) {
    var doc = new document_1.Document(filePath), pc = doc.getPageCount();
    doc.deletePage(pc - 1);
    doc.write(outFile);
    var docLessOne = new document_1.Document(outFile), docLessOnePageCount = docLessOne.getPageCount();
    t.assert(pc === docLessOnePageCount + 1);
    fs_1.unlinkSync(outFile);
    t.end();
});
test('document merger', function (t) {
    var doc = new document_1.Document(filePath), originalPC = doc.getPageCount();
    doc.mergeDocument(filePath);
    doc.write(outFile);
    var doc2 = new document_1.Document(outFile), doc2PC = doc2.getPageCount();
    t.assert(originalPC * 2 === doc2PC, "merged document");
    fs_1.unlinkSync(outFile);
    t.end();
});
test('get page', function (t) {
    var doc = new document_1.Document(filePath), page = doc.getPage(0); // pages are zero indexed
    t.ok(page);
    t.end();
});
test('Document set encrypt', function (t) {
    t.end();
});
