#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // for uint64_t
#include <string.h> // for memcpy()
#include <unistd.h> // for ssize_t
#include <openssl/sha.h> // for SHA256()
#include <openssl/evp.h> 

unsigned char* Read_File (char fileName[], int *fileLen);
void Write_File(char fileName[], char input[]);
void AES128ECB_Encrypt(const unsigned char *key, const unsigned char *input, unsigned char *output);
void Convert_to_Hex(char output[], unsigned char input[], int inputlength);

int main(int argc, char *argv[]) { 
    int message_length;
    unsigned char* message = Read_File(argv[1], &message_length);

    int num_blocks;
    int need_padding = 0;
    if (message_length % 16 == 0) {
         num_blocks = message_length/16;
    }
    else {
        num_blocks = (message_length/16) + 1;
        need_padding = 1;
    }

    // building the blocks 
    unsigned char blocks[num_blocks][16];
    for (int x = 0; x < num_blocks; x++) {
        if (x == num_blocks-1 && need_padding) {
            int bytes_left = message_length % 16;
            strncpy(blocks[x], message + (16*x), bytes_left);
            for (int y = bytes_left; y < 16; y++) {
                blocks[x][y] = 0;
            }
        }
        else {
            strncpy(blocks[x], message + (16*x), 16);
        }
    }

    unsigned char hashes[num_blocks][32];
    unsigned char zeros_string[16] = {0}; // I tried using '0' but that resulted in the wrong output so i switchted to actual 0s
    for (int x = 0; x < num_blocks; x++) {
        unsigned char aes_output[16];
        if (x == 0) {
            AES128ECB_Encrypt(blocks[x], zeros_string, aes_output);
            for (int i = 0; i < 16; i++) {
                hashes[x][i] = aes_output[i] ^ zeros_string[i];
            }
        }
        else {
            AES128ECB_Encrypt(blocks[x], hashes[x-1], aes_output);
            for (int i = 0; i < 16; i++) {
                hashes[x][i] = aes_output[i] ^ hashes[x-1][i];
            }
        }
    }

    char first_hash_hex[33];
    Convert_to_Hex(first_hash_hex, hashes[0], 16);
    first_hash_hex[32] = '\0';
    Write_File("FirstHash.txt", first_hash_hex);

    char final_hash_hex[33];
    Convert_to_Hex(final_hash_hex, hashes[num_blocks-1], 16);
    final_hash_hex[32] = '\0';
    Write_File("FinalHash.txt", final_hash_hex);

    // printf("first hash: %s\n", first_hash_hex);
    // printf("final hash: %s\n", final_hash_hex);

    free(message);
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

/*==================================
    AES-128 ECB Encryption Function
====================================*/
/*--- Description:
*   Function uses AES-128 (so 128-bit input, 128-bit output, and 128-bit key)
*       ECB, so no chaining or IV, sufficient for a toy implementation
*       Explicitly set no padding, so input must be 16 bytes exactly
*/
void AES128ECB_Encrypt(const unsigned char *key, const unsigned char *input, unsigned char *output) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int outlen;

    EVP_EncryptInit_ex2(ctx, EVP_aes_128_ecb(), key, NULL, NULL);
    EVP_CIPHER_CTX_set_padding(ctx, 0);  // Disable padding

    EVP_EncryptUpdate(ctx, output, &outlen, input, 16);

    EVP_CIPHER_CTX_free(ctx);
}

/*============================
        Convert to Hex 
        Note: make sure output array size is double the size of input
==============================*/
void Convert_to_Hex(char output[], unsigned char input[], int inputlength)
{
    const char hex_digits[] = "0123456789abcdef";
    for (int i = 0; i < inputlength; i++) {
        output[2 * i] = hex_digits[(input[i] >> 4) & 0x0F]; // high nibble
        output[2 * i + 1] = hex_digits[input[i] & 0x0F]; // low nibble
    }
}