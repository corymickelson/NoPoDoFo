"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var fs_1 = require("fs");
var path_1 = require("path");
var test = require("tape");
var document_1 = require("../lib/document");
var object_1 = require("../lib/object");
var field_1 = require("../lib/field");
var painter_1 = require("../lib/painter");
var image_1 = require("../lib/image");
var filePath = path_1.join(__dirname, '../test.pdf'), outFile = './test.out.pdf', doc = new document_1.Document(filePath), page = doc.getPage(0);
function pageRotation() {
    test('set page rotation', function (t) {
        var originalRotation = page.rotation, newRotation = originalRotation + 90;
        page.rotation = newRotation;
        doc.write(outFile);
        var testDoc = new document_1.Document(outFile), testPage = testDoc.getPage(0);
        t.assert(testPage.rotation === newRotation, 'Page rotation updated');
        fs_1.unlinkSync(outFile);
        t.end();
    });
}
function pageProperties() {
    test('page instance properties', function (t) {
        t.assert(page.number === 1, 'page number valid');
        t.assert(page.width > 0, 'page width valid');
        t.assert(page.height > 0, 'page height valid');
        t.end();
    });
}
function pageTrimBox() {
    test('get trimbox', function (t) {
        var trimBox = page.trimBox;
        t.assert(trimBox.height > 0, 'trimbox height is not null');
        t.assert(trimBox.width > 0, 'trimbox width not null');
        t.assert(trimBox.left === 0, 'trimbox left');
        t.assert(trimBox.bottom === 0, 'trimbox bottom');
        t.end();
    });
}
function pageFields() {
    test('get number of fields', function (t) {
        var n = page.getNumFields();
        t.assert(typeof n === 'number');
        // t.assert(page.getNumFields() === 22, 'page get number of fields counts all fields')
        t.end();
    });
}
function pageFieldInfoArray() {
    test('get fields info', function (t) {
        try {
            var fields = page.getFieldsInfo();
            // t.assert(fields.length === 22, 'page get field info get\'s info for each field')
            t.assert(fields instanceof Array);
            t.ok(fields);
            t.assert(fields.length > 0);
            t.end();
        }
        catch (e) {
            console.log(e);
            t.fail();
        }
    });
}
function pageGetField() {
    test('get field', function (t) {
        var field = page.getField(0);
        t.assert(field instanceof field_1.Field, 'is an instance of Field');
        t.end();
    });
}
function pageGetFields() {
    test('get fields', function (t) {
        var fields = page.getFields();
        t.assert(Array.isArray(fields), 'returns an array');
        t.assert(fields.length === page.getNumFields(), 'is not an empty array');
        t.assert(fields[0] instanceof field_1.Field, 'contains instance(s) of Field');
        t.end();
    });
}
function pageGetAnnotsCount() {
    test('page number of annotations', function (t) {
        t.assert(page.getNumAnnots() === 22, 'get\'s all annotations on the page');
        t.end();
    });
}
function pageGetAnnot() {
    test('page get annotation', function (t) {
        var annot = page.getAnnotation(0);
        t.ok(annot);
        t.assert(annot.getType() === 'Widget');
        t.end();
    });
}
function pageContents() {
    test('page contents', function (t) {
        var contents = page.getContents(false);
        t.assert(contents instanceof object_1.Obj, 'is an instance of Obj');
        t.assert(contents.length > 0, 'content is not null or empty');
        t.end();
    });
}
function pageResources() {
    test('page resources', function (t) {
        var resources = page.getResources();
        t.assert(resources instanceof object_1.Obj, 'is instance ob Obj');
        t.assert(resources.length > 0, 'is not null or empty');
        t.end();
    });
}
function pageAddImg() {
    test('add image', function (t) {
        var painter = new painter_1.Painter(), img = new image_1.Image(doc, '/home/red/test.jpg');
        painter.page = page;
        painter.drawImage(img, 0, page.height - img.getHeight());
        painter.finishPage();
        if (fs_1.existsSync('./img.out.pdf'))
            fs_1.unlinkSync('./img.out.pdf');
        doc.write('./img.out.pdf');
        // todo: finish extracting img, requires stream wrapper completion
        var objs = doc.getObjects();
        for (var i = 0; i < objs.length; i++) {
            var o = objs[i];
            if (o.type === 'Dictionary') {
                var objDict = o.asDictionary(), objType = objDict.hasKey('Type') ? objDict.getKey('Type') : null, objSubType = objDict.hasKey('SubType') ? objDict.getKey('SubType') : null;
                if ((objType && objType.type === 'Name') ||
                    (objSubType && objSubType.type === 'Name')) {
                    if ((objType && objType.asName() === 'XObject') || (objSubType && objSubType.asName() === 'Image')) {
                        var imgObj = o.asDictionary().hasKey('Filter') ? o.asDictionary().getKey('Filter') : null;
                        if (imgObj && imgObj.type === 'Array') {
                            if (imgObj.asArray().length === 1) {
                                if (imgObj.asArray().at(0).type === 'Name') {
                                    if (imgObj.asArray().at(0).asName() === 'DCTDecode') {
                                        extractImg(o, true);
                                        return;
                                    }
                                }
                            }
                        }
                        else if (imgObj && imgObj.type === 'Name' && imgObj.asName() === 'DCTDecode') {
                            extractImg(o, true);
                            return;
                        }
                        else {
                            extractImg(o, false);
                            return;
                        }
                    }
                }
            }
        }
        function extractImg(obj, jpg) {
            var ext = jpg ? '.jpg' : '.ppm';
            t.end();
        }
        t.fail('should have been able to find the DCTDecode object');
    });
}
function runTest(test) {
    setImmediate(function () {
        global.gc();
        test();
    });
}
function runAll() {
    [
        pageRotation,
        pageProperties,
        pageTrimBox,
        pageFields,
        pageFieldInfoArray,
        pageGetField,
        pageGetFields,
        pageGetAnnotsCount,
        pageGetAnnot,
        pageContents,
        pageResources,
        pageAddImg
    ].map(function (i) { return runTest(i); });
}
exports.runAll = runAll;
