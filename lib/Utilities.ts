/**
 * @desc Converts a unit of measurement(mm or in) to PDF units
 * @param value
 * @param unit
 */
export function convertToPdfUnits(value: number, unit: 'inch' | 'mm') {
    if (unit === 'mm') {
        return value * 72.0 / 25.4
    } else if (unit === 'inch') {
        return value * 72.0
    } else {
        throw Error('Conversions are only in milimeters or inches')
    }
}
