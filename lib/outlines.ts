import {IObj} from "./object";
import {NPDFExternal} from "./index";
import {IAction, IDestination} from "./action";
import {NPDFColor} from "./painter";

export interface IOutlines {
    new(obj:IObj|NPDFExternal<IObj>): IOutlines
    readonly prev: IObj
    readonly next: IObj
    readonly first: IObj
    readonly last: IObj
    destination: IDestination
    action: IAction
    title: string
    textFormat: any
    textColor: NPDFColor
    createChild(name: string, value: IDestination): any
    createNext(name: string, value: IDestination|IAction): any
    insertItem(item: IObj): void
    getParent(): IOutlines
    erase(): void
}