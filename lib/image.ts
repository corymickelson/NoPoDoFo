const mod = require('../src/npdf')

import {IDocument} from './document'

export interface IImage {
    getWidth(): number
    getHeight(): number
    setFile(file: string): void
    setData(data: Buffer): void
    isLoaded(): boolean
}
export class Image implements IImage {
    private _instance: any
    constructor(private _doc: IDocument, data?: string | Buffer) {
        if (data) {
            this._instance = new mod.Image(_doc, data)
        }
        else {
            this._instance = new mod.Image(_doc)
        }
    }
    getWidth(): number {
        return this._instance.getWidth()
    }
    getHeight(): number {
        return this._instance.getHeight()
    }
    setFile(file: string): void {
        this._instance.setFile(file)
    }
    /**
     * @description Embeds data as an image on the document. To use this image pass to Painter.drawImage.
     *      New instances will reuse embedded img object if passed the same image reference.
     * @throws {TypeError} - throw type error on invalid arguments.
     * @param data img data
     * @returns void
     */
    setData(data: Buffer | string): void {
        if(Buffer.isBuffer(data)) this._instance.setData(data.toString('utf8'))
        else if(typeof data === 'string' || (data as any) instanceof String)
            this._instance.setData(data)
        else throw new TypeError("Image.setData requires a single argument of type string | Buffer")
    }
    isLoaded(): boolean {
        return this._instance.isLoaded()
    }

}