import {nopodofo, NPDFAnnotation, NPDFFieldType, NPDFPageEvent, NPDFPageLayout, NPDFPageMode} from '../'
import {NDocument} from "./NDocument";
import {NAnnotation} from "./NAnnotation";
import {NObject} from "./NObject";
import Field = nopodofo.Field;
import {NCheckBox, NComboBox, NField, NListBox, NPushButton, NSignatureField, NTextField} from "./NField";


export class NPage {
    get width(): number {
        return this.self.width;
    }

    set width(value: number) {
        this.self.width = value;
    }

    get trimBox(): nopodofo.Rect {
        return this.self.trimBox;
    }

    set trimBox(value: nopodofo.Rect) {
        this.self.trimBox = value;
    }

    get rotation(): number {
        return this.self.rotation;
    }

    set rotation(value: number) {
        this.self.rotation = value;
    }

    get resources(): NObject {
        return new NObject(this.parent, this.self.resources)
    }

    get number(): number {
        return this.self.number;
    }

    get height(): number {
        return this.self.height;
    }

    set height(value: number) {
        this.self.height = value;
    }

    get contents(): NObject {
        return new NObject(this.parent, this.self.contents)
    }

    constructor(private parent: NDocument, public self: nopodofo.Page) {
    }

    annotationCount(): number {
        return this.self.annotationCount()
    }

    createAnnotation(type: NPDFAnnotation, rect: nopodofo.Rect): NAnnotation {
        const annot = this.self.createAnnotation(type, rect)
        return new NAnnotation(this.parent, annot)
    }

    createField(type: NPDFFieldType, annot: NAnnotation, form: nopodofo.Form, opts?: NObject): NField {
        const field = this.self.createField(type, annot.self, form, opts)
        switch (type) {
            case NPDFFieldType.PushButton:
                return new NPushButton(this.parent, field as nopodofo.PushButton)
            case NPDFFieldType.Checkbox:
                return new NCheckBox(this.parent, field as nopodofo.Checkbox)
            case NPDFFieldType.TextField:
                return new NTextField(this.parent, field as nopodofo.TextField)
            case NPDFFieldType.ComboBox:
                return new NComboBox(this.parent, field as nopodofo.ComboBox)
            case NPDFFieldType.ListBox:
                return new NListBox(this.parent, field as nopodofo.ListBox)
            case NPDFFieldType.Signature:
            case NPDFFieldType.RadioButton:
            case NPDFFieldType.Unknown:
                break
        }
        throw Error(`Type ${type} not yet implemented`)
    }

    deleteAnnotation(index: number): void {
    }

    deleteField(index: number): void {
    }

    fieldCount(): number {
        return this.self.fieldCount()
    }

    flattenFields(): void {
    }

    getAnnotation(index: number): NAnnotation {
        return new NAnnotation(this.parent, this.self.getAnnotation(index))
    }

    getArtBox(): nopodofo.Rect {
        return this.self.getArtBox()
    }

    getBleedBox(): nopodofo.Rect {
        return this.self.getBleedBox()
    }

    getField<T extends nopodofo.Field>(index: number): NField {
        const field = this.self.getField(index)
        return NField.create(this.parent, field)
    }

    getFieldIndex(fieldName: string): number {
        return this.self.getFieldIndex(fieldName)
    }

    getFields(): NField[] {
        const fields = this.self.getFields()
        return fields.map(i => NField.create(this.parent, i))
    }

    getMediaBox(): nopodofo.Rect {
        return this.self.getMediaBox()
    }

}
