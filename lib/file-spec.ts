import {IBase} from "./base-document";
import {IObj} from "./object";

export interface IFileSpec {
    /**
     * Create a new FileSpec object
     * @param file
     * @param doc
     * @param embed
     */
    new(file: string, doc: IBase, embed?: boolean): IFileSpec

    /**
     * Copy an existing FileSpec from an IObj
     */
    new(obj: IObj): IFileSpec

    readonly name:string

    /**
     * @desc Can only get the file if it has been embedded into the document.
     * @returns {Buffer | undefined}
     */
    getContents(): Buffer | undefined
}