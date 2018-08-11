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
import {IPage} from "./page";

export enum NPDFDestinationType {
    XYZ,
    Fit,
    FitH,
    FitV,
    FitR,
    FitB,
    FitBH,
    FitBV,
    Unknown = 0xFF
}
export enum NPDFDestinationFit {
    Fit,
    FitH,
    FitV,
    FitB,
    FitBH,
    FitBV,
    Unknown = 0xFF
}
export interface IDestination {
    new(page: IPage, fit: NPDFDestinationFit): IDestination
    new(page: IPage, fit: NPDFDestinationFit, fitArg: number): IDestination
    new(page: IPage, left: number, top: number, zoom: number): IDestination
    readonly page: IPage
    readonly type: NPDFDestinationType
}