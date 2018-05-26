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

import {IBase} from './base-document'

export enum NPDFImageFormat {
    data,
    png,
    tiff,
    jpeg,
}

export interface IImage {
    /**
     *
     * @param {IBase} doc
     * @param {string | Buffer} source
     * @param {NPDFImageFormat} [format] - defaults to data
     * @returns {IImage}
     */
    new(doc: IBase, source: string | Buffer, format?:NPDFImageFormat): IImage
    readonly width: number
    readonly height: number

    setInterpolate(v: boolean): void
}

