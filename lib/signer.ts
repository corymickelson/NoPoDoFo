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
import {Callback, __mod, IDocument} from './document'
import {ISignatureField} from "./field";
import {access} from "fs";
import {F_OK, R_OK} from 'constants'

export interface ISigner {
    new(doc: IDocument, output?: string): ISigner
    setField(field: ISignatureField): void
    sign(signature: string, cb: Callback): void
}

export function signature(certfile: string, pkeyfile: string, password: string = ''): Promise<string> {
    return new Promise((resolve, reject) => {
    const check = (file:string) => {
        return new Promise((resolve, reject) => {
            access(file, F_OK | R_OK, err => {
                err ? reject(err) : resolve()
            })
        })
    }
    Promise.all([check(certfile), check(pkeyfile)])
        .then(() => {
            resolve(__mod.signature(certfile, pkeyfile, password))
        })
        .catch(err => {
           reject(err)
        })

    })

}