website: website.c
	gcc website.c -o website -Wall -ansi -W -std=c99 -g -ggdb -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -lfuse -ltidy

mount:
	./website -d test1 test2

clean:
	rm website

umount:
	fusermount -u test2

remount: 
	gcc website.c -o website -Wall -ansi -W -std=c99 -g -ggdb -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -lfuse -ltidy
	fusermount -u test2
	./website -d test1 test2 &
