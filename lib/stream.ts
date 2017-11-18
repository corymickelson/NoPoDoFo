export class Stream {
    constructor(private _instance:any) {}
    write(output:string, cb:(e:Error, d:any) => void): void {
        this._instance.write(output, cb)
    }
    
}