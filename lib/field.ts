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
import {IDocument} from './document'
import {IObj} from "./object";
import {IAnnotation} from "./annotation";
import {IListItem} from "../dist/field";
import {NPDFColor, NPDFHighlightingMode} from "./painter";
import {IAction, NPDFMouseEvent, NPDFPageEvent} from "./action";
import {IPage} from './page';
import {IForm} from './form';

export interface NPDFTextFieldOpts {
    maxLen?: number
    multiLine?: boolean
    passwordField?: boolean
    fileField?: boolean
    spellCheckEnabled?: boolean
    scrollEnabled?: boolean
    /**
     * @desc Divide text into equal length combs.
     * @requires NPDFTextFieldOpts#maxLen
     */
    combs?: boolean
    richText?: boolean
}

export enum NPDFCertificatePermission {
    NoPerms = 1,
    FormFill = 2,
    Annotations = 3,
}

export enum NPDFFieldType {
    PushButton,
    CheckBox,
    RadioButton,
    TextField,
    ComboBox,
    ListBox,
    Signature,
    Unknown = 0xff
}
export type NPDFFieldTypeString =
    'TextField'
    | 'CheckBox'
    | 'RadioButton'
    | 'PushButton'
    | 'Signature'
    | 'ListField'
    | 'ComboBox'
    | 'ListBox'

export interface IField {
    readOnly: boolean
    required: boolean
    readonly type: NPDFFieldType
    readonly typeAsString: NPDFFieldTypeString
    fieldName: string
    alternateName?: string
    mappingName?: string
    exported?: boolean

    setBackgroundColor(color: NPDFColor): void

    setBorderColor(color: NPDFColor): void

    setHighlightingMode(mode: NPDFHighlightingMode): void

    setMouseAction(on: NPDFMouseEvent, action: IAction): void

    setPageAction(on: NPDFPageEvent, action: IAction): void
}

export interface ITextField extends IField {
    /**
     * @desc Create from an existing Field
     */
    new(page: IPage, fieldIndex: number): ITextField

    /**
     * @desc Creates a new TextField
     */
    new(form: IForm, annotation: IAnnotation): ITextField

    text: string
    maxLen: number
    multiLine: boolean
    passwordField: boolean
    fileField: boolean
    spellCheckEnabled: boolean
    scrollEnabled: boolean
    combs: boolean
    richText: boolean
}

export interface ICheckBox extends IField {
    /**
     * @desc Create from an existing Field
     */
    new(page: IPage, fieldIndex: number): ICheckBox

    /**
     * @desc Creates a new TextField
     */
    new(form: IForm, annotation: IAnnotation): ICheckBox

    checked: boolean
}

export interface IListField extends IField {
    selected: number
    length: number
    spellCheckEnabled: boolean
    sorted: boolean
    multiSelect: boolean

    isComboBox(): boolean

    insertItem(value: string, displayName: string): void

    removeItem(index: number): void

    getItem(index: number): IListItem
}

export interface IComboBox extends IListField {
    /**
     * @desc Create from an existing Field
     */
    new(page: IPage, fieldIndex: number): IComboBox

    /**
     * @desc Creates a new TextField
     */
    new(form: IForm, annotation: IAnnotation): IComboBox

    editable: boolean
}

export interface IListBox extends IListField {
    /**
     * @desc Create from an existing Field
     */
    new(page: IPage, fieldIndex: number): IListBox

    /**
     * @desc Creates a new TextField
     */
    new(form: IForm, annotation: IAnnotation): IListBox

}

export interface IPushButton extends IField {
     /**
     * @desc Create from an existing Field
     */
    new(page: IPage, fieldIndex: number): IPushButton

    /**
     * @desc Creates a new TextField
     */
    new(form: IForm, annotation: IAnnotation): IPushButton

    rollover:string
    rolloverAlternate: string
}

export enum NPDFAnnotationAppearance {
    normal,
    rollover,
    down
}

export interface ISignatureField {
    new(annotation: IAnnotation, doc: IDocument): ISignatureField

    setAppearanceStream(xObj: any, appearance: NPDFAnnotationAppearance, name: string): void

    setReason(reason: string): void

    setLocation(location: string): void

    setCreator(creator: string): void

    setDate(dateTime?: string): void

    addCertificateReference(perm: NPDFCertificatePermission): void

    setFieldName(name: string): void

    getObject(): IObj

    ensureSignatureObject(): void
}

export type IListItem = {
    value: string,
    display: string
}
