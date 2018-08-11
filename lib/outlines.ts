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

import { IObj } from "./object";
import { IAction } from "./action";
import { NPDFColor } from "./painter";
import {IDestination} from "./destination";


/**
 * IOutline represents an PdfOutlineItem.
 * An Outline is created by another outline, or the outline root
 * @see IBase#getOutlines
 */
export interface IOutline {
    readonly prev: IOutline | null
    readonly next: IOutline | null
    readonly first: IOutline | null
    readonly last: IOutline | null
    destination: IDestination
    action: IAction
    title: string
    textFormat: number
    textColor: NPDFColor
    createChild(name: string, value: IDestination): IOutline
    createNext(name: string, value: IDestination | IAction): IOutline
    insertItem(item: IObj): void
    getParent(): IOutline
    erase(): void
}