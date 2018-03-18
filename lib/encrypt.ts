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

export type ProtectionSummary = {
    Accessible: boolean
    Print: boolean
    Copy: boolean
    DocAssembly: boolean
    Edit: boolean
    EditNotes: boolean
    FillAndSign: boolean
    HighPrint: boolean
}


export interface IEncrypt {
    owner: string
    user: string
    encryptionKey: string
    keyLength: number
    protections: ProtectionSummary
    isAllowed(action: ProtectionOption): boolean
}

