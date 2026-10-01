package main

/*
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>

typedef struct {
    HANDLE in;
    HANDLE out;
} Console;

Console* open_console() {
    Console* c = malloc(sizeof(Console));

    if (!c) return NULL;

    c->in = GetStdHandle(STD_INPUT_HANDLE);
    c->out = GetStdHandle(STD_OUTPUT_HANDLE);

    return c;
}

DWORD read_console(Console* c, char* buf, DWORD size) {
    DWORD n = 0;
    ReadFile(c->in, buf, size, &n, NULL);
    return n;
}

void write_console(Console* c, const char* buf, DWORD size) {
    DWORD n;
    WriteFile(c->out, buf, size, &n, NULL);
}

void close_console(Console* c) {
    free(c);
}
*/
import "C"

import (
	"os"
	"unsafe"
)

func decimalToInt(s []byte) uint64 {
	var n uint64

	for _, ch := range s {
		if ch >= '0' && ch <= '9' {
			n = n*10 + uint64(ch-'0')
		}
	}

	return n
}

func toBinary(n uint64) []byte {
	if n == 0 {
		return []byte{0}
	}

	var bits []byte

	for n > 0 {
		bits = append(bits, byte(n&1))
		n >>= 1
	}

	return bits
}

func shiftLeft(bits []byte) []byte {
	result := make([]byte, len(bits)+1)

	for i := range bits {
		result[i+1] = bits[i]
	}

	return result
}
func binaryAdd(a, b []byte) []byte {
	n := len(a)

	if len(b) > n {
		n = len(b)
	}

	result := make([]byte, 0, n+1)

	var carry byte

	for i := 0; i < n; i++ {
		var x, y byte

		if i < len(a) {
			x = a[i]
		}

		if i < len(b) {
			y = b[i]
		}

		sum := x + y + carry

		result = append(result, sum&1)
		carry = (sum >> 1)
	}

	if carry != 0 {
		result = append(result, carry)
	}

	return result
}

func normalize(bits []byte) []byte {
	for len(bits) > 1 && bits[len(bits)-1] == 0 {
		bits = bits[:len(bits)-1]
	}

	return bits
}

func binaryCompare(a, b []byte) int {
	a = normalize(a)
	b = normalize(b)

	if len(a) > len(b) {
		return 1
	}

	if len(a) < len(b) {
		return -1
	}

	// 从最高位开始比较
	for i := len(a) - 1; i >= 0; i-- {
		if a[i] > b[i] {
			return 1
		}

		if a[i] < b[i] {
			return -1
		}
	}

	return 0
}

func main() {
	console := C.open_console()

	if console == nil {
		os.Exit(1)
	}

	defer C.close_console(console)

	buffer := make([]byte, 1024)

	n := C.read_console(
		console,
		(*C.char)(unsafe.Pointer(&buffer[0])),
		C.DWORD(len(buffer)),
	)

	input := buffer[:int(n)]

	var nums [3][]byte
	index := 0
	start := -1

	for i, ch := range input {
		if ch >= '0' && ch <= '9' {
			if start == -1 {
				start = i
			}
		} else if start != -1 {
			if index < 3 {
				nums[index] = input[start:i]
				index++
			}
			start = -1
		}
	}

	if start != -1 && index < 3 {
		nums[index] = input[start:]
		index++
	}

	if index != 3 {
		return
	}
	a := toBinary(decimalToInt(nums[0]))
	b := toBinary(decimalToInt(nums[1]))
	c := toBinary(decimalToInt(nums[2]))

	// 2a
	aa := shiftLeft(a)

	// 2b
	bb := shiftLeft(b)

	// 2c
	cc := shiftLeft(c)

	left := binaryAdd(aa, bb)

	var output []byte

	if binaryCompare(left, cc) > 0 {
		output = []byte("Good\r\n")
	} else {
		output = []byte("Bad\r\n")
	}

	C.write_console(
		console,
		(*C.char)(unsafe.Pointer(&output[0])),
		C.DWORD(len(output)),
	)
}
