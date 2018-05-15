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
import {__mod, Document} from './document'
import {NPDFInternal, IObj} from "./object";
import {Annotation} from "./annotation";


export interface IFieldInfo {
    name: string,
    alternateName: string,
    mappingName: string,
    required: boolean,
    readOnly: boolean,
    value: string | number,
    maxLen?: number,
    multiLine?: boolean,
    caption?: string,
    type: string,
    selected?: string
}

export type FieldType =
    'TextField'
    | 'CheckBox'
    | 'RadioButton'
    | 'PushButton'
    | 'Signature'
    | 'ListField'
    | 'ComboBox'
    | 'ListBox'

export class Field {

    get readOnly():boolean {
        return this._instance.readOnly
    }
    set readOnly(v:boolean) {
        this._instance.readOnly = v
    }
    constructor(private _instance: NPDFInternal) {
    }

    getType(): FieldType {
        return this._instance.getType()
    }

    getFieldName(): string {
        return this._instance.getFieldName()
    }

    getAlternateName(): string {
        return this._instance.getAlternateName()
    }

    getMappingName(): string {
        return this._instance.getMappingName()
    }

    isRequired(): boolean {
        return this._instance.isRequired()
    }

    setRequired(required: boolean): void {
        this._instance.setRequired(required)
    }

    setAlternateName(name: string): void {
        this._instance.setAlternateName(name)
    }

    setMappingName(name: string): void {
        this._instance.setMappingName(name)
    }
}

export class TextField extends Field {
    private _textFieldInstance: any

    constructor(field: Field) {
        super((field as any)._instance)
        if (field.getType() !== 'TextField') {
            throw Error('field parameter must be a field of type TextField')
        }
        this._textFieldInstance = new __mod.TextField((field as any)._instance)
    }

    get text(): string {
        return this._textFieldInstance.text
    }

    set text(value: string) {
        this._textFieldInstance.text = value
    }
}

export class CheckBox extends Field {
    private _checkboxInstance: any

    get checked() {
        return this._checkboxInstance.checked
    }

    set checked(value: boolean) {
        this._checkboxInstance.checked = value
    }

    constructor(field: Field) {
        super((field as any)._instance)
        if (field.getType() !== 'CheckBox') {
            throw Error('must be of type CheckBox')
        }
        this._checkboxInstance = new __mod.CheckBox((field as any)._instance)
    }
}

export type IListItem = {
    value: string,
    display: string
}

export class EnumerableField extends Field {
    private _enumerableFieldInstance: any

    get selected(): number {
        return this._enumerableFieldInstance.selected
    }

    set selected(index: number) {
        this._enumerableFieldInstance.selected = index
    }

    get length() {
        return this._enumerableFieldInstance.length
    }

    constructor(field: Field) {
        super((field as any)._instance)
        const t = field.getType()
        if (t === 'ListBox' || t === 'ComboBox') {
            this._enumerableFieldInstance = new __mod.ListField((field as any)._instance)
        } else {
            throw TypeError("EnumerableField must be a field type ListBox or ComboBox")
        }
    }

    getItem(index: number): IListItem {
        return this._enumerableFieldInstance.getItem(index)
    }

    setItem(item: IListItem): void {
        this._enumerableFieldInstance.insertItem(item.value, item.display)
    }

    removeItem(index: number): void {
        this._enumerableFieldInstance.removeItem(index)
    }
}

export class ListBox extends EnumerableField {
    private _listBoxInstance: NPDFInternal

    constructor(field: Field) {
        super(field)
        if (field.getType() !== 'ListBox') {
            throw TypeError('Must be field type ListBox')
        }
        this._listBoxInstance = new __mod.ListBox((field as any)._instance)
    }
}

export class ComboBox extends EnumerableField {
    private _comboBoxInstance: NPDFInternal

    constructor(field: Field) {
        super(field)
        if (field.getType() !== 'ComboBox') {
            throw TypeError('Must be field type ComboBox')
        }
        this._comboBoxInstance = new __mod.ComboBox((field as any)._instance)
    }
}

export class SignatureField {
    private _instance: any

    constructor(annot: Annotation | any, doc?: Document) {
        if (doc instanceof Document) {
            this._instance = new __mod.SignatureField((annot as any)._instance, doc.form, (doc as any)._instance)
        }
        else {
            this._instance = new __mod.SignatureField(annot)
        }
    }

    setReason(reason: string): void {
        this._instance.setReason(reason)
    }

    setLocation(local: string): void {
        this._instance.setLocation(local)
    }

    setCreator(creator: string): void {
        this._instance.setCreator(creator)
    }

    setDate(): void {
        this._instance.setDate()
    }

    setFieldName(n: string): void {
        this._instance.setFieldName(n)
    }

    getObject(): IObj {
        return this._instance.getObject()
    }
}