/**
 * The PdfDate object is a specific format for Date objects in Pdf
 */
export interface IDate {
    /**
     * Create a new PdfDate object with a timestamp of now
     * @returns {IDate}
     */
    new(): IDate

    /**
     * Create an existing timestamp from a string.
     * The string must be formatted in the following:
     * (D:YYYYMMDDHHmmSSOHH'mm')
     * @param {string} timestamp
     * @returns {IDate}
     */
    new(timestamp: string): IDate

    /**
     * returns in the the following format: (D:YYYYMMDDHHmmSSOHH'mm')
     * @returns {string}
     */
    toString(): string
}

function doubleDigit(v: number): string {
    return `${v}`.length === 1 ? `0${v}` : `${v}`
}

/**
 * Convert node Date to Pdf Date string
 * @param {Date} d - The Date to convert
 * @returns {string} - d (Date) in pdf string format
 */
export function toPDFFormat(d: Date) {
    const offset = d.getTimezoneOffset() / 60
    let dateString = d.toISOString().replace(/-|T|\.|:/g, '').substr(0, 14)
    return `${dateString}-${doubleDigit(offset)}'00'`
}