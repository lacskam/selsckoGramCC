#include "encry.h"



EVP_PKEY* generate_x25519_key() {
    EVP_PKEY* key = nullptr;
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, nullptr);
    EVP_PKEY_keygen_init(ctx);
    EVP_PKEY_keygen(ctx, &key);
    EVP_PKEY_CTX_free(ctx);
    return key;
}

void generate_pub_key(uint8_t *key_pub, EVP_PKEY* local_key) {
    size_t len = 32;
    EVP_PKEY_get_raw_public_key(local_key, key_pub, &len);

}


std::vector<uint8_t> procces_pub_key(uint8_t *key_pub,EVP_PKEY* local_key) {
    EVP_PKEY* peer = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, nullptr, key_pub, 32);
    std::vector<uint8_t> local_secret = derive_shared_secret(local_key, peer);
    EVP_PKEY_free(peer);
    return local_secret;
}



std::vector<uint8_t> derive_shared_secret(EVP_PKEY* priv, EVP_PKEY* peer_pub) {
    std::vector<uint8_t> secret(32);
    size_t secret_len = secret.size();
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(priv, nullptr);
    EVP_PKEY_derive_init(ctx);
    EVP_PKEY_derive_set_peer(ctx, peer_pub);
    EVP_PKEY_derive(ctx, secret.data(), &secret_len);
    EVP_PKEY_CTX_free(ctx);
    return secret;
}


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

