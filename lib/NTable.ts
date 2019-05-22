import {nopodofo, NPDFPoint} from "../index"
import {NDocument} from "./NDocument"
import {NPainter} from "./NPainter"
import {NFont} from "./NFont"

export class NTable {
    get wordWrap(): boolean {
        return this.self.wordWrap
    }

    set wordWrap(value: boolean) {
        this.self.wordWrap = value
    }

    get tableWidth(): number {
        return this.self.tableWidth
    }

    set tableWidth(value: number) {
        this.self.tableWidth = value
    }

    get tableHeight(): number {
        return this.self.tableHeight
    }

    set tableHeight(value: number) {
        this.self.tableHeight = value
    }

    get foregroundColor(): nopodofo.Color {
        return this.self.foregroundColor
    }

    set foregroundColor(value: nopodofo.Color) {
        this.self.foregroundColor = value
    }

    get borderWidth(): number {
        return this.self.borderWidth
    }

    set borderWidth(value: number) {
        this.self.borderWidth = value
    }

    get backgroundColor(): nopodofo.Color {
        return this.self.backgroundColor
    }

    set backgroundColor(value: nopodofo.Color) {
        this.self.backgroundColor = value
    }

    get autoPageBreak(): boolean {
        return this.self.autoPageBreak
    }

    set autoPageBreak(value: boolean) {
        this.self.autoPageBreak = value
    }

    get alignment(): string {
        return this.self.alignment
    }

    set alignment(value: string) {
        this.self.alignment = value
    }

    constructor(private parent: NDocument, private self: nopodofo.SimpleTable) {
    }

    borderEnable(v: boolean): void {
        this.self.borderEnable(v)
    }

    columnCount(): number {
        return this.self.columnCount()
    }

    draw(point: NPDFPoint, painter: NPainter): void {
        this.self.draw(point, (painter as any).self)
    }

    enableBackground(v: boolean): void {
        this.self.enableBackground(v)
    }

    getBorderColor(col: number, row: number): nopodofo.Color {
        return this.self.getBorderColor(col, row)
    }

    getFont(col: number, row: number): NFont {
        const n = this.self.getFont(col, row)
        return new NFont(this.parent, n)
    }

    getImage(col: number, row: number): Buffer {
        return this.self.getImage(col, row)
    }

    getText(col: number, row: number): string {
        return this.self.getText(col, row)
    }

    getVerticalAlignment(col: number, row: number): string {
        return this.self.getVerticalAlignment(col, row)
    }

    hasBackgroundColor(col: number, row: number): boolean {
        return this.self.hasBackgroundColor(col, row)
    }

    hasBorders(): boolean {
        return this.self.hasBorders()
    }

    hasImage(col: number, row: number): boolean {
        return this.self.hasImage(col, row)
    }

    rowCount(): number {
        return this.self.rowCount()
    }

    setColumnWidth(n: number): void {
        return this.self.setColumnWidth(n)
    }

    setColumnWidths(n: number[]): void {
        this.self.setColumnWidths(n)
    }

    setFont(font: NFont): void {
        this.self.setFont((font as any).self)
    }

    setRowHeight(n: number): void {
        this.self.setRowHeight(n)
    }

    setRowHeights(n: number[]): void {
        this.self.setRowHeights(n)
    }

    setText(col: number, row: number, text: string): void {
        this.self.setText(col, row, text)
    }

}
