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
import { Page } from './page'
import { Document, __mod } from './document'

export class Rect {
    private _instance: any

    constructor(position?: Page | Array<number>) {
        if (Array.isArray(position)) {
            this._instance = new __mod.Rect(
                (position as number[])[0],
                (position as number[])[1],
                (position as number[])[2],
                (position as number[])[3])
        }
        else if(position instanceof Page) {
            this._instance = new __mod.Rect(position)
        }
        else {
            this._instance = new __mod.Rect()
        }
    }
    get bottom(): number {
        return this._instance.bottom
    }

    set bottom(value: number) {
        this._instance.bottom = value
    }

    get left(): number {
        return this._instance.left
    }

    set left(value: number) {
        this._instance.left = value
    }

    get width(): number {
        return this._instance.width
    }

    set width(value: number) {
        this._instance.width = value
    }

    get height(): number {
        return this._instance.height
    }

    set height(value: number) {
        this._instance.height = value
    }

    intersect(rect: Rect): void {
        this._instance.intersect(rect)
    }
}