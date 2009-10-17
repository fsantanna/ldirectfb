all:
	gcc -I /usr/local/include/directfb -O2 -fpic -c directfb.c
	gcc -I /usr/local/include/directfb -O2 -fpic -c IDirectFB.c
	gcc -I /usr/local/include/directfb -O2 -fpic -c IDirectFBDisplayLayer.c
	gcc -I /usr/local/include/directfb -O2 -fpic -c IDirectFBSurface.c
	gcc -I /usr/local/include/directfb -O2 -fpic -c IDirectFBWindow.c
	gcc -I /usr/local/include/directfb -O2 -fpic -c IDirectFBImageProvider.c
	gcc -I /usr/local/include/directfb -O2 -fpic -c IDirectFBVideoProvider.c
	gcc -I /usr/local/include/directfb -O2 -fpic -c IDirectFBFont.c
	gcc -I /usr/local/include/directfb -O2 -fpic -c IDirectFBEventBuffer.c
	gcc -O -shared -fpic -o directfb.so directfb.o -ldirectfb \
		IDirectFB.o \
	   	IDirectFBDisplayLayer.o \
	   	IDirectFBSurface.o \
	   	IDirectFBWindow.o \
	   	IDirectFBImageProvider.o \
	   	IDirectFBVideoProvider.o \
	   	IDirectFBFont.o \
	   	IDirectFBEventBuffer.o

clean:
	rm *.o
