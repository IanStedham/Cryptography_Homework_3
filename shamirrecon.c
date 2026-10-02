#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // for uint64_t
#include <string.h> // for memcpy()
#include <unistd.h> // for ssize_t
#include <openssl/sha.h> // for SHA256()
#include <openssl/evp.h> 

#define LENGTH_OF_EACH_MESSAGE 64

unsigned char* Read_File (char fileName[], int *fileLen);
void Read_Multiple_Lines_from_File(char fileName[], unsigned char message[][LENGTH_OF_EACH_MESSAGE], int num);
void Write_File(char fileName[], char input[]);
uint64_t str_to_uint64(const unsigned char *str, int str_len);
void uint64_to_str(uint64_t value, unsigned char *output, int output_len);
int64_t euclid_gcd(int64_t a, int64_t b);
int64_t mul_mod(int64_t a, int64_t b, int64_t m);
int64_t square_multiply(int64_t base, int64_t exponent, int64_t modulo);
int64_t extended_euclid(int64_t number, int64_t modulo);
int64_t mod_ensure_positive_result(int64_t num, int64_t mod);

int main(int argc, char *argv[]) { 
    int modulos_length;
    unsigned char* modulos_string = Read_File(argv[1], &modulos_length);
    uint64_t modulos = str_to_uint64(modulos_string, modulos_length);
    printf("Finished modulos\n");

    unsigned char share1_string[2][LENGTH_OF_EACH_MESSAGE];
    Read_Multiple_Lines_from_File("Share1.txt", share1_string, 2);
    uint64_t share1_x = str_to_uint64(share1_string[0], LENGTH_OF_EACH_MESSAGE);
    uint64_t share1_y = str_to_uint64(share1_string[1], LENGTH_OF_EACH_MESSAGE);

    unsigned char share2_string[2][LENGTH_OF_EACH_MESSAGE];
    Read_Multiple_Lines_from_File("Share2.txt", share2_string, 2);
    uint64_t share2_x = str_to_uint64(share2_string[0], LENGTH_OF_EACH_MESSAGE);
    uint64_t share2_y = str_to_uint64(share2_string[1], LENGTH_OF_EACH_MESSAGE);

    unsigned char share3_string[2][LENGTH_OF_EACH_MESSAGE];
    Read_Multiple_Lines_from_File("Share3.txt", share3_string, 2);
    uint64_t share3_x = str_to_uint64(share3_string[0], LENGTH_OF_EACH_MESSAGE);
    uint64_t share3_y = str_to_uint64(share3_string[1], LENGTH_OF_EACH_MESSAGE);

    unsigned char share4_string[2][LENGTH_OF_EACH_MESSAGE];
    Read_Multiple_Lines_from_File("Share4.txt", share4_string, 2);
    uint64_t share4_x = str_to_uint64(share4_string[0], LENGTH_OF_EACH_MESSAGE);
    uint64_t share4_y = str_to_uint64(share4_string[1], LENGTH_OF_EACH_MESSAGE);

    unsigned char share5_string[2][LENGTH_OF_EACH_MESSAGE];
    Read_Multiple_Lines_from_File("Share5.txt", share5_string, 2);
    uint64_t share5_x = str_to_uint64(share5_string[0], LENGTH_OF_EACH_MESSAGE);
    uint64_t share5_y = str_to_uint64(share5_string[1], LENGTH_OF_EACH_MESSAGE);


    /*
    since evaluating at 0, numerator and denominator can be calculated immediately. Will reevaluate after meeting with TA
    this can be reformatted into a for loop easily once I know which shares should be used 
    reconalt not working on my laptop, will also ask TA about this, theres a note that this might happen
    use different shares
    */

    // larange bias for share 1 evaluated at 0
    uint64_t numerator_1 = mul_mod(share2_x, share3_x, modulos);
    uint64_t denominator_1 = mul_mod((share1_x - share2_x), (share1_x - share3_x), modulos);
    uint64_t denominator_inverse_1 = extended_euclid(denominator_1, modulos);
    uint64_t larange_bias_1 = mul_mod(numerator_1, denominator_inverse_1, modulos);
    printf("\nnumerator_1: %ld\n", numerator_1);
    printf("denominator_1: %ld\n", denominator_1);
    printf("denominator_inverse_1: %ld\n", denominator_inverse_1);
    printf("larange_bias_1: %ld\n", larange_bias_1);

    // larange bias for share 2 evaluated at 0
    uint64_t numerator_2 = mul_mod(share1_x, share3_x, modulos);
    uint64_t denominator_2 = mul_mod((share2_x - share1_x), (share2_x - share3_x), modulos);
    uint64_t denominator_inverse_2 = extended_euclid(denominator_2, modulos);
    uint64_t larange_bias_2 = mul_mod(numerator_2, denominator_inverse_2, modulos);
    printf("\nnumerator_2: %ld\n", numerator_2);
    printf("denominator_2: %ld\n", denominator_2);
    printf("denominator_inverse_2: %ld\n", denominator_inverse_2);
    printf("larange_bias_2: %ld\n", larange_bias_2);

    // larange bias for share 3 evaluated at 0
    uint64_t numerator_3 = mul_mod(share1_x, share2_x, modulos);
    uint64_t denominator_3 = mul_mod((share3_x - share1_x), (share3_x - share2_x), modulos);
    uint64_t denominator_inverse_3 = extended_euclid(denominator_3, modulos);
    uint64_t larange_bias_3 = mul_mod(numerator_3, denominator_inverse_3, modulos);
    printf("\nnumerator_3: %ld\n", numerator_3);
    printf("denominator_3: %ld\n", denominator_3);
    printf("denominator_inverse_3: %ld\n", denominator_inverse_3);
    printf("larange_bias_3: %ld\n", larange_bias_3);

    uint64_t larange_bias_mul_share_y_1 = mul_mod(share1_y, larange_bias_1, modulos);
    uint64_t larange_bias_mul_share_y_2 = mul_mod(share2_y, (larange_bias_2*-1), modulos);
    uint64_t larange_bias_mul_share_y_3 = mul_mod(share3_y, larange_bias_3, modulos);
    printf("\nlarange_bias_mul_share_y_1: %ld\n", larange_bias_mul_share_y_1);
    printf("larange_bias_mul_share_y_2: %ld\n", larange_bias_mul_share_y_2);
    printf("larange_bias_mul_share_y_3: %ld\n", larange_bias_mul_share_y_3);

    uint64_t secret_reconstructed = mod_ensure_positive_result(larange_bias_mul_share_y_1 + larange_bias_mul_share_y_2 + larange_bias_mul_share_y_3, modulos);
    //secret_reconstructed = secret_reconstructed % modulos;
    if (secret_reconstructed < 0) {
        secret_reconstructed = secret_reconstructed + modulos;
    }
    printf("\nsecret_reconstructed: %ld\n", secret_reconstructed);

    char secret_reconstructed_string[LENGTH_OF_EACH_MESSAGE];
    uint64_to_str(secret_reconstructed, secret_reconstructed_string, LENGTH_OF_EACH_MESSAGE);
    Write_File("Recovered.txt", secret_reconstructed_string);
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

/*=======================================
        Read Multiple Lines from File
========================================*/
//*** This function has a fixed output length (LENGTH_OF_EACH_MESSAGE) and takes the number of lines to read as an argument (ensure that array is of sufficient size)
//*** If necessary, change the output length accordingly (it can read lengths smaller than the specified size just fine)
void Read_Multiple_Lines_from_File(char fileName[], unsigned char message[][LENGTH_OF_EACH_MESSAGE], int num)
{
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    int line_count = 0;
    ssize_t line_size;
    FILE *fp = fopen(fileName, "r");
    if (!fp)
        fprintf(stderr, "Error opening file '%s'\n", fileName);

    line_size = getline(&line_buf, &line_buf_size, fp);
    for(int j=0; line_size >= 0 && j < num; j++)
    {
        // Trim newline
        if (line_size > 0 && line_buf[line_size-1] == '\n')
            line_buf[--line_size] = '\0';

        memset(message[j], 0, LENGTH_OF_EACH_MESSAGE);

        // Copy up to LENGTH_OF_EACH_MESSAGE or line_size, whichever is smaller
        int copy_len = line_size < LENGTH_OF_EACH_MESSAGE ? line_size : LENGTH_OF_EACH_MESSAGE;
        memcpy(message[j], line_buf, copy_len);

        // Debug print, set print format length (*) to max LENGTH_OF_EACH_MESSAGE otherwise printf overruns with strings missing null terminator (which is most of the time) 
        printf("Message%d (%ld) == %.*s\n", j+1, line_size, LENGTH_OF_EACH_MESSAGE, message[j]);

        line_size = getline(&line_buf, &line_buf_size, fp);
    }

    free(line_buf);
    fclose(fp);
}

/*============================
        Write to File
==============================*/
void Write_File(char fileName[], char input[]) {
    FILE *pFile;
    pFile = fopen(fileName,"w");
    if (pFile == NULL){
        printf("Error opening file. \n");
        exit(0);
    }
    fputs(input, pFile);
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

int64_t mod_ensure_positive_result(int64_t num, int64_t mod) {
    int64_t result = num % mod;
    if (result < 0) {
        return result + mod;
    }
    else {
        return result;
    }
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