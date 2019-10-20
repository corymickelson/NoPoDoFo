import {nopodofo, NPDFAnnotation, NPDFFieldType, NPDFPageEvent, NPDFPageLayout, NPDFPageMode} from '../'
import {NDocument} from "./NDocument";
import {NAnnotation} from "./NAnnotation";
import {NObject} from "./NObject";
import {NCheckBox, NComboBox, NField, NListBox, NPushButton, NTextField} from "./NField";
import {NRect} from "./NRect";


export class NPage {
    get width(): number {
        return this.self.width;
    }

    set width(value: number) {
        this.self.width = value;
    }

    get trimBox(): NRect {
        return new NRect(this.parent, this.self.trimBox)
    }

    set trimBox(value: NRect) {
        this.self.trimBox = (value as any).self;
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

    createAnnotation(type: NPDFAnnotation, rect: NRect): NAnnotation {
        const annot = this.self.createAnnotation(type, (rect as any).self)
        return new NAnnotation(this.parent, annot)
    }

    createField(type: NPDFFieldType, annot: NAnnotation, form: nopodofo.Form, opts?: NObject): NField {
        const field = nopodofo.Page.createField(type, annot.self, form, opts as any)
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
        this.self.deleteAnnotation(index)
    }

    deleteField(index: number): void {
        this.self.deleteField(index)
    }

    fieldCount(): number {
        return this.self.fieldCount()
    }

    flattenFields(): void {
        throw Error('Flatten Fields is not yet implemented')
    }

    getAnnotation(index: number): NAnnotation {
        return new NAnnotation(this.parent, this.self.getAnnotation(index))
    }

    getArtBox(): NRect {
        return new NRect(this.parent, this.self.getArtBox())
    }

    getBleedBox(): NRect {
        return new NRect(this.parent, this.self.getBleedBox())
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

    getMediaBox(): NRect {
        return new NRect(this.parent, this.self.getMediaBox())
    }

}
