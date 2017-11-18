export type ProtectionOption = 'Copy' | 'Print' | 'Edit' | 'EditNotes' | 'FillAndSign' | 'Accessible' | 'DocAssembly' | 'HighPrint'
export type EncryptInitOption = {
    userPassword?: string
    ownerPassword?: string
    protection?: Array<ProtectionOption>
    algorithm?: 'rc4v1' | 'rc4v2' | 'aesv2' | 'aesv3'
    keyLength?: number
}
export class Encrypt {
    private _complete = true;
    public get owner() {
        return this._instance.owner
    }
    public get user() {
        return this._instance.user
    }
    public get encryptionKey() {
        return this._instance.encryptionKey
    }
    public get keyLength() {
        return this._instance.keyLength
    }
    public get permission() {
        return this._instance.permission
    }
    constructor(private _instance: any, public option?:EncryptInitOption) { 
        if(_instance === null && option instanceof Object) {
            this._complete = false;
        }
    }
    isAllowed(action: ProtectionOption): boolean {
        if(!this._complete) {
            throw Error('Encryption has not yet been persisted. Use Document.encrypt = {this} to set encryption')
        }
        return this._instance.isAllowed(action)
    }
    /**
     * 
     * @param pwd - user password -OR- owner password
     * @returns {boolean} - true / false is password correct
     */
    authenticate(pwd: { ownerPassword?: string | undefined; userPassword?: string | undefined; }): boolean {
        if(!this._complete) {
            throw Error('Encryption has not yet been persisted. Use Document.encrypt = {this} to set encryption')
        }
        return this._instance.authenticate(pwd)
    }
}