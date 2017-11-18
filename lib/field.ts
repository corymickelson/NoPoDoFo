import {__mod, Document} from './document'
import {IPage} from './page'
import {Obj} from "./object";
import {Annotation} from "./annotation";
import {Rect} from "./rect";
import {Form} from "./form";


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
    private _instance: any
    constructor(page: IPage, fieldIndex: number) {
        this._instance = new __mod.Field((page as any)._instance, fieldIndex)
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
        this._instance = new __mod.TextField((field as any)._instance)
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
        this._instance = new __mod.CheckBox((field as any)._instance)
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
        this._instance = new __mod.ListBox((field as any)._instance)
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

export class SignatureField {
    private _instance:any
    constructor(annot:Annotation|any, form?:Form, doc?:Document) {
        if(form instanceof Form && doc instanceof Document) {
            this._instance = new __mod.SignatureField((annot as any)._instance, (form as any)._instance, (doc as any)._instance)
        }
        else {
            this._instance = new __mod.SignatureField(annot)
        }
    }

    // setSignature(data:Buffer): void {
    //     this._instance.setSignature(data)
    // }

    setReason(reason:string): void {
        this._instance.setReason(reason)
    }

    setLocation(local:string): void {
        this._instance.setLocation(local)
    }

    setCreator(creator:string): void {
        this._instance.setCreator(creator)
    }

    setDate(): void {
        this._instance.setDate()
    }

    getObject(): Obj {
        const instance = this._instance.getObject()
        return new Obj(instance)
    }
}