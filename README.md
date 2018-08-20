# NoPoDoFo

[![CircleCI](https://circleci.com/gh/corymickelson/NoPoDoFo.svg?style=svg)](https://circleci.com/gh/corymickelson/NoPoDoFo)
[![Coverage Status](https://coveralls.io/repos/github/corymickelson/NoPoDoFo/badge.svg?branch=master)](https://coveralls.io/github/corymickelson/NoPoDoFo?branch=master)

NoPoDoFo is a bindings library to [PoDoFo](http://podofo.sourceforge.net/index.html), hence the name No(de)PoDoFo. The aim of this project it to bring a complete PDF solution
to the Nodejs ecosystem. NoPoDoFo provides low level PDF api's for creating new PDF documents, and reading and modifying 
existing PDF documents. Features also include encryption, signing, and drawing api's. Please take note that this project
is currently in <mark>early development</mark>. 

To get started please read [the book]()

### __Current Version 0.7.0__
[![NPM](https://nodei.co/npm/nopodofo.png?downloads=true&downloadRank=true)](http://nodei.co/npm/nopodofo)

### [Typescript Api Documentation](https://corymickelson.github.io/NoPoDoFo/index)<small>, generated via typedoc</small>

## A Brief Introduction

NoPoDoFo is a bindings library for NodeJS to PoDoFo. The project began mid 2017 stemming from my own frustration at the lack of 
open source options in the NodeJS ecosystem for handling PDF creation and modifications. I had at the time written many scripts
for invoking command line utilities from node, such as pdftk and poppler to perform one-off tasks, but as the company I was with
at the time started adopting more and more NodeJS the need for a complete PDF library was becoming increasingly apparent, and nobody
at the time, at least in the open, was doing any work to fill this void, and thus NoPoDoFo was born. 
PoDoFo was chosen as the underlying engine (not really sure if engine is the correct term but it's what we will use for now) after 
considering the ease of building PoDoFo, and the completeness of the api compared to similar lower lever libraries. 
NoPoDoFo is licensed under AGPL, a copy of which is available at the root directory of the project.

## Project Structure

NoPoDoFo bindings `/src` follow as closely as possible the same folder structure as PoDoFo. 
The library `/lib` provides a signature map from native bindings to typescript definitions, as interfaces and enums.
These types are exported when NoPoDoFo is included as a dependency, `import {} from 'nopodofo'`.
Using NoPoDoFo as a stand alone library is possible, but the api is faily low level requiring some familiarity
with the PDF specification. It is my hope that NoPoDoFo will used as a building block for higher level PDF libraries
for the NodeJS ecosystem.

### Conventions

NoPoDoFo exports class definitions with the `I` prefix, ex `IDocument` is the typescript interface to NoPoDoFo::Document, which 
is a wrapper around PoDoFo::PdfMemDocument.

NoPoDoFo exports enum's and other static values with the `NPDF` prefix, ex `NPDFWriteMode` is an enumerable, that has been copied from 
PoDoFo::ePdfWriteMode for NoPoDoFo ease of use. If NoPoDoFo did not create and copy such values the end user would have to have an 
intimate knowledge of PoDoFo.
