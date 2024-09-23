mytar: main.o create.o listings.o extract.o
	gcc -o mytar -Wall -g main.o listings.o header.h create.o extract.o

main.o: main.c
	gcc -c -Wall -g main.c header.h

extract.o: extract.c
	gcc -c -Wall -g extract.c header.h

listings.o: listings.c
	gcc -c -Wall -g listings.c header.h

create.o: create.c
	gcc -c -Wall -g create.c header.h

clean:
	rm *.o
