#@IgnoreInspection BashAddShebang
git clone https://github.com/svn2github/podofo $HOME/podofo && \
cd $HOME/podofo && \
mkdir $HOME/podofo/build && \
cd $HOME/podofo/build && \
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local \
 -DCMAKE_CXX_FLAGS="-fPIC" \
 -DPODOFO_BUILD_SHARED=1 \
 -DPODOFO_BUILD_STATIC=1 \
 -DWANT_BOOST=1 && \
make && make install
npm i -g nopodofo
cd /usr/local/lib/node_modules/nopodofo && npm i && \
./node_modules/.bin/cmake-js build -s=c++17
