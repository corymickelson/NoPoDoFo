import { nopodofo as npdf } from '../../';
export declare class FormsSpec {
    doc?: npdf.Base;
    page?: npdf.Page;
    setupTests(): void;
    createStreamDocumentForm(): void;
    formInstanceAccessors(): void;
    formDefaults(): void;
    formResourceDictionary(): void;
}
