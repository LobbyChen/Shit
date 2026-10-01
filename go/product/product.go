package main

/*
#cgo windows LDFLAGS: -static-libgcc -static-libstdc++
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include "eigen_compare.h"

typedef struct {
    HANDLE in;
    HANDLE out;
} Console;

Console* open_console() {
    Console* c = (Console*)malloc(sizeof(Console));
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
    if (c) free(c);
}
*/
import "C"
import (
	"os"
	"strconv"
	"unsafe"
)

func normalize(a []byte) []byte {
	if len(a) == 0 {
		return []byte{0}
	}
	for len(a) > 1 && a[len(a)-1] == 0 {
		a = a[:len(a)-1]
	}
	return a
}

func decimalToBinary(s []byte) []byte {
	result := []byte{0}
	for _, ch := range s {
		if ch < '0' || ch > '9' {
			continue
		}
		digit := byte(ch - '0')
		result = multiplySmall(result, 10)
		if digit != 0 {
			result = addSmall(result, digit)
		}
	}
	return normalize(result)
}

func multiplySmall(a []byte, x byte) []byte {
	if x == 0 {
		return []byte{0}
	}
	result := make([]byte, 0, len(a)+8)
	var carry byte
	for _, bit := range a {
		v := bit*x + carry
		result = append(result, v&1)
		carry = v >> 1
	}
	for carry > 0 {
		result = append(result, carry&1)
		carry >>= 1
	}
	return normalize(result)
}

func addSmall(a []byte, x byte) []byte {
	result := make([]byte, len(a))
	copy(result, a)
	carry := x
	for i := 0; i < len(result) && carry > 0; i++ {
		v := result[i] + carry
		result[i] = v & 1
		carry = v >> 1
	}
	for carry > 0 {
		result = append(result, carry&1)
		carry >>= 1
	}
	return normalize(result)
}

func binaryAdd(a, b []byte) []byte {
	a = normalize(a)
	b = normalize(b)
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
		carry = sum >> 1
	}
	if carry != 0 {
		result = append(result, carry)
	}
	return normalize(result)
}

func shiftLeft(a []byte, count int) []byte {
	if len(a) == 0 || (len(a) == 1 && a[0] == 0) {
		return []byte{0}
	}
	result := make([]byte, len(a)+count)
	copy(result[count:], a)
	return result
}

func binaryMultiply(a, b []byte) []byte {
	a = normalize(a)
	b = normalize(b)
	if isZero(a) || isZero(b) {
		return []byte{0}
	}
	result := []byte{0}
	for i, bit := range b {
		if bit == 1 {
			shifted := shiftLeft(a, i)
			result = binaryAdd(result, shifted)
		}
	}
	return normalize(result)
}

func binarySubtract(a, b []byte) []byte {
	a = normalize(a)
	b = normalize(b)
	result := make([]byte, len(a))
	var borrow byte
	for i := 0; i < len(a); i++ {
		x := a[i]
		var y byte
		if i < len(b) {
			y = b[i]
		}
		if x < y+borrow {
			result[i] = x + 2 - y - borrow
			borrow = 1
		} else {
			result[i] = x - y - borrow
			borrow = 0
		}
	}
	return normalize(result)
}

func isZero(a []byte) bool {
	a = normalize(a)
	return len(a) == 1 && a[0] == 0
}

func binaryToDecimalString(a []byte) string {
	a = normalize(a)
	if isZero(a) {
		return "0"
	}
	decimal := []byte{'0'}
	for i := len(a) - 1; i >= 0; i-- {
		carry := byte(0)
		for j := 0; j < len(decimal); j++ {
			v := (decimal[j]-'0')*2 + carry
			decimal[j] = v%10 + '0'
			carry = v / 10
		}
		if carry != 0 {
			decimal = append(decimal, carry+'0')
		}
		if a[i] == 1 {
			carry = 1
			for j := 0; j < len(decimal); j++ {
				v := (decimal[j] - '0') + carry
				decimal[j] = v%10 + '0'
				carry = v / 10
				if carry == 0 {
					break
				}
			}
			if carry != 0 {
				decimal = append(decimal, carry+'0')
			}
		}
	}
	for i, j := 0, len(decimal)-1; i < j; i, j = i+1, j-1 {
		decimal[i], decimal[j] = decimal[j], decimal[i]
	}
	return string(decimal)
}

func binaryCompare(a []byte, b []byte) int {
	a = normalize(a)
	b = normalize(b)
	if len(a) == 0 {
		a = []byte{0}
	}
	if len(b) == 0 {
		b = []byte{0}
	}
	return int(C.eigen_compare_binary(
		(*C.uchar)(unsafe.Pointer(&a[0])),
		C.size_t(len(a)),
		(*C.uchar)(unsafe.Pointer(&b[0])),
		C.size_t(len(b)),
	))
}

func nextToken(data []byte, pos *int) []byte {
	for *pos < len(data) {
		ch := data[*pos]
		if ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t' {
			break
		}
		(*pos)++
	}
	start := *pos
	for *pos < len(data) {
		ch := data[*pos]
		if ch < '0' || ch > '9' {
			break
		}
		(*pos)++
	}
	return data[start:*pos]
}

func main() {
	c := C.open_console()
	if c == nil {
		return
	}
	defer C.close_console(c)

	buffer := make([]byte, 1024*1024)
	n := C.read_console(c, (*C.char)(unsafe.Pointer(&buffer[0])), C.DWORD(len(buffer)))
	if n == 0 {
		return
	}

	input := buffer[:int(n)]
	pos := 0

	nToken := nextToken(input, &pos)
	if len(nToken) == 0 {
		return
	}
	count, err := strconv.Atoi(string(nToken))
	if err != nil || count <= 0 {
		msg := []byte("Not Found\r\n")
		C.write_console(c, (*C.char)(unsafe.Pointer(&msg[0])), C.DWORD(len(msg)))
		return
	}

	hValues := make([]string, 0, count)
	for len(hValues) < count {
		token := nextToken(input, &pos)
		if len(token) == 0 {
			break
		}
		hValues = append(hValues, string(token))
	}

	if len(hValues) < count {
		msg := []byte("Not Found\r\n")
		C.write_console(c, (*C.char)(unsafe.Pointer(&msg[0])), C.DWORD(len(msg)))
		return
	}

	tToken := nextToken(input, &pos)
	if len(tToken) == 0 {
		msg := []byte("Not Found\r\n")
		C.write_console(c, (*C.char)(unsafe.Pointer(&msg[0])), C.DWORD(len(msg)))
		return
	}
	tStr := string(tToken)

	product := []byte{1}
	for i := 0; i < count; i++ {
		hiBin := decimalToBinary([]byte(hValues[i]))
		product = binaryMultiply(product, hiBin)
	}

	tBin := decimalToBinary([]byte(tStr))
	compareResult := binaryCompare(product, tBin)
	if compareResult >= 0 {
		msg := []byte("Not Found\r\n")
		C.write_console(c, (*C.char)(unsafe.Pointer(&msg[0])), C.DWORD(len(msg)))
		return
	}

	diff := binarySubtract(tBin, product)
	diff = binarySubtract(diff, []byte{1})

	if isZero(diff) {
		msg := []byte("Not Found\r\n")
		C.write_console(c, (*C.char)(unsafe.Pointer(&msg[0])), C.DWORD(len(msg)))
		return
	}

	resultStr := binaryToDecimalString(diff)
	output := []byte(resultStr + "\r\n")
	C.write_console(c, (*C.char)(unsafe.Pointer(&output[0])), C.DWORD(len(output)))

	_ = os.Args
}
