import { IObj, Obj, NPDFInternal } from "./object"
import { __mod, Document } from "./document"

export type IDictionaryKeyType = 'boolean' | 'long' | 'name' | 'real'
export interface IDictionary {
  dirty: boolean
  immutable: boolean
  tryGet(doc: Document, candidate: IObj): IDictionary | null
  getKey(k: string): IObj
  addKey(prop: string, value: boolean | number | string | IObj): void
  getKeys(): string[]
  hasKey(k: string): boolean
  removeKey(k: string): void
  getKeyAs(k: string, t: IDictionaryKeyType): string | number
  clear(): void
  write(destination: string, cb: (e: Error, i: string) => void): void
  writeSync(destination: string): void
}
export class Dictionary implements IDictionary {
  static tryGet(doc: Document, candidate: IObj): IDictionary | null {
    if(candidate instanceof Obj)
      return __mod.Dictionary.tryGetDictionary((doc as any)._instance, (candidate as any)._instance)
    else if(candidate instanceof __mod.Obj)
      return __mod.Dictionary.tryGetDictionary((doc as any)._instance, candidate)
    else return null
  }
  tryGet(doc: Document, candidate: IObj): IDictionary | null {
    return Dictionary.tryGet(doc, candidate)
  }
  get immutable(): boolean {
    return this._instance.dirty
  }
  set immutable(v: boolean) {
    this._instance.dirty = v
  }
  get dirty(): boolean {
    return this._instance.dirty
  }
  set dirty(v: boolean) {
    this._instance.dirty = v
  }
  getKey(k: string): IObj {
    return this._instance.getKey(k)
  }
  getKeys(): string[] {
    return this._instance.getKeys()
  }
  hasKey(k: string): boolean {
    return this._instance.hasKey(k)
  }
  removeKey(k: string): void {
    return this._instance.removeKey(k)
  }
  getKeyAs(k: string, t: IDictionaryKeyType): string | number {
    return this._instance.getKeyAs(k, t)
  }
  clear(): void {
    this._instance.clear()
  }
  write(destination: string, cb: (e: Error, i: string) => void): void {
    this._instance.write(destination, cb)
  }
  writeSync(destination: string): void {
    this._instance.writeSync(destination)
  }
  constructor(private _instance: NPDFInternal) { }
  addKey(prop: string, value: boolean | number | string | IObj): void {
    if (typeof prop === 'string' || typeof prop === 'boolean' || typeof prop === 'number') {
      this._instance.addKey(prop, value)
    } else if ((value as any) instanceof Obj || (value as any) instanceof __mod.Obj) {
      value = (value as any)._instance
      this._instance.addKey(prop, value)
    } else {
      throw TypeError('invalid value data type')
    }
  }
}