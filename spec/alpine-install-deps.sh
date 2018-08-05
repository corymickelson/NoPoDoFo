#@IgnoreInspection BashAddShebang
git clone https://github.com/svn2github/podofo $HOME/podofo && \
cd $HOME/podofo && \
mkdir $HOME/podofo/build && \
cd $HOME/podofo/build && \
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/  && \
make && make install
