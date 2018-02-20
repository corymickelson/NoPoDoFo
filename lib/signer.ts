/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
 * Authors: Cory Mickelson, et al.
 * 
 * NoPoDoFo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NoPoDoFo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import { __mod, Document } from './document'
import { SignatureField } from "./field";
import { existsSync } from "fs";
import { Data } from './data';
import { Annotation } from './annotation';

/**
 * The Signer class binds PoDoFo::PdfSignOutputDevice
 */
export class Signer {
    private _instance: any
    constructor(doc: Document, output?: string) {
        if (output) {
            this._instance = new __mod.Signer((doc as any)._instance, output)

        } else
            this._instance = new __mod.Signer((doc as any)._instance)
    }

    signSync(signature: string , output?:string): Buffer {
        return this._instance.signSync(signature, output)
    }

    sign(signature:string, output:string, cb:(e:Error, d:Buffer) => void): void {
        this._instance.sign(cb, signature, output)
    }

    setField(field:SignatureField) {
        if(!(field as any)._instance) {
            throw Error("field undefined")
        }
        this._instance.setField((field as any)._instance)
    }

    getField(): SignatureField {
        return new SignatureField(this._instance.getField())
    }
}

export function signature(certfile: string, pkeyfile: string, password: string = ''): string {
    if (!existsSync(certfile) || !existsSync(pkeyfile)) {
        throw Error("One or both files not found")
    }
    return __mod.signature(certfile, pkeyfile, password)
}