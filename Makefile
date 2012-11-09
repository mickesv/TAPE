CLANGLIB=/opt/local/libexec/llvm-3.1/lib
CLANGINC=/opt/local/libexec/llvm-3.1/include
CFLAGS=-g -L$(CLANGLIB) -I$(CLANGINC)
CC=g++
PARSERS=parser.o functioncallparser.o loopdepthparser.o functionsizeparser.o
CEOBJECTS=codeext.o config.o stringstuff.o model.o csvfile.o debug.o extractor.o $(PARSERS)

%.o: %.cc *.hh
	$(CC) $(CFLAGS) -c $<

# This will have to be modified when we start adding programs
codeext: $(CEOBJECTS)
	$(CC) $(CFLAGS) -lclang -o codeext $(CEOBJECTS)

all: codeext

go: codeext
	LD_LIBRARY_PATH=$(CLANGLIB) ./codeext read=false

gawk: codeext
	LD_LIBRARY_PATH=$(CLANGLIB) ./codeext cfgFile=gawk.cfg read=false


clean:
	rm *.o
	rm *~
	rm codeext
