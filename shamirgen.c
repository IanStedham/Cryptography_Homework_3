#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // for uint64_t
#include <string.h> // for memcpy()
#include <unistd.h> // for ssize_t
#include <openssl/sha.h> // for SHA256()
#include <openssl/evp.h> 

#define LENGTH_OF_EACH_MESSAGE 64
#define N_SHARES 5
#define THRESHOLD 3

unsigned char* Read_File (char fileName[], int *fileLen);
void Write_Multiple_Lines_to_File(char fileName[], char input[][LENGTH_OF_EACH_MESSAGE], int num);
uint64_t str_to_uint64(const unsigned char *str, int str_len);
void uint64_to_str(uint64_t value, unsigned char *output, int output_len);
int64_t euclid_gcd(int64_t a, int64_t b);
int64_t mul_mod(int64_t a, int64_t b, int64_t m);
int64_t square_multiply(int64_t base, int64_t exponent, int64_t modulo);
int64_t extended_euclid(int64_t number, int64_t modulo);

int main(int argc, char *argv[]) { 
    int secret_length;
    unsigned char* secret_string = Read_File(argv[1], &secret_length);
    uint64_t secret = str_to_uint64(secret_string, secret_length);

    int modulos_length;
    unsigned char* modulos_string = Read_File(argv[2], &modulos_length);
    uint64_t modulos = str_to_uint64(modulos_string, modulos_length);

    uint64_t y_shares[N_SHARES];
    int coefficients[THRESHOLD];
    coefficients[0] = secret;
    for (int x = 1; x < THRESHOLD; x++) {
        int current_coefficient = rand(); 
        coefficients[x] = current_coefficient;
        printf("coefficient %d: %d\n", x, current_coefficient);
    }

    printf("\n");
    for (int x = 0; x < N_SHARES; x++) {
        uint64_t current_y_share = secret;
        for (int i = 1; i < THRESHOLD; i++) {
            uint64_t exponent_result = square_multiply((x+1), i, modulos);
            uint64_t mul_mod_result = mul_mod(coefficients[i], exponent_result, modulos);
            current_y_share += mul_mod_result;
        }
        y_shares[x] = current_y_share % modulos;
        printf("x share: %d, y_share: %ld\n", (x+1), y_shares[x]);
    }

    for (int x = 0; x < N_SHARES; x++) {
        char y_share_string[LENGTH_OF_EACH_MESSAGE];
        uint64_to_str(y_shares[x], y_share_string, LENGTH_OF_EACH_MESSAGE);
        char x_share_string[LENGTH_OF_EACH_MESSAGE];
        snprintf(x_share_string, sizeof(x_share_string), "%d", (x+1));

        char lines[2][LENGTH_OF_EACH_MESSAGE];
        strncpy(lines[0], x_share_string, LENGTH_OF_EACH_MESSAGE - 1);
        strncpy(lines[1], y_share_string, LENGTH_OF_EACH_MESSAGE - 1);

        char file_name[11];
        snprintf(file_name, sizeof(file_name), "Share%d.txt", (x+1));

        Write_Multiple_Lines_to_File(file_name, lines, 2);
    }
}

/*============================
        Read from File
==============================*/
unsigned char* Read_File (char fileName[], int *fileLen)
{
    FILE *pFile;
	pFile = fopen(fileName, "r");
	if (pFile == NULL)
	{
		printf("Error opening file.\n");
		exit(0);
	}
    fseek(pFile, 0L, SEEK_END);
    int temp_size = ftell(pFile)+1;
    fseek(pFile, 0L, SEEK_SET);
    unsigned char *output = (unsigned char*) malloc(temp_size);
	fgets(output, temp_size, pFile);
	fclose(pFile);

    *fileLen = temp_size-1;
	return output;
}

/*============================================s
        Write Multiple Lines to File
==============================================*/
//*** This function has a fixed input length (LENGTH_OF_EACH_MESSAGE) and takes the number of lines to write as an argument
//*** If necessary, change the input size accordingly (for lines smaller than LENGTH_OF_EACH_MESSAGE, fputs relies on the null terminator to know where EOL is)
//*** If you want to write "unsigned char" into a file, change the format of 'input' and 'temp' to unsinged char (or just cast it on call)
void Write_Multiple_Lines_to_File(char fileName[], char input[][LENGTH_OF_EACH_MESSAGE], int num) { 
    FILE *pFile;
    pFile = fopen(fileName,"w");
    if (pFile == NULL) {
        printf("Error opening file. \n");
        exit(0);
    }
    for(int i=0; i < num; i++) {
        char temp[LENGTH_OF_EACH_MESSAGE+1];
        temp[LENGTH_OF_EACH_MESSAGE] = '\0';
        memcpy(temp, input[i], LENGTH_OF_EACH_MESSAGE);
        fputs(temp, pFile);
        
        if (i < (num-1)) fputs("\n", pFile);
    }
    fclose(pFile);
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

// gcd - swapped with int64_t
int64_t euclid_gcd(int64_t a, int64_t b) {
    while (a != 0) {
        int64_t temp = a;
        a = b % a;
        b = temp;
    }
    return b;
}

//helper: multiplication/mod reduction
int64_t mul_mod(int64_t a, int64_t b, int64_t m) {
    return (a * b) % m;
}

// S & M - 
int64_t square_multiply(int64_t base, int64_t exponent, int64_t modulo) {
    int64_t result = 1;

    // count bits in exponent (replaces mpz_sizeinbase)
    int bit_len = 0;
    for (int64_t e = exponent; e > 0; e >>= 1)
        bit_len++;

    // most significant -> least significant, same loop as before
    for (int x = bit_len - 1; x >= 0; x--) {
        result = mul_mod(result, result, modulo);
        if ((exponent >> x) & 1)
            result = mul_mod(result, base, modulo);
    }
    return result;
}

//extended euclid - swapped gmp with int64_t 
int64_t extended_euclid(int64_t number, int64_t modulo) {
    int64_t remainder[256];
    int64_t quotient[256];
    int64_t x[256];
    int64_t y[256];

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

    int64_t inverse = x[n-1] % modulo;
    if (inverse < 0) {
        inverse += modulo;
    }
    return inverse; // meaningful only when *gcd_out == 1
}