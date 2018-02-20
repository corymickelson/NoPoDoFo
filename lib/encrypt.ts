/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
 * Authors: Cory Mickelson, et al.
 * 
 * NoPoDoFo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NoPoDoFo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
export type ProtectionOption = 'Copy' | 'Print' | 'Edit' | 'EditNotes' | 'FillAndSign' | 'Accessible' | 'DocAssembly' | 'HighPrint'
export type EncryptOption = {
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
    constructor(private _instance: any, public option?:EncryptOption) { 
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