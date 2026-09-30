#include "RequiredFunctionsHW3.c"

int main(int argc, char *argv[]) {
    //read the seed
    int seed_length;
    unsigned char *seed = Read_File(argv[1], &seed_length);

    //read the messages
    //NOTE: required args are defined in provided Requiredfunctions
    unsigned char messages[NUMBER_OF_MESSAGES][LENGTH_OF_EACH_MESSAGE];
    Read_Multiple_Lines_from_File(argv[2], messages, NUMBER_OF_MESSAGES);

    //create the first key
    unsigned char key[SHA256_DIGEST_LENGTH];
    SHA256(seed, seed_length, key);

    //buffer to store all keys to be written to keys.txt
    // char all_keys[NUMBER_OF_MESSAGES * (2 * SHA256_DIGEST_LENGTH + 1)];

    FILE *keys_txt = fopen("Keys.txt", "w");
    FILE *cipher_txt = fopen("Ciphertexts.txt", "w");

    for (int i = 0; i < NUMBER_OF_MESSAGES; i++) {
        unsigned char ciphertext[LENGTH_OF_EACH_MESSAGE];

        //store key - convert to hex, terminate, put in keys_txt file
        char key_hex[65];
        Convert_to_Hex(key_hex, key, 32);
        key_hex[64] = '\0';

        fputs(key_hex, keys_txt);
        if (i+1 < NUMBER_OF_MESSAGES)
            fputc('\n', keys_txt);

        //encrypt with current key
        AES256CTR_Encrypt(key,messages[i], LENGTH_OF_EACH_MESSAGE, ciphertext);

        //store ciphertext - convert to hex, terminate, newline between lines, append
        char ciphertext_hex[129];
        Convert_to_Hex(ciphertext_hex, ciphertext, LENGTH_OF_EACH_MESSAGE);
        ciphertext_hex[128] = '\0';
        fputs(ciphertext_hex, cipher_txt);
        if (i + 1 < NUMBER_OF_MESSAGES)
            fputc('\n', cipher_txt);

        //spawn new key from old key
        unsigned char new_key[SHA256_DIGEST_LENGTH]; 
        SHA256(key, sizeof(key), new_key);
        memcpy(key, new_key, sizeof(key));
    }

    fclose(keys_txt);
    fclose(cipher_txt);

    //
    free(seed);

    return 0;
}
