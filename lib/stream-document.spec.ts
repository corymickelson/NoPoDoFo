import {NPDFVersion, NPDFWriteMode, StreamDocument} from './stream-document'
import * as tap from 'tape'
import { join } from 'path';
import { Test } from 'tape';

const filePath = join(__dirname, '../test-documents/test.pdf'),
    pwdDoc = join(__dirname, '../test-documents/pwd.pdf')

const end = (...tests: Test[]) => tests.forEach(t => t.end())

tap('Stream Document Api', sub => {
    sub.test('Initialization', standard => {
        const tmpFile = join(__dirname, '../test-documents/streamed.pdf')
        const doc = new StreamDocument(tmpFile, NPDFVersion.Pdf17, NPDFWriteMode.Default)
    })
})
