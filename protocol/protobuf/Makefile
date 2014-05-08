SRC=pb.c

TARGET=pb.so
CFLAGS=`pkg-config --cflags lua5.2`
LDFLAGS=`pkg-config --libs lua5.2`

all:$(TARGET)

$(TARGET):$(SRC)
	gcc -O3 -shared -fPIC $(SRC) $(CFLAGS) $(LDFLAGS) -o $@
