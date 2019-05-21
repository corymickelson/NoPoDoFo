import {nopodofo} from "../index";
import {NDocument} from "./NDocument";

export class NDate implements nopodofo.Date {
    private self: nopodofo.Date

    /**
     * Convert node Date to Pdf Date string
     * @param {Date} d - The Date to convert
     * @returns {string} - d (Date) in pdf string format
     */
    static toPDFFormat(d: Date): string {
        const doubleDigit = (v: number) => {
            return `${v}`.length === 1 ? `0${v}` : `${v}`;
        }
        const offset = d.getTimezoneOffset() / 60;
        let dateString = d.toISOString().replace(/-|T|\.|:/g, '').substr(0, 14);
        return `${dateString}-${doubleDigit(offset)}'00'`;
    }

    constructor(private parent: NDocument, timestamp?: string) {
        if (timestamp) {
            this.self = new nopodofo.Date(timestamp)
        } else {
            this.self = new nopodofo.Date()
        }
    }

    /**
     * returns in the the following format: (D:YYYYMMDDHHmmSSOHH'mm')
     * @returns {string}
     */
    toString(): string {
        return this.self.toString()
    }
}
