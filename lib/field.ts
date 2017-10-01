const mod = require('bindings')('npdf')
import {IPage} from './page'

export interface ITextField {
    getText(): string
    setText(value: string): void
}
export interface IField {
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
    private _instance: any
    constructor(page: IPage, fieldIndex: number) {
        this._instance = new mod.Field(page, fieldIndex)
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
    private _instance: any
    constructor(field: IField) {
        this._instance = new mod.TextField(field)
    }
    getText(): string {
        return this._instance.getText()
    }
    setText(value: string): void {
        return this._instance.setText(value)
    }
}
