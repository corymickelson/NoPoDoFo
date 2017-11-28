FROM node:8.9.1-alpine

RUN apk update && \
    apk upgrade && \
    apk add tiff-dev libpng-dev libjpeg-dev fontconfig-dev freetype-dev libidn-dev openssl-dev zlib-dev cmake make gcc g++ git python2 && \
    cd /home && \
    git clone https://github.com/svn2github/podofo && \
    cd podofo && \
    mkdir build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr .. \
    -DPODOFO_HAVE_PNG_LIB=1 \
    -DPODOFO_HAVE_JPEG_LIB=1 \
    -DPODOFO_HAVE_TIFF_LIB=1 \
    -DLIBCRYPTO_LIBRARIES=/usr/lib/libcrypto.so \
    -DLIBCRYPT_INCLUDE_DIR=/usr/include/openssl && \
    make && \
    make install && \
    cd /home && \
    git clone https://github.com/corymickelson/NoPoDoFo npdf && \
    cd npdf && \
    npm i --podofo_library=/home/lib/libpodofo.so --podofo_include_dir=/home/include
