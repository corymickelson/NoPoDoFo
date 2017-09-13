///<reference path="./npdf.d.ts">
/**
 * @type {npdf}
 */
const npdf = require('bindings')('npdf')

export class Transform {
    /**
     *
     * @param {string} file
     */
    constructor(file) {
        this.document = new npdf.Document()
        this.document.load(file)
    }

}
