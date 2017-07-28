CPP     	= 	g++ -O3  -std=c++1z -fexceptions -D__GLIBCXX_TYPE_INT_N_0=__int128 -D__GLIBCXX_BITSIZE_INT_N_0=128
STRIPPER 	= 	./removecomments.pl
EMCC    	=       ~/Extern/emscripten3/emscripten/emcc -Wc++11-extensions -std=c++11
OPT             = 	 -s PRECISE_I64_MATH=1
SET     	= 	-O2 -s ALLOW_MEMORY_GROWTH=1 -s ASM_JS=0
LIBPATH 	= 
SYSLIBS 	= 	-lstdc++ -lboost_regex -lboost_thread -lboost_system -lboost_date_time -lrt  -lpthread -lm
PRGS		=	brucelied


all: le_build brucelied lie_test

lie_test: le_build/lie_test.o
	$(CPP) -fexceptions -D_BOOL  $^ -I$(INCPATH) -L$(LIBPATH) $(SYSLIBS) -o $@
le_build/lie_test.o: lie_test.cpp spaces.h algebra.h math_helpers.h lie.h
	$(CPP) -fexceptions -D_BOOL -c $< -I$(INCPATH) -L$(LIBPATH)  -DHAVE_IOMANIP -DHAVE_IOSTREAM -DHAVE_LIMITS_H -o $@

brucelied: le_build/brucelied.o
	$(CPP) -fexceptions -D_BOOL  $^ -I$(INCPATH) -L$(LIBPATH) $(SYSLIBS) -o $@

clean:
	rm -rf le_build brucelied lie_test;\

le_build/brucelied.o: brucelied.cpp brucelied.h simple_httpd.h spaces.h algebra.h math_helpers.h lie.d
	$(CPP) -fexceptions -D_BOOL -c $< -I$(INCPATH) -L$(LIBPATH)  -DHAVE_IOMANIP -DHAVE_IOSTREAM -DHAVE_LIMITS_H -o $@

le_build:
	mkdir ./le_build

brucelieclient-opt.js: brucelieclient.js
	closure-compiler --language_in=ECMASCRIPT5  --compilation_level ADVANCED_OPTIMIZATIONS --js $< --warning_level QUIET > $@ && cp ./$@ ./data/
brucelieclient.js: main.js webglfoo.js lie-stripped.js LA_helpers.js
	cat $^ > $@ && cp ./$@ ./data/

lie.js: lie.sym lie.cpp lie.h
	$(EMCC) $(OPT) $<  -o $@ -s EXPORTED_FUNCTIONS=$(shell cat $^) $(SET)
lie-stripped.js: lie.js
	$(STRIPPER) $^ > $@
lie.sym: lie.cpp
symbols=\"[`awk '$$1 ~/EMCEXPORT/{sub(/\(.*/,"");printf "\x27_"$$3"\x27,"}' $<`]\";echo $$symbols > $@; echo "Functions to export: " $$symbols;


