#include <openssl/evp.h>
#include <openssl/rand.h>
#include "encrypt.h"

int encrypt_data(const unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext, int *ciphertext_len) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len_tmp;

    unsigned char key[32];
    unsigned char iv[16];

    RAND_bytes(key, sizeof(key));
    RAND_bytes(iv, sizeof(iv));

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, ciphertext, &len_tmp, plaintext, plaintext_len);
    *ciphertext_len = len_tmp;

    EVP_EncryptFinal_ex(ctx, ciphertext + len_tmp, &len_tmp);
    *ciphertext_len += len_tmp;

    EVP_CIPHER_CTX_free(ctx);
    return 0;
}
