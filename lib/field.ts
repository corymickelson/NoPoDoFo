// const mod = require('bindings')('npdf')

import {__mod} from './document'
import {IPage} from './page'


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

export type FieldType = 'TextField' | 'CheckBox' | 'RadioButton' | 'PushButton' | 'Signature' | 'ListField'
export class Field  {
    _instance: any
    constructor(page: IPage, fieldIndex: number) {
        this._instance = new __mod.Field(page._instance, fieldIndex)
    }

    getType(): FieldType {
        const type = this._instance.getType()
        return type === 'ComboBox' || type === 'ListBox' ? // Combobox and Listbox both implement ListField
            'ListField': type
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

export class TextField {
    _instance: any
    constructor(field: Field) {
        if(field.getType() !== 'TextField') {
            throw Error('field parameter must be a field of type TextField')
        }
        this._instance = new __mod.TextField(field._instance)
    }
    get text(): string {
        return this._instance.text
    }
    set text(value: string) {
        this._instance.text = value
    }
}

export class CheckBox {
    _instance:any
    get checked() {
        return this._instance.checked
    }
    set checked(value:boolean) {
        this._instance.checked = value
    }
    constructor(field: Field) {
        if(field.getType() !== 'CheckBox') {
            throw Error('must be of type CheckBox')
        }
        this._instance = new __mod.CheckBox(field._instance)
    }
}

export type IListItem = {
    value:string,
    display:string
}
export class ListField {
    private _instance: any
    get selected(): number {
        return this._instance.selected
    }
    set selected(index: number) {
        this._instance.selected = index
    }
    get length() {
        return this._instance.length
    }
    constructor(field: Field) {
        if(field.getType() !== 'ListField') {
            throw Error('must be of type ListField')
        }
        this._instance = new __mod.ListBox(field._instance)
    }

    getItem(index:number): IListItem {
        return this._instance.getItem(index)
    }

    setItem(item: IListItem): void {
        this._instance.insertItem(item.value, item.display)
    }
    removeItem(index: number): void {
        this._instance.removeItem(index)
    }
}