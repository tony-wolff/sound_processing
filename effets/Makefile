noisegate.so : noisegate.c
	gcc -fPIC -c noisegate.c
	ld -shared noisegate.o -o noisegate.so

amp.so : amplification.c
	gcc -fPIC -c amplification.c
	ld -shared amplification.o -o amp.so

vc.so : vocalremover.c
	gcc -fPIC -c vocalremover.c
	ld -shared vocalremover.o -o vc.so