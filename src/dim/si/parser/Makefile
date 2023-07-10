.PHONY: all
all : ../quantity.tab.cpp

../quantity.tab.cpp: quantity.y
	bison --defines=../quantity.tab.hpp -o ../quantity.tab.cpp quantity.y

.PHONY: clean
clean:
	rm -f ../quantity.tab.hpp ./quantity.tab.cpp


