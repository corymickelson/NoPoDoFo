import { Obj} from './object'
import {Document} from './document'
import * as test from 'tape'
import {join} from 'path';
import {existsSync, unlinkSync} from 'fs'
import {Dictionary} from './dictionary';
import {Arr} from "./arr";

const filePath = join(__dirname, '../test.pdf'),
    outFile = './test.out.pdf',
    outTxtFile = './test.out.txt'

const doc = new Document(filePath)



