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
import {
    IField,
    ICheckBox,
    IComboBox,
    IListBox,
    IPushButton,
    ITextField
} from './field'
import { IRect } from './rect';
import { IObj } from './object';
import { IAnnotation, NPDFAnnotation } from './annotation';

export interface IPage {
    rotation: number
    trimBox: IRect
    number: number
    width: number
    height: number
    contents: IObj
    resources: IObj

    /**
     * Get the field as a field of type T where T is one of: Checkbox, Pushbutton, Textfield, Combobox, or ListField
     * @param index - field index
     * @see ICheckBox
     * @see IListBox
     * @see ITextField
     * @see IComboBox
     * @see IPushButton
     */
    getField<T extends IField>(index: number): T
    /**
     * Get all fields on this page.
     * Note: to determine the type of field use the Field.type property
     */
    getFields(): IField[]
    fieldsCount(): number
    getFieldIndex(fieldName: string): number
    getMediaBox(): IRect
    getBleedBox(): IRect
    getArtBox(): IRect
    annotationsCount(): number
    createAnnotation(type: NPDFAnnotation, rect: IRect): IAnnotation
    getAnnotation(index: number): IAnnotation
    deleteAnnotation(index: number): void
}
