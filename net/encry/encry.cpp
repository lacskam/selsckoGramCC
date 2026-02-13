#include "encry.h"

bool encrypt_packet(
    const uint8_t* plaintext,
    const size_t plaintext_size,
    const std::vector<uint8_t>& key,
    const uint8_t* nonce,
    uint8_t* ciphertext,
    uint8_t* tag)
{
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (EVP_EncryptInit_ex(ctx, EVP_chacha20_poly1305(), NULL, NULL, NULL) != 1)
        return false;

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, NULL);

    if (EVP_EncryptInit_ex(ctx, NULL, NULL, key.data(), nonce) != 1)
        return false;

    int len;

    if (EVP_EncryptUpdate(ctx, ciphertext, &len,
                          plaintext, plaintext_size) != 1)
        return false;

    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1)
        return false;

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, (void*)tag);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool decrypt_packet(
    const uint8_t* ciphertext,
    size_t ciphertext_size,
    const std::vector<unsigned char>& key,
    const uint8_t* nonce,
    const uint8_t* tag,
    uint8_t* plaintext)
{
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (EVP_DecryptInit_ex(ctx, EVP_chacha20_poly1305(), NULL, NULL, NULL) != 1)
        return false;

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, NULL);

    if (EVP_DecryptInit_ex(ctx, NULL, NULL, key.data(), nonce) != 1)
        return false;


    int len;

    if (EVP_DecryptUpdate(ctx, plaintext, &len,
                          ciphertext, ciphertext_size) != 1)
        return false;

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, 16, (void*)tag);

    if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) <= 0)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

