CLANGLIB=/opt/local/libexec/llvm-3.1/lib
CLANGINC=/opt/local/libexec/llvm-3.1/include
CFLAGS=-g -L$(CLANGLIB) -I$(CLANGINC)
CC=g++
PARSERS=parser.o functioncallparser.o loopdepthparser.o functionsizeparser.o gvaccessparser.o
COMMON= config.o stringstuff.o model.o csvfile.o debug.o
CEOBJECTS=codeext.o extractor.o $(COMMON) $(PARSERS)
GRMOBJECTS=grmaker.o graphmaker.o $(COMMON)
#GRAOBJECTS=granalyse.o componentmaker.o $(COMMON)
GRAOBJECTS=granalyse.o componentmaker.o $(COMMON)

%.o: %.cc *.hh
	$(CC) $(CFLAGS) -c $<

# This will have to be modified when we start adding programs
codeext: $(CEOBJECTS)
	$(CC) $(CFLAGS) -lclang -o codeext $(CEOBJECTS)

grmaker: $(GRMOBJECTS)
	$(CC) $(CFLAGS) -lclang -o grmaker $(GRMOBJECTS)

granalyse: $(GRAOBJECTS)
	$(CC) $(CFLAGS) -lclang -o granalyse $(GRAOBJECTS)

all: codeext grmaker granalyse

go: all
	./codeext debugLevel=0
	./granalyse debugLevel=0
	@echo "----------"
	@echo "Final Model:"
	@cat msvtest.csv
	@echo "----------"
	./grmaker debugLevel=0


gawk: all
#	./codeext cfgFile=gawk.cfg
	./granalyse cfgFile=gawk.cfg
#	@echo "Final Model:"
#	@cat gawk.csv
#	@echo "----------"
	./grmaker cfgFile=gawk.cfg


clean:
	rm *.o
	rm *~
	rm codeext
