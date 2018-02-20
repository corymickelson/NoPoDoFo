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
import { __mod } from "./document";

export type External = any
export class Data {
    private _instance: any
    get value(): string {
        return this._instance.value
    }
    constructor(value: string | External) {
        if (typeof value === 'string') {
            this._instance = new __mod.Data(value)
        } else {
            this._instance = value
        }
    }
    write(output: string): void {
        if (!output) {
            throw Error("outpust must be a valid path")
        }
        this._instance.write(output)
    }
}