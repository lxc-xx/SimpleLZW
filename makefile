lzw : lzw.o dic.o
	gcc -o lzw lzw.c dic.o -g
dic.o : dic.h
	gcc -o dic.o dic.c -g -c 
clean : 
	rm dic.o
	rm lzw.o
	rm lzw
