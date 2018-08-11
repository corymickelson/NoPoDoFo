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
import {IBase} from "./base-document";
import {IObj} from "./object";

export interface IFileSpec {
    /**
     * Create a new FileSpec object
     * @param file
     * @param doc
     * @param embed
     */
    new(file: string, doc: IBase, embed?: boolean): IFileSpec

    /**
     * Copy an existing FileSpec from an IObj
     */
    new(obj: IObj): IFileSpec

    readonly name:string

    /**
     * @desc Can only get the file if it has been embedded into the document.
     * @returns {Buffer | undefined}
     */
    getContents(): Buffer | undefined
}