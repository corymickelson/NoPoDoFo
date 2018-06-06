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
import { IDocument} from "./document";
import {npdf, NPDFInternal} from './'
import {IFont, NPDFAlignment, NPDFColor, NPDFPoint, NPDFVerticalAlignment, IPainter} from "./painter";
import {IPage} from './page'

export class Cell {
    /**
     * Get the font on this cell or null for the default font
     * @returns {Font}
     */
    get font(): IFont {
        return this._table.getFont(this._col, this._row)
    }

    get text(): string {
        return this._table.getText(this._col, this._row)
    }

    set text(v: string) {
        this._table.text = {col: this._col, row: this._row, text: v}
    }

    get foregroundColor(): NPDFColor {
        return this._table.getForegroundColor(this._col, this._row)
    }

    get backgroundColor(): NPDFColor {
        return this._table.getBackgroundColor(this._col, this._row)
    }

    get alignment(): NPDFAlignment {
        return this._table.getAlignment(this._col, this._row)
    }

    get wordWrap(): boolean {
        return this._table.getWordWrap(this._col, this._row)
    }

    get verticalAlignment(): NPDFVerticalAlignment {
        return this._table.getVerticalAlignment(this._col, this._row)
    }

    constructor(private _table: NPDFInternal, private _col: number, private _row: number) {
    }

    getImage(): Buffer {
        return this._table.getImage(this._col, this._row)
    }

    hasImage(): boolean {
        return this._table.hasImage()
    }

    hasBackgroundColor(): boolean {
        return this._table.hasBackgroundColor()
    }

}


export class Table {
    private _instance: any
    private _position: NPDFPoint = {x:0, y:0}
    private _page: IPage|null = null

    // model accessors
    /**
     * Sets the font for all contents of the table
     * @param {Font} v
     */
    set font(v: IFont) {
        if (v instanceof (npdf.Font as any) === false) {
            throw Error("must be instance of Font")
        }
        this._instance.font = (v as any)._instance
    }

    get page():IPage|null {
        return this._page
    }

    set page(value:IPage|null) {
        this._page = value
    }

    get borderWidth(): number {
        return this._instance.borderWidth
    }

    set borderWidth(v: number) {
        this._instance.borderWidth = v
    }

    set foregroundColor(v: NPDFColor) {
        this._instance.foregroundColor = v
    }

    set alignment(v: NPDFAlignment) {
        this._instance.alignment = v
    }

    set wordWrap(v: boolean) {
        this._instance.wordWrap = v
    }

    // table accessors
    get tableWidth(): number {
        if (!this._position) {
            throw Error('Table position must be set prior to calling this method')
        }
        if (!this._page) {
            throw Error('Table _page must be set prior to calling this method')
        }
        return this._instance.getTableWidth(this._position.x, this._position.y, (this._page as any)._instance)
    }

    set tableWidth(v: number) {
        this._instance.tableWidth = v
    }

    get tableHeight(): number {
        if (!this._position) {
            throw Error('Table position must be set prior to calling this method')
        }
        if (!this._page) {
            throw Error('Table _page must be set prior to calling this method')
        }
        return this._instance.getTableHeight(this._position.x, this._position.y, (this._page as any)._instance)
    }

    set tableHeight(v: number) {
        this._instance.tableHeight = v
    }

    /**
     * true if a new page is created automatically if more space is required to draw the table.
     * @returns {boolean}
     */
    get autoPageBreak(): boolean {
        return this._instance.autoPageBreak
    }

    /**
     * Automatically create a new page and continue drawing the table on the new page, if there is not enough space on the current page.
     * The newly created page will be set as the current page on the painter used to draw and will be created using the same size as the old page.
     * @param {boolean} v
     */
    set autoPageBreak(v: boolean) {
        this._instance.autoPageBreak = v
    }

    constructor(doc: IDocument, cols: number, rows: number) {
        this._instance = new npdf.SimpleTable((doc as any)._instance, cols, rows)
    }

    // model methods
    enableBorder(v: boolean): void {
        this._instance.borderEnable(v)
    }

    hasBorders(): boolean {
        return this._instance.hasBorders()
    }

    enableBackground(v: boolean): void {
        this._instance.enableBackground = v
    }

    // table methods
    draw(point: NPDFPoint, painter: IPainter): void {
        if (painter instanceof (npdf.Painter as any) === false) {
            throw Error('painter must be an instance of NoPoDoFo Painter')
        }
        this._instance.draw(point, (painter as any)._instance)
    }

    columnCount(): number {
        return this._instance.columnCount()
    }

    rowCount(): number {
        return this._instance.rowCount()
    }

    columnWidth(v: number): void {
        this._instance.columnWidth(v)
    }

    columnWidths(v: Array<number>): void {
        this._instance.columnWidths(v)
    }

    rowHeight(v: number): void {
        this._instance.rowHeight(v)
    }

    rowHeights(v: Array<number>): void {
        this._instance.rowHeights(v)
    }
}
