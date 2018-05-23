import {IBase} from "./base-document";

export interface IFileSpec {
    new(file: string, doc: IBase, embed?: boolean): IFileSpec
    readonly name:string

    /**
     * @desc Can only get the file if it has been embedded into the document.
     * @returns {Buffer | undefined}
     */
    getContents(): Buffer | undefined
}