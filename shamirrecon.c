#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // for uint64_t
#include <string.h> // for memcpy()
#include <unistd.h> // for ssize_t
#include <openssl/sha.h> // for SHA256()
#include <openssl/evp.h> 

unsigned char* Read_File (char fileName[], int *fileLen);
void Write_File(char fileName[], char input[]);
uint64_t str_to_uint64(const unsigned char *str, int str_len)
void uint64_to_str(uint64_t value, unsigned char *output, int output_len);
int128_t euclid_gcd(int128_t a, int128_t b);
int128_t mul_mod(int128_t a, int128_t b, int128_t m);
int128_t square_multiply(int128_t base, int128_t exponent, int128_t modulo);
int128_t extended_euclid(int128_t number, int128_t modulo);

int main(int argc, char *argv[]) { 
    int modulos_length;
    unsigned char* modulos_string = Read_File(argv[2], &modulos_length);
    uint64_t modulos = str_to_uint64(modulos_string, modulos_length);

}

// Convert decimal string to uint64_t
uint64_t str_to_uint64(const unsigned char *str, int str_len) {
    uint64_t result = 0;
    int negative = 0;
    if (*str == '-') {
        negative = 1;
        str++;
    }
    for (int i=0; i < str_len && *str >= '0' && *str <= '9'; i++) {
        result = result * 10 + (*str - '0');
        str++;
    }
    return negative ? -result : result;
}

// Convert uint64_t to decimal string
// Note: ensure that output array size is large enough including the null terminator at the end
void uint64_to_str(uint64_t value, unsigned char *output, int output_len) {
    int negative = value < 0;
    if (negative) 
        value = -value;

    // Not going to need more than 10 digits, make it 15 just to be safe
    char temp[15];
    int i = 0;

    // In reverse order, reversed at the end
    do {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0 && i < (int)sizeof(temp) - 1);

    if (negative) 
        temp[i++] = '-';

    if (i >= output_len) {
        output[0] = '\0';  // Not enough space
        return;
    }

    // Reverse the string into the output
    for (int j = 0; j < i; j++) {
        output[j] = temp[i - j - 1];
    }
    // Null-terminate
    output[i] = '\0';
}

// gcd - swapped with int128_t
int128_t euclid_gcd(int128_t a, int128_t b) {
    while (a != 0) {
        int128_t temp = a;
        a = b % a;
        b = temp;
    }
    return b;
}

//helper: multiplication/mod reduction
int128_t mul_mod(int128_t a, int128_t b, int128_t m) {
    return (a * b) % m;
}

// S & M - 
int128_t square_multiply(int128_t base, int128_t exponent, int128_t modulo) {
    int128_t result = 1;

    // count bits in exponent (replaces mpz_sizeinbase)
    int bit_len = 0;
    for (int128_t e = exponent; e > 0; e >>= 1)
        bit_len++;

    // most significant -> least significant, same loop as before
    for (int x = bit_len - 1; x >= 0; x--) {
        result = mul_mod(result, result, modulo);
        if ((exponent >> x) & 1)
            result = mul_mod(result, base, modulo);
    }
    return result;
}

//extended euclid - swapped gmp with int128_t 
int128_t extended_euclid(int128_t number, int128_t modulo) {
    int128_t remainder[256];
    int128_t quotient[256];
    int128_t x[256];
    int128_t y[256];

    remainder[0] = number;
    remainder[1] = modulo;
    x[0] = 1; x[1] = 0;
    y[0] = 0; y[1] = 1;
    int n = 1;

    while (remainder[n] != 0) {
        n++;
        remainder[n] = remainder[n-2] % remainder[n-1];
        quotient[n]  = remainder[n-2] / remainder[n-1];
        x[n] = x[n-2] - quotient[n] * x[n-1];
        y[n] = y[n-2] - quotient[n] * y[n-1];
    }

    int128_t inverse = x[n-1] % modulo;
    if (inverse < 0) {
        inverse += modulo;
    }
    return inverse; // meaningful only when *gcd_out == 1
}