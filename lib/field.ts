const mod = require('bindings')('npdf')
import {IPage} from './page'

export interface ITextField {
    _instance:any
    text:string
}
export interface ICheckBox {
    _instance:any
    checked:boolean
}
export interface IField {
    _instance:any
    getType(): string

    getFieldName(): string

    getAlternateName(): string

    getMappingName(): string

    isRequired(): boolean

    setRequired(required: boolean): void

    setAlternateName(name: string): void

    setMappingName(name: string): void
}
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

export type FieldType = 'TextField' | 'CheckBox' | 'ListBox' | 'RadioButton' | 'ComboBox' | 'PushButton' | 'Signature'
export class Field implements IField {
    _instance: any
    constructor(page: IPage, fieldIndex: number) {
        this._instance = new mod.Field(page._instance, fieldIndex)
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

export class TextField implements ITextField {
    _instance: any
    constructor(field: IField) {
        this._instance = new mod.TextField(field._instance)
    }
    get text(): string {
        return this._instance.text
    }
    set text(value: string) {
        this._instance.text = value
    }
}

export class CheckBox implements ICheckBox {
    _instance:any
    get checked() {
        return this._instance.checked
    }
    set checked(value:boolean) {
        this._instance.checked = value
    }
    constructor(field: IField) {
        this._instance = new mod.CheckBox(field._instance)
    }
}

