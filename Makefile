rofs: rofs.c
	gcc rofs.c -o rofs -Wall -ansi -W -std=c99 -g -ggdb -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -lfuse

mount:
	./rofs -d test1 test2

clean:
	rm rofs

umount:
	fusermount -u test2

remount: 
	gcc rofs.c -o rofs -Wall -ansi -W -std=c99 -g -ggdb -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -lfuse
	fusermount -u test2
	./rofs -d test1 test2 &
