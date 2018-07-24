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