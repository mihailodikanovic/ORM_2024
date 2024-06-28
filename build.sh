CC=gcc
CFLAGS="-Wall"

SRC_DIR="src"
HEADER_DIR="include"

SRC="$SRC_DIR/server.c $SRC_DIR/server_functions.c"
OBJ="$SRC_DIR/server.o $SRC_DIR/server_functions.o"

EXECUTABLE="server"
rm -f $EXECUTABLE

HEADERS="$HEADER_DIR/server_functions.h $HEADER_DIR/common.h"

compile_and_link() {
    $CC -c $1 $CFLAGS -o ${1/.c/.o} -I$HEADER_DIR
}

for header_file in $HEADERS; do
    if [ ! -f $header_file ]; then
        echo "Error: Header file $header_file not found."
        exit 1
    fi
done

for source_file in $SRC; do
    compile_and_link $source_file
done

$CC $OBJ -o $EXECUTABLE

rm -f $SRC_DIR/server.o
rm -f $SRC_DIR/server_functions.o


CC=gcc
CFLAGS="-Wall"

SRC="$SRC_DIR/client.c $SRC_DIR/client_functions.c"
OBJ="$SRC_DIR/client.o $SRC_DIR/client_functions.o"

EXECUTABLE="client"
rm -f $EXECUTABLE

HEADERS="$HEADER_DIR/client_functions.h $HEADER_DIR/common.h"

compile_and_link() {
    $CC -c $1 $CFLAGS -o ${1/.c/.o} -I$HEADER_DIR
}

for header_file in $HEADERS; do
    if [ ! -f $header_file ]; then
        echo "Error: Header file $header_file not found."
        exit 1
    fi
done

for source_file in $SRC; do
    compile_and_link $source_file
done

$CC $OBJ -o $EXECUTABLE

rm -f $SRC_DIR/client_functions.o
rm -f $SRC_DIR/client.o

