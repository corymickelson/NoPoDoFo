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
import { IBase } from "./base-document";
import { IEncrypt } from "./encrypt";

export enum NPDFVersion {
    Pdf11,
    Pdf12,
    Pdf13,
    Pdf14,
    Pdf15,
    Pdf16,
    Pdf17,
}

export enum NPDFWriteMode {
    Default = 0x01,
    Compact = 0x02
}
export interface IStreamDocument extends IBase {
    /**
     *
     * @param {string} [file]
     * @param {{version: NPDFVersion, writer: NPDFWriteMode, encrypt: IEncrypt}} [opts] -
     *      defaults to {pdf1.7, writeMode_default, null}
     * @returns {IStreamDocument}
     */
    new(file?: string, opts?: { version: NPDFVersion, writer: NPDFWriteMode, encrypt?: IEncrypt }): IStreamDocument
    /**
     * Closing a stream document will prevent any further writes to the document.
     * If the object was instantiated with a file path, close will write to this file.
     * If "new" was called without any args, the document is written to a nodejs buffer, close
     *   will return the buffer.
     */
    close(): string | Buffer
}
