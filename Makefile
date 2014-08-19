UNAME := $(shell uname -s)
CPPFLAGS=
LDFLAGS= -lpthread -lrt
CPP=gcc
ifeq ($(UNAME),Darwin)
CPP=clang
LDFLAGS=
endif

HEADERS=config.h
SOURCES=memaccesstest.c
EXENAME=memaccesstest


all: $(EXENAME)

$(EXENAME): 
	$(CPP) $(LDFLAGS) $(CPPFLAGS) $(SOURCES) -o $(EXENAME)

clean:
	rm -rf $(OBJECTS) $(EXENAME)




