# peer-chat
Server and Client OpenSSL Peer Chat (Windows/Linux)

On Linux: gcc main.c -o sslchat -lssl -lcrypto -pthread -std=gnu99

On Windows: use DevC++ (latest)

with linker options:

#  -mwindows
# -lssl
# -lcrypto
# -leay32 
# -lwsock32
# -lws2_32 
# -lpthread
# -s

# Code profiling -std ISO C99
