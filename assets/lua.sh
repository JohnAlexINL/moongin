curl -L -R -O https://www.lua.org/ftp/lua-5.4.7.tar.gz
tar zxf lua-5.4.7.tar.gz
cd lua-5.4.7
make all test
cd lua-5.4.7
sudo make install
rm -rf ./lua-5.4.7 ./lua-5.4.7.tar.gz