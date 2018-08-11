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
import { IDocument } from './document'
import { ISignatureField } from "./field";
import {Callback} from "./index";


export interface ISigner {
    signatureField: ISignatureField

    /**
     * Creates a new instance of Signer. Signer is the only way to
     * @param {IDocument} doc
     * @param {string} [output] - optional if provided writes to this path, otherwise
     *      a buffer is returned
     * @returns {ISigner}
     */
    new(doc: IDocument, output?: string): ISigner

    /**
     * Loads the Certificate and Private Key and stores the values into the Signer instance.
     * Values are not retrievable but are stored for use in Signer.sign
     * After both cert and pkey are loaded, a minimal signature size is calculated and returned to
     * the caller. To complete the signing process this minimum signature size value needs to be
     * provided to the write method.
     *
     * @param {string} certificate
     * @param {string} pkey
     * @param {string | Callback} p - either the pkey password or callback
     * @param {Callback} [cb] - callback
     * @returns {Number} - minimum signature size
     */
    loadCertificateAndKey(certificate:string, pkey: string, p:string|Callback<Number>, cb?:Callback<Number>): number

    /**
     * Signs the document output to disk or a node buffer
     * The loadCertificateAndKey must be loaded prior to calling write
     * @see loadCertificateAndKey
     * @param {Number} minSignatureSize
     * @param {Callback} cb
     */
    write(minSignatureSize: Number, cb: Callback<Buffer|string>): void
}

