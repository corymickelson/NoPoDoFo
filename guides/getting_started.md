# Build Dependencies

## Windows

Building podofo on windows is made much simpler with the help of vcpkg. Vcpkg does offer a podofo pacakage but for nopodofo we will want to build podofo from source with additional libraries that are not included in the vcpkg compiled podofo.
This build does not support AES encryption option, to enable AES you will need libidn v1 (which unfortunately is not available via vcpkg).
 - Install [vcpkg](https://github.com/Microsoft/vcpkg)
 - Install libraries `vcpkg install {lib}:x86-windows-static` (or x64-windows-static if using 64 bit node)
   - tiff
   - libpng
   - jpeg 
   - fontconfig
   - freetype
   - openssl
   - zlib

The NoPoDoFo gyp file uses the environment variable `vcpkg_path` for linking the above libraries. This value should be set to your
`{vcpkg root}/installed/{architecture}-windows-static` ex. `C:\vcpkg\installed\x86-windows-static

## Linux

There are many linux distro's, but for this guide I will only be covering arch linux. Other distro's will have similar
installation instruction, if you are truely blocked on the installation of PoDoFo please create an issue.

dependencies:
 - openssl-dev
 - fontconfig-dev
 - libtiff-dev
 - libidn-dev
 - libjpeg-turbo-dev
 - libpng-dev
 - freetype-dev
 - zlib-dev

Install dependencies with `pacman -S [dependencies]`

## Mac
todo

# Building NoPoDoFo

With all dependencies installled you can install NoPoDoFo with `npm i nopodofo`
