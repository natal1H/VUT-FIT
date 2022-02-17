#/bin/bash
cd && \
wget http://ftp.ruby-lang.org/pub/ruby/2.5/ruby-2.5.3.tar.gz && \
tar -xzvf ruby-2.5.3.tar.gz && \
cd ruby-2.5.3/ && \
./configure && \
make && \
sudo make install && \
ruby -v;
