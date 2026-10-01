#include "RequiredFunctionsHW3.c"

int main(int argc, char *argv[]) {
    //read the shared seed and create the first key
    int seed_length;
    unsigned char *seed = Read_File(argv[1], &seed_length);
    unsigned char key[SHA256_DIGEST_LENGTH];
    SHA256(seed, seed_length, key);

    FILE *cipher_txt = fopen(argv[2], "r");
    FILE *plain_txt = fopen("Plaintexts.txt", "w");

    for (int i = 0; i < NUMBER_OF_MESSAGES; i++) {
        //one ciphertext is 64 bytes, stored as 128 hex characters
        char ciphertext_hex[130];
        unsigned char ciphertext[LENGTH_OF_EACH_MESSAGE];
        unsigned char plaintext[LENGTH_OF_EACH_MESSAGE];

        fgets(ciphertext_hex, sizeof(ciphertext_hex), cipher_txt);
        for (int j = 0; j < LENGTH_OF_EACH_MESSAGE; j++)
            sscanf(ciphertext_hex + 2 * j, "%2hhx", &ciphertext[j]);

        //decrypt with the current key and write one 64-byte message
        AES256CTR_Decrypt(key, ciphertext, LENGTH_OF_EACH_MESSAGE, plaintext);
        fwrite(plaintext, 1, LENGTH_OF_EACH_MESSAGE, plain_txt);
        if (i + 1 < NUMBER_OF_MESSAGES)
            fputc('\n', plain_txt);

        //spawn the next key from the one just used
        unsigned char new_key[SHA256_DIGEST_LENGTH];
        SHA256(key, sizeof(key), new_key);
        memcpy(key, new_key, sizeof(key));
    }

    fclose(cipher_txt);
    fclose(plain_txt);
    free(seed);
    return 0;
}
