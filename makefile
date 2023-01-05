

ifndef BUILDDIR 
	export BUILDDIR=$(shell pwd)/build
endif

ifeq ($(OS),Windows_NT)
#windows stuff here
	MD=mkdir
	LIBFILE=libgeopack.dll
else
#linux and mac here
	OS=$(shell uname -s)
	MD=mkdir -p
	ifeq ($(OS),Linux)
		LIBFILE=libgeopack.so
	else
		LIBFILE=libgeopack.dylib
	endif
endif

ifeq ($(PREFIX),)
#install path
	PREFIX=/usr/local
endif


.PHONY: all lib obj clean test install testinstall uninstall

all: obj lib

windows: winobj winlib

obj:
	$(MD) $(BUILDDIR)
	cd src; make obj

lib:
	cd src; make lib

winobj:
	$(MD) $(BUILDDIR)
	cd src; make winobj

winlib: 
	cd src; make winlib


test:
	cd test; make all

clean:
	cd test; make clean
	-rm -v lib/$(LIBFILE)
	-rm -v build/*.o
	-rmdir -v build
	-rm -v testinstall

install:
	cp -v include/geopack.h $(PREFIX)/include
	cp -v include/geopackc.h $(PREFIX)/include

	cp -v lib/$(LIBFILE) $(PREFIX)/lib
	chmod 0775 $(PREFIX)/lib/$(LIBFILE)
ifeq ($(OS),Linux)
	ldconfig
endif


uninstall:
	rm -v $(PREFIX)/include/geopack.h
	rm -v $(PREFIX)/include/geopackc.h
	rm -v $(PREFIX)/lib/$(LIBFILE)
ifeq ($(OS),Linux)
	ldconfig
endif

testinstall:
	g++ -std=c++17 test/testc_installed.cc -o testinstall -lgeopack
	./testinstall
	rm -v testinstall