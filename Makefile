#
# generic makefile
#

AVS_PATH=/usr/avs
INC_FILE=$(AVS_PATH)/include/Makeinclude
include $(INC_FILE)

include Makeinclude.$(shell uname)

INC = $(AVS_PATH)/include $(INCLUDE_EXTRA)
CFLAGS = -I$(INC) $(LOCAL_CFLAGS) $(G) -Wall $(OPTIMIZATION_FLAGS)
CC=gcc

.PHONY: install uninstall clean all

all:	libprojectX.a libprojectX_a.a

libprojectX.a:  	projectX.o
	ar rcs libprojectX.a projectX.o

libprojectX_a.a:	projectX_a.o
	ar rcs libprojectX_a.a projectX_a.o

projectX.o:     	projectX.c projectX.h
	gcc -c projectX.c -DDO_AVS $(CFLAGS)

projectX_a.o:   	projectX.c projectX.h
	gcc -c projectX.c -o projectX_a.o $(CFLAGS)


install:	libprojectX.a libprojectX_a.a
	cp libprojectX.a /usr/lib
	chown root /usr/lib/libprojectX.a
	cp libprojectX_a.a /usr/lib
	chown root /usr/lib/libprojectX_a.a
	cp projectX.h /usr/include
	chown root /usr/include/projectX.h
	cp projectX_interface.h /usr/include
	chown root /usr/include/projectX_interface.h
	ranlib /usr/lib/libprojectX.a
	ranlib /usr/lib/libprojectX_a.a

uninstall:
	rm /usr/lib/libprojectX.a
	rm /usr/lib/libprojectX_a.a
	rm /usr/include/projectX.h
	rm /usr/include/projectX_interface.h

clean:
	rm projectX.o
	rm projectX_a.o
	rm libprojectX.a
	rm libprojectX_a.a

