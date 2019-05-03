import {NDocument} from "./NDocument";
import {ListItem, nopodofo, NPDFFieldType, NPDFHighlightingMode, NPDFMouseEvent, NPDFPageEvent} from "../index";
import {NAnnotation} from "./NAnnotation";
import {NAction} from "./NAction";
import {NObject} from "./NObject";

export class NField {
    get DA(): string {
        if (!this.self.DA) {
            return ''
        } else {
            return this.self.DA
        }
    }

    set DA(value: string) {
        this.self.DA = value;
    }

    get AP(): nopodofo.Dictionary {
        if (!this.self.AP) {
            this.self.AP = new nopodofo.Dictionary()
        }
        return this.self.AP
    }

    set AP(value: nopodofo.Dictionary) {
        this.self.AP = value;
    }

    get exported(): boolean {
        return this.self.exported;
    }

    set exported(value: boolean) {
        this.self.exported = value;
    }

    get mappingName(): string {
        if (!this.self.mappingName) {
            return ''
        } else {
            return this.self.mappingName
        }
    }

    set mappingName(value: string) {
        this.self.mappingName = value;
    }

    get alternateName(): string {
        if (!this.self.alternateName) {
            return ''
        } else {
            return this.self.alternateName
        }
    }

    set alternateName(value: string) {
        this.self.alternateName = value;
    }

    get fieldName(): string {
        return this.self.fieldName
    }

    set fieldName(value: string) {
        this.self.fieldName = value;
    }

    get widgetAnnotation(): NAnnotation {
        return new NAnnotation(this.parent, this.self.widgetAnnotation)
    }

    get type(): NPDFFieldType {
        return this.self.type
    }

    get obj(): NObject {
        return new NObject(this.parent, this.self.obj)
    }


    constructor(private parent: NDocument, protected self: nopodofo.Field) {
    }


    setBackgroundColor(color: nopodofo.Color): void {
        this.self.setBackgroundColor(color)
    }

    setBorderColor(color: nopodofo.Color): void {
        this.self.setBorderColor(color)
    }

    setHighlightingMode(mode: NPDFHighlightingMode): void {
        this.self.setHighlightingMode(mode)
    }

    setMouseAction(on: NPDFMouseEvent, action: NAction): void {
        this.self.setMouseAction(on, action.self)
    }

    setDate(dateTime?: string | nopodofo.Date): void {
        this.self.setDate(dateTime)
    }

    setPageAction(on: NPDFPageEvent, action: NAction): void {
        this.self.setPageAction(on, action.self)
    }

    static create<T extends nopodofo.Field>(parent: NDocument, field: T): NField {
        if (field instanceof nopodofo.TextField) {
            return new NTextField(parent, field)
        } else if (field instanceof nopodofo.ComboBox) {
            return new NComboBox(parent, field)
        } else if (field instanceof nopodofo.ListBox) {
            return new NListBox(parent, field)
        } else if (field instanceof nopodofo.PushButton) {
            return new NPushButton(parent, field)
        } else if (field instanceof nopodofo.Checkbox) {
            return new NCheckBox(parent, field)
        } else {
            throw TypeError()
        }
    }
}

abstract class NListField<T extends nopodofo.ListField> extends NField {
    get multiSelect(): boolean {
        return (super.self as nopodofo.ListField).multiSelect;
    }

    set multiSelect(value: boolean) {
        (super.self as nopodofo.ListField).multiSelect = value;
    }

    get sorted(): boolean {
        return (super.self as nopodofo.ListField).sorted;
    }

    set sorted(value: boolean) {
        (super.self as nopodofo.ListField).sorted = value;
    }

    get spellCheckEnabled(): boolean {
        return (super.self as nopodofo.ListField).spellCheckEnabled;
    }

    set spellCheckEnabled(value: boolean) {
        (super.self as nopodofo.ListField).spellCheckEnabled = value;
    }

    get length(): number {
        return (super.self as nopodofo.ListField).length;
    }

    set length(value: number) {
        (super.self as nopodofo.ListField).length = value;
    }

    get selected(): number {
        return (super.self as nopodofo.ListField).selected;
    }

    set selected(value: number) {
        (super.self as nopodofo.ListField).selected = value;
    }

