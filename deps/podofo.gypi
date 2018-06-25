{
  "targets": [
    {
      "target_name": "libpodofo",
      "cflags": [ "-w" ],
      "defines": [
        "BUILDING_PODOFO=1",
      ],
      # "dependencies": [
      #   "./idn.gypi:libidn"
      # ],
      "type": "static_library",
      "sources": [
        # From PODOFO_BASE_SOURCES
        "../node_modules/podofo/src/base/PdfArray.cpp",
        "../node_modules/podofo/src/base/PdfCanvas.cpp",
        "../node_modules/podofo/src/base/PdfColor.cpp",
        "../node_modules/podofo/src/base/PdfContentsTokenizer.cpp",
        "../node_modules/podofo/src/base/PdfData.cpp",
        "../node_modules/podofo/src/base/PdfDataType.cpp",
        "../node_modules/podofo/src/base/PdfDate.cpp",
        "../node_modules/podofo/src/base/PdfDictionary.cpp",
        "../node_modules/podofo/src/base/PdfEncoding.cpp",
        "../node_modules/podofo/src/base/PdfEncodingFactory.cpp",
        "../node_modules/podofo/src/base/PdfEncrypt.cpp",
        "../node_modules/podofo/src/base/PdfError.cpp",
        "../node_modules/podofo/src/base/PdfFileStream.cpp",
        "../node_modules/podofo/src/base/PdfFilter.cpp",
        "../node_modules/podofo/src/base/PdfFiltersPrivate.cpp",
        "../node_modules/podofo/src/base/PdfImmediateWriter.cpp",
        "../node_modules/podofo/src/base/PdfInputDevice.cpp",
        "../node_modules/podofo/src/base/PdfInputStream.cpp",
        "../node_modules/podofo/src/base/PdfLocale.cpp",
        "../node_modules/podofo/src/base/PdfMemStream.cpp",
        "../node_modules/podofo/src/base/PdfMemoryManagement.cpp",
        "../node_modules/podofo/src/base/PdfName.cpp",
        "../node_modules/podofo/src/base/PdfObject.cpp",
        "../node_modules/podofo/src/base/PdfObjectStreamParserObject.cpp",
        "../node_modules/podofo/src/base/PdfOutputDevice.cpp",
        "../node_modules/podofo/src/base/PdfOutputStream.cpp",
        "../node_modules/podofo/src/base/PdfParser.cpp",
        "../node_modules/podofo/src/base/PdfParserObject.cpp",
        "../node_modules/podofo/src/base/PdfRect.cpp",
        "../node_modules/podofo/src/base/PdfRefCountedBuffer.cpp",
        "../node_modules/podofo/src/base/PdfRefCountedInputDevice.cpp",
        "../node_modules/podofo/src/base/PdfReference.cpp",
        "../node_modules/podofo/src/base/PdfStream.cpp",
        "../node_modules/podofo/src/base/PdfString.cpp",
        "../node_modules/podofo/src/base/PdfTokenizer.cpp",
        "../node_modules/podofo/src/base/PdfVariant.cpp",
        "../node_modules/podofo/src/base/PdfVecObjects.cpp",
        "../node_modules/podofo/src/base/PdfWriter.cpp",
        "../node_modules/podofo/src/base/PdfXRef.cpp",
        "../node_modules/podofo/src/base/PdfXRefStream.cpp",
        "../node_modules/podofo/src/base/PdfXRefStreamParserObject.cpp",

        # From PODOFO_DOC_SOURCES,
        "../node_modules/podofo/src/doc/PdfAcroForm.cpp",
        "../node_modules/podofo/src/doc/PdfAction.cpp",
        "../node_modules/podofo/src/doc/PdfAnnotation.cpp",
        "../node_modules/podofo/src/doc/PdfCMapEncoding.cpp",
        "../node_modules/podofo/src/doc/PdfContents.cpp",
        "../node_modules/podofo/src/doc/PdfDestination.cpp",
        "../node_modules/podofo/src/doc/PdfDifferenceEncoding.cpp",
        "../node_modules/podofo/src/doc/PdfDocument.cpp",
        "../node_modules/podofo/src/doc/PdfElement.cpp",
        "../node_modules/podofo/src/doc/PdfEncodingObjectFactory.cpp",
        "../node_modules/podofo/src/doc/PdfExtGState.cpp",
        "../node_modules/podofo/src/doc/PdfField.cpp",
        "../node_modules/podofo/src/doc/PdfFileSpec.cpp",
        "../node_modules/podofo/src/doc/PdfFont.cpp",
        "../node_modules/podofo/src/doc/PdfFontCID.cpp",
        "../node_modules/podofo/src/doc/PdfFontCache.cpp",
        "../node_modules/podofo/src/doc/PdfFontConfigWrapper.cpp",
        "../node_modules/podofo/src/doc/PdfFontFactory.cpp",
        "../node_modules/podofo/src/doc/PdfFontMetrics.cpp",
        "../node_modules/podofo/src/doc/PdfFontMetricsBase14.cpp",
        "../node_modules/podofo/src/doc/PdfFontMetricsFreetype.cpp",
        "../node_modules/podofo/src/doc/PdfFontMetricsObject.cpp",
        "../node_modules/podofo/src/doc/PdfFontSimple.cpp",
        "../node_modules/podofo/src/doc/PdfFontTTFSubset.cpp",
        "../node_modules/podofo/src/doc/PdfFontTrueType.cpp",
        "../node_modules/podofo/src/doc/PdfFontType1.cpp",
        "../node_modules/podofo/src/doc/PdfFontType1Base14.cpp",
        "../node_modules/podofo/src/doc/PdfFontType3.cpp",
        "../node_modules/podofo/src/doc/PdfFunction.cpp",
        "../node_modules/podofo/src/doc/PdfHintStream.cpp",
        "../node_modules/podofo/src/doc/PdfIdentityEncoding.cpp",
        "../node_modules/podofo/src/doc/PdfImage.cpp",
        "../node_modules/podofo/src/doc/PdfInfo.cpp",
        "../node_modules/podofo/src/doc/PdfMemDocument.cpp",
        "../node_modules/podofo/src/doc/PdfNamesTree.cpp",
        "../node_modules/podofo/src/doc/PdfOutlines.cpp",
        "../node_modules/podofo/src/doc/PdfPage.cpp",
        "../node_modules/podofo/src/doc/PdfPagesTree.cpp",
        "../node_modules/podofo/src/doc/PdfPagesTreeCache.cpp",
        "../node_modules/podofo/src/doc/PdfPainter.cpp",
        "../node_modules/podofo/src/doc/PdfPainterMM.cpp",
        "../node_modules/podofo/src/doc/PdfShadingPattern.cpp",
        "../node_modules/podofo/src/doc/PdfSignOutputDevice.cpp",
        "../node_modules/podofo/src/doc/PdfSignatureField.cpp",
        "../node_modules/podofo/src/doc/PdfStreamedDocument.cpp",
        "../node_modules/podofo/src/doc/PdfTable.cpp",
        "../node_modules/podofo/src/doc/PdfTilingPattern.cpp",
        "../node_modules/podofo/src/doc/PdfXObject.cpp",
      ],
      ############### Windows ####################
      "conditions": [
        ["OS=='win'", {
          "cflags": [
            "/EHsc",
            '/EH'
          ],
          "variables": {
            "VCPKG_Path%": '$(vcpkg_path)'
          },
          "libraries": [
            "-lws2_32",
            "-l<(VCPKG_Path)/lib/bz2.lib",
            "-l<(VCPKG_Path)/lib/expat.lib",
            "-l<(VCPKG_Path)/lib/fontconfig.lib",
            "-l<(VCPKG_Path)/lib/freetype.lib",
            "-l<(VCPKG_Path)/lib/jpeg.lib",
            "-l<(VCPKG_Path)/lib/libcharset.lib",
            "-l<(VCPKG_Path)/lib/libeay32.lib",
            "-l<(VCPKG_Path)/lib/libiconv.lib",
            "-l<(VCPKG_Path)/lib/libpng16.lib",
            "-l<(VCPKG_Path)/lib/lzma.lib",
            "-l<(VCPKG_Path)/lib/ssleay32.lib",
            "-l<(VCPKG_Path)/lib/tiff.lib",
            "-l<(VCPKG_Path)/lib/tiffxx.lib",
            "-l<(VCPKG_Path)/lib/turbojpeg.lib",
          ],
          "include_dirs": [
            "<(VCPKG_Path)/include",
            "<(VCPKG_Path)/include/fontconfig", 
            "<(VCPKG_Path)/include/freetype", 
            "<(VCPKG_Path)/include/boost", 
            "<(VCPKG_Path)/include/libpng16", 
            "<(VCPKG_Path)/include/lzma", 
            "<(VCPKG_Path)/include/openssl", 
            "<(VCPKG_Path)/include/freetype", 
          ],
        }],
         ############### linux ####################
        ["OS=='linux'", {
          "cflags_cc!": [
            "-fno-exceptions",
            "-fno-rtti"
          ],
          "variables": {
            "type": "static_library",
          },
          "libraries": [
            '<!@(pkg-config openssl --libs)',
            '<!@(pkg-config fontconfig --libs)',
            '<!@(pkg-config libpng --libs)',
            '<!@(pkg-config libjpeg --libs)',
            '<!@(pkg-config libpng --libs)',
            '<!@(pkg-config freetype2 --libs)',
            '<!@(pkg-config libidn --libs)',
            '<!@(pkg-config libtiff-4 --libs)',
          ],
          "include_dirs": [
            '<!@(pkg-config openssl --cflags-only-I | sed s/-I//g)',
            # '<(node_root_dir)/deps/openssl/openssl/include',
            '<!@(pkg-config libpng --cflags-only-I | sed s/-I//g)',
            '<!@(pkg-config fontconfig --cflags-only-I | sed s/-I//g)',
            '<!@(pkg-config freetype2 --cflags-only-I | sed s/-I//g)',
            '<!@(pkg-config libjpeg --cflags-only-I | sed s/-I//g)',
            '<!@(pkg-config libidn --cflags-only-I | sed s/-I//g)',
            '<!@(pkg-config libtiff-4 --cflags-only-I | sed s/-I//g)',
          ]
        }],
        ############### Darwin ####################
        ["OS=='mac'", {
          "xcode_settings": {
           "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
           "GCC_ENABLE_CPP_RTTI": "YES"
          }
        }]
      ], # end conditions
      "configurations": {
        "Release": {
          "msvs_settings": {
            "VCCLCompilerTool": {
              "RuntimeTypeInfo": "true"
            }
          }
        }
      },
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1
        }
      },
      "include_dirs": [
        "../node_modules/podofo/podofo/",
        "../node_modules/podofo/src/",
        "../node_modules/podofo/",
        "overrides/<(OS)/<(target_arch)/"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "../node_modules/podofo/src/",
          "../node_modules/podofo/",
          "overrides/<(OS)/<(target_arch)/"
        ]
      }
    }
  ]
}