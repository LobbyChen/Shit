package main

/*
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>

typedef struct {
    HANDLE in;
    HANDLE out;
} Console;

Console* console_open() {
    Console* c = (Console*)malloc(sizeof(Console));
    if (c == NULL) {
        return NULL;
    }

    c->in  = GetStdHandle(STD_INPUT_HANDLE);
    c->out = GetStdHandle(STD_OUTPUT_HANDLE);

    if (c->in == INVALID_HANDLE_VALUE ||
        c->out == INVALID_HANDLE_VALUE) {
        free(c);
        return NULL;
    }

    return c;
}

int console_read(Console* c, char* buf, DWORD size, DWORD* n) {
    return ReadFile(c->in, buf, size, n, NULL);
}

int console_write(Console* c, const char* buf, DWORD size) {
    DWORD written = 0;

    return WriteFile(
        c->out,
        buf,
        size,
        &written,
        NULL
    );
}

void console_close(Console* c) {
    if (c != NULL) {
        free(c);
    }
}
*/
import "C"

import (
	"os"
	"unsafe"
)

// 整数解析器
func parseInt(data []byte, pos *int) int64 {
	for *pos < len(data) &&
		(data[*pos] == ' ' ||
			data[*pos] == '\n' ||
			data[*pos] == '\r' ||
			data[*pos] == '\t') {
		*pos++
	}

	sign := int64(1)

	if *pos < len(data) && data[*pos] == '-' {
		sign = -1
		*pos++
	}

	var value int64

	for *pos < len(data) {
		ch := data[*pos]

		if ch < '0' || ch > '9' {
			break
		}

		value = value*10 + int64(ch-'0')
		*pos++
	}

	return value * sign
}

func main() {
	console := C.console_open()

	if console == nil {
		os.Exit(1)
	}

	defer C.console_close(console)

	buffer := make([]byte, 4096)

	var read C.DWORD

	ok := C.console_read(
		console,
		(*C.char)(unsafe.Pointer(&buffer[0])),
		C.DWORD(len(buffer)),
		&read,
	)

	if ok == 0 {
		os.Exit(1)
	}

	data := buffer[:int(read)]

	pos := 0

	a := parseInt(data, &pos)
	b := parseInt(data, &pos)
	c := parseInt(data, &pos)

	left := 2*a + 2*b
	right := 2 * c

	var output []byte

	if left > right {
		output = []byte("Good\r\n")
	} else {
		output = []byte("Bad\r\n")
	}

	C.console_write(
		console,
		(*C.char)(unsafe.Pointer(&output[0])),
		C.DWORD(len(output)),
	)
}
