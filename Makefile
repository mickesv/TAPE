CLANGLIB=/opt/local/libexec/llvm-3.1/lib
CLANGINC=/opt/local/libexec/llvm-3.1/include
CFLAGS=-g -L$(CLANGLIB) -I$(CLANGINC)
CC=g++
PARSERS=parser.o functioncallparser.o loopdepthparser.o functionsizeparser.o gvaccessparser.o verboseparser.o
COMMON= config.o stringstuff.o model.o csvfile.o debug.o basicnodetypes.o
CEOBJECTS=codeext.o extractor.o $(COMMON) $(PARSERS)
GRMOBJECTS=grmaker.o graphmaker.o $(COMMON)
#GRAOBJECTS=granalyse.o componentmaker.o $(COMMON)
GRAOBJECTS=granalyse.o componentmaker.o $(COMMON)

# Speaking of build times; this rule is murder: a changed hh-file recompiles everything
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
	./codeext
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

this: all
	./codeext cfgFile=this.cfg
	./grmaker cfgFile=this.cfg

clean:
	rm *.o
	rm *~
	rm codeext
	rm granalyse
	rm grmaker
