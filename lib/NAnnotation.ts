import {nopodofo, NPDFAnnotationFlag, NPDFAnnotationType} from '../'
import {NDocument} from "./NDocument"
import {NAction} from "./NAction"
import {NFileSpec} from "./NFileSpec";

export class NAnnotation {
    get title(): string {
        return this.self.title;
    }

    set title(value: string) {
        this.self.title = value;
    }

    get rect(): nopodofo.Rect {
        return this.self.rect;
    }

    set rect(value: nopodofo.Rect) {
        this.self.rect = value;
    }

    get quadPoints(): number[] {
        return this.self.quadPoints;
    }

    set quadPoints(value: number[]) {
        this.self.quadPoints = value;
    }

    get open(): boolean {
        return this.self.open;
    }

    set open(value: boolean) {
        this.self.open = value;
    }

    get flags(): NPDFAnnotationFlag {
        return this.self.flags;
    }

    set flags(value: NPDFAnnotationFlag) {
        this.self.flags = value;
    }

    get destination(): nopodofo.Destination {
        return this.self.destination;
    }

    set destination(value: nopodofo.Destination) {
        this.self.destination = value;
    }

    get content(): string {
        return this.self.content;
    }

    set content(value: string) {
        this.self.content = value;
    }

    get color(): nopodofo.Color {
        return this.self.color;
    }

    set color(value: nopodofo.Color) {
        this.self.color = value;
    }

    get attachment(): NFileSpec | null {
        if (this.self.attachment) {
            return new NFileSpec(this.parent, this.self.attachment)
        }
        return null
    }

    set attachment(value: NFileSpec | null) {
        this.self.attachment = (value as any).self
    }

    get action(): NAction {
        return new NAction(this.parent, this.self.action);
    }

    set action(value: NAction) {
        this.self.action = (value as any).self;
    }

    constructor(private parent: NDocument, self: nopodofo.Annotation) {
        this.self = self
    }

    public self: nopodofo.Annotation

    getType(): NPDFAnnotationType {
        return this.self.getType()
    }

    hasAppearanceStream(): boolean {
        return this.self.hasAppearanceStream()
    }

    setAppearanceStream(xobj: nopodofo.XObject): void {
        this.self.setAppearanceStream(xobj)
    }

    setBorderStyle(v: { horizontal: number; vertical: number; width: number }, stroke?: nopodofo.Array): void {
        this.self.setBorderStyle(v, stroke)
    }
}
