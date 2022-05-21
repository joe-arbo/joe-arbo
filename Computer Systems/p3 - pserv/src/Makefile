DEP_BASE_DIR=../deps
DEP_INCLUDE_DIR=$(DEP_BASE_DIR)/include
DEP_LIB_DIR=$(abspath $(DEP_BASE_DIR)/lib)

CFLAGS=-g -O2 -pthread -Wall -Werror -Wmissing-prototypes -I$(DEP_INCLUDE_DIR)

# include lib directory into runtime path to facilitate dynamic linking
LDFLAGS=-pthread -Wl,-rpath -Wl,$(DEP_LIB_DIR)
LDLIBS=-L$(DEP_LIB_DIR) -ljwt -ljansson -lcrypto -ldl

HEADERS=socket.h http.h hexdump.h buffer.h bufio.h threadpool.h list.h
OBJ=main.o socket.o hexdump.o http.o bufio.o threadpool.o list.o


OTHERS=jwt_demo_rs256 jwt_demo_hs256

all:    server $(OTHERS)

$(OBJ) : $(HEADERS)

server: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS) 

clean:
	/bin/rm -f $(OBJ) $(OTHERS) server
