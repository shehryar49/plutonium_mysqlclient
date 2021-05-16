# plutonium_mysqlclient

Plutonium module that allows plutonium programs to connect to mysql server.
This module uses libmysqlclient.
You need to install libmysqlclient on your system first!
HOW TO COMPILE
-------------

g++ -shared mysql.cpp -o mysql.so -fPIC -lmysqlclient
