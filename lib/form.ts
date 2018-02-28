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
import {Document, __mod} from './document'
import {Obj} from "./object";

export class Form {
    private _instance:any

    get needAppearances() {return this._instance.needAppearances}
    set needAppearances(value:boolean) {this._instance.needAppearances = value}

    constructor(document:Document) {
        this._instance = new __mod.Form((document as any)._instance)
    }

    getObject():Obj {
        return new Obj(this._instance.getObject())
    }
}