    protected constructor(parent: NDocument, self: nopodofo.ListField) {
        super(parent, self)
    }

    isComboBox(): boolean {
        return (super.self as nopodofo.ListField).isComboBox()
    }

    insertItem(value: string, displayName: string): void {
        return (super.self as nopodofo.ListField).insertItem(value, displayName)
    }

    removeItem(index: number): void {
        return (super.self as nopodofo.ListField).removeItem(index)
    }

    getItem(index: number): ListItem {
        return (super.self as nopodofo.ListField).getItem(index)
    }
}

export class NCheckBox extends NField {
    get checked(): boolean {
        return (super.self as nopodofo.Checkbox).checked
    }

    set checked(v: boolean) {
        (super.self as nopodofo.Checkbox).checked = v
    }

    constructor(parent: NDocument, self: nopodofo.Checkbox) {
        super(parent, self);
    }

}

export class NTextField extends NField {

    get richText(): boolean {
        return (super.self as nopodofo.TextField).richText;
    }

    set richText(value: boolean) {
        (super.self as nopodofo.TextField).richText = value;
    }

    get combs(): boolean {
        return (super.self as nopodofo.TextField).combs;
    }

    set combs(value: boolean) {
        (super.self as nopodofo.TextField).combs = value;
    }

    get scrollEnabled(): boolean {
        return (super.self as nopodofo.TextField).scrollEnabled;
    }

    set scrollEnabled(value: boolean) {
        (super.self as nopodofo.TextField).scrollEnabled = value;
    }

    get spellCheckEnabled(): boolean {
        return (super.self as nopodofo.TextField).spellCheckEnabled;
    }

    set spellCheckEnabled(value: boolean) {
        (super.self as nopodofo.TextField).spellCheckEnabled = value;
    }

    get fileField(): boolean {
        return (super.self as nopodofo.TextField).fileField;
    }

    set fileField(value: boolean) {
        (super.self as nopodofo.TextField).fileField = value;
    }

    get passwordField(): boolean {
        return (super.self as nopodofo.TextField).passwordField;
    }

    set passwordField(value: boolean) {
        (super.self as nopodofo.TextField).passwordField = value;
    }

    get multiLine(): boolean {
        return (super.self as nopodofo.TextField).multiLine;
    }

    set multiLine(value: boolean) {
        (super.self as nopodofo.TextField).multiLine = value;
    }

    get maxLen(): number {
        return (super.self as nopodofo.TextField).maxLen;
    }

    set maxLen(value: number) {
        (super.self as nopodofo.TextField).maxLen = value;
    }

    get text(): string {
        return (super.self as nopodofo.TextField).text;
    }

    set text(value: string) {
        (super.self as nopodofo.TextField).text = value;
    }

    constructor(parent: NDocument, self: nopodofo.TextField) {
        super(parent, self)
    }

    refreshAppearanceStream(): void {
        (super.self as nopodofo.TextField).refreshAppearanceStream()
    }
}

export class NListBox extends NListField<nopodofo.ListBox> {
    constructor(parent: NDocument, self: nopodofo.ListBox) {
        super(parent, self)
    }
}

export class NComboBox extends NListField<nopodofo.ComboBox> {
    get editable(): boolean {
        return (super.self as nopodofo.ComboBox).editable
    }

    set editable(v: boolean) {
        (super.self as nopodofo.ComboBox).editable = v
    }

    constructor(parent: NDocument, self: nopodofo.ListBox) {
        super(parent, self)
    }
}

export class NPushButton extends NField {
    get rolloverAlternate(): string {
        return (super.self as nopodofo.PushButton).rolloverAlternate;
    }

    set rolloverAlternate(value: string) {
        (super.self as nopodofo.PushButton).rolloverAlternate = value;
    }

    get rollover(): string {
        return (super.self as nopodofo.PushButton).rollover;
    }

    set rollover(value: string) {
        (super.self as nopodofo.PushButton).rollover = value;
    }

    constructor(parent: NDocument, self: nopodofo.PushButton) {
        super(parent, self)
    }

}

export class NSignatureField {
    constructor(private parent: NDocument, private self: nopodofo.SignatureField) {
    }
}
