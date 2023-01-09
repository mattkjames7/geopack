

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
	cd lib/libspline; make obj
	cd lib/datetime; make obj
	cd src; make obj

lib:
	cd src; make lib

winobj:
	$(MD) $(BUILDDIR)
	cd lib/libspline; make obj
	cd lib/datetime; make obj
	cd src; make winobj

winlib: 
	cd src; make winlib


test:
	cd test; make all

clean:
	-rm -v lib/libgeopack.*
	-rm -v build/*.o
	-rmdir -v build
	-rm -v testinstall

install:
	cp -v include/geopack.h $(PREFIX)/include

	cp -v lib/$(LIBFILE) $(PREFIX)/lib
	chmod 0775 $(PREFIX)/lib/$(LIBFILE)
ifeq ($(OS),Linux)
	ldconfig
endif


uninstall:
	rm -v $(PREFIX)/include/geopack.h
	rm -v $(PREFIX)/lib/$(LIBFILE)
ifeq ($(OS),Linux)
	ldconfig
endif

testinstall:
	g++ test/test.cc -o testcc -lgeopack
	gcc test/test.c -o testc -lgeopack
	./testcc
	./testc
	rm -v testcc
	rm -v testc