git clone https://github.com/svn2github/podofo $HOME/podofo && \
cd $HOME/podofo && \
mkdir $HOME/podofo/build && \
cmake -DCMAKE_INSTALL_PREFIX=/usr $HOME/podofo/ \
-DPODOFO_BUILD_SHARED=1 \
-DPODOFO_HAVE_PNG_LIB=1 \
-DPODOFO_HAVE_JPEG_LIB=1 \
-DPODOFO_HAVE_TIFF_LIB=1 \
-DLIBCRYPTO_LIBRARIES=/usr/lib/libcrypto.so \
-DLIBCRYPTO_INCLUDE_DIR=/usr/include/openssl && \
make && \
make install && \
git clone https://github.com/corymickelson/NoPoDoFo $HOME/npdf && \
cd $HOME/npdf && \
npm i && \
npm run compile && npm run lib-build && npm test
