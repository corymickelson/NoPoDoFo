import { IObj } from "./object";

export type NPDFDictionaryKeyType = 'boolean' | 'long' | 'name' | 'real'
export interface NPDFDictionary {
  dirty: boolean
  immutable: boolean
  getKey(k:string): IObj
  addKey(prop:string, value: boolean|number|string|Object):void
  getKeys():string[]
  hasKey(k:string): boolean
  removeKey(k:string): void
  getKeyAs(k:string, t: NPDFDictionaryKeyType): string| number
  clear(): void
  write(destination:string, cb: (e:Error, i:string) => void): void
  writeSync(destination:string): void
}