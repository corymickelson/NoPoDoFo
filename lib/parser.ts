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
import { Page } from "./page";
import { __mod, Document } from "./document";

/**
 * This class is a parser for content streams in PDF documents.
 * PoDoFo::PdfContentsTokenizer is currently a work in progress.
 */
export class ContentsTokenizer {
    private _instance:any
    constructor(page:Page, doc:Document) {
        this._instance = new __mod.ContentsTokenizer((page as any)._instance, (doc as any)._instance)
    }

    /**
     * Read all text content from the page.
     * @returns {Array<string>}
     */
    readAllContent(): Array<string> {
        return this._instance.readAll()
    }
}