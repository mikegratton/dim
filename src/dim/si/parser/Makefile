.PHONY: all
all : ../quantity.lex.cpp ../quantity.tab.cpp

../quantity.lex.cpp: quantity.l quantity.y
	flex --header-file=../quantity.lex.hpp -o ../quantity.lex.cpp quantity.l

../quantity.tab.cpp: quantity.y
	bison --report=all --defines=../quantity.tab.hpp -o ../quantity.tab.cpp quantity.y

.PHONY: clean
clean:
	rm -f ../quantity.tab.hpp ./quantity.tab.cpp ../quantity.lex.cpp ../quantity.lex.hpp


