{
  "targets": [
    {
      "target_name": "libfreetype",
      "type": "static_library",
      "defines": [
        "FT2_BUILD_LIBRARY",
      ],
      "include_dirs": [
        "freetype/include/",
      ],
      "sources": [
        # From vendor/freetype/docs/INSTALL.ANY
        # This is for the default config. If we want to customize the config, we
        # need to create a custom ftconfig.h with the appropriate definitions
        # according to vendor/freetype/docs/CUSTOMIZING.

        # base components (required)
        "freetype/src/base/ftsystem.c",
        "freetype/src/base/ftinit.c",
        "freetype/src/base/ftdebug.c",

        "freetype/src/base/ftbase.c",

        "freetype/src/base/ftbbox.c",       # recommended, see <ftbbox.h>
        "freetype/src/base/ftglyph.c",      # recommended, see <ftglyph.h>

        "freetype/src/base/ftbdf.c",        # optional, see <ftbdf.h>
        "freetype/src/base/ftbitmap.c",     # optional, see <ftbitmap.h>
        "freetype/src/base/ftcid.c",        # optional, see <ftcid.h>
        "freetype/src/base/ftfntfmt.c",     # optional, see <ftfntfmt.h>
        "freetype/src/base/ftfstype.c",     # optional
        "freetype/src/base/ftgasp.c",       # optional, see <ftgasp.h>
        "freetype/src/base/ftgxval.c",      # optional, see <ftgxval.h>
        "freetype/src/base/ftlcdfil.c",     # optional, see <ftlcdfil.h>
        "freetype/src/base/ftmm.c",         # optional, see <ftmm.h>
        "freetype/src/base/ftotval.c",      # optional, see <ftotval.h>
        "freetype/src/base/ftpatent.c",     # optional
        "freetype/src/base/ftpfr.c",        # optional, see <ftpfr.h>
        "freetype/src/base/ftstroke.c",     # optional, see <ftstroke.h>
        "freetype/src/base/ftsynth.c",      # optional, see <ftsynth.h>
        "freetype/src/base/fttype1.c",      # optional, see <t1tables.h>
        "freetype/src/base/ftwinfnt.c",     # optional, see <ftwinfnt.h>

        # font drivers (optional; at least one is needed)
        "freetype/src/bdf/bdf.c",           # BDF font driver
        "freetype/src/cff/cff.c",           # CFF/OpenType font driver
        "freetype/src/cid/type1cid.c",      # Type 1 CID-keyed font driver
        "freetype/src/pcf/pcf.c",           # PCF font driver
        "freetype/src/pfr/pfr.c",           # PFR/TrueDoc font driver
        "freetype/src/sfnt/sfnt.c",         # SFNT files support (TrueType & OpenType)
        "freetype/src/truetype/truetype.c", # TrueType font driver
        "freetype/src/type1/type1.c",       # Type 1 font driver
        "freetype/src/type42/type42.c",     # Type 42 font driver
        "freetype/src/winfonts/winfnt.c",   # Windows FONT / FNT font driver

        # rasterizers (optional; at least one is needed for vector formats)
        "freetype/src/raster/raster.c",     # monochrome rasterizer
        "freetype/src/smooth/smooth.c",     # anti-aliasing rasterizer

        # auxiliary modules (optional)
        "freetype/src/autofit/autofit.c",   # auto hinting module
        "freetype/src/cache/ftcache.c",     # cache sub-system (in beta)
        "freetype/src/gzip/ftgzip.c",       # support for compressed fonts (.gz)
        "freetype/src/lzw/ftlzw.c",         # support for compressed fonts (.Z)
        "freetype/src/bzip2/ftbzip2.c",     # support for compressed fonts (.bz2)
        "freetype/src/gxvalid/gxvalid.c",   # TrueTypeGX/AAT table validation
        "freetype/src/otvalid/otvalid.c",   # OpenType table validation
        "freetype/src/psaux/psaux.c",       # PostScript Type 1 parsing
        "freetype/src/pshinter/pshinter.c", # PS hinting module
        "freetype/src/psnames/psnames.c",   # PostScript glyph names support
      ],
      "conditions": [
        [ "OS=='mac'", {
          "sources": [
            "freetype/src/base/ftmac.c",        # only on the Macintosh
          ],
        }],
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "freetype/include",
        ]
      }
    }
  ]
}