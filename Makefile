NAME := Smiril-peer-chat
MAJOR1 := 1
MINOR1 := 1
CODENAME := \x1B[32mWensday\x1B[39m - \x1B[33mThe Enemy Dail\x1B[39m
LIBNAME := $(NAME)
VERSION := $(LIBNAME) $(MAJOR).$(MINOR) $(CODENAME)
LIBS := 
PREFIX := /bin
CXX := gcc
CX1 := i686-w64-mingw32-gcc
CX2 := i686-darwin-mingw32-gcc
CXXFLAGS := -lpthread -lcrypto -lssl -Wall -Wextra -pedantic \
        -Wno-variadic-macros \
        -DVERSION="\"$(VERSION)\""
        
.PHONY: all peer-chat debug-peer-chat install uninstall clean version

all: peer-chat 
peer-chat: main.c	
	$(CXX) main.c $(CXXFLAGS) -o peer-chat
debug-peer-chat: CXXFLAGS += -g -DDEBUG
debug-peer-chat: main.c
	$(CXX) $(CXXFLAGS) main.c -o debug-peer-chat
version:
	@echo $(VERSION)

install: peer-chat
	mkdir -p $(PREFIX)
	cp peer-chat $(PREFIX)
	chmod +x $(PREFIX)/peer-chat
	
uninstall: 
	rm -rf $(PREFIX)/peer-chat
clean: 
	rm -rf peer-chat
