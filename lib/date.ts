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