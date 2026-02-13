#ifndef ENCRY_H
#define ENCRY_H
#include <openssl/rand.h>
#include <openssl/evp.h>

#include <vector>
#include <iostream>
#include <cstring>




bool encrypt_packet(
    const uint8_t* plaintext,
    const size_t plaintext_size,
    const std::vector<uint8_t>& key,
    const uint8_t* nonce,
    uint8_t* ciphertext,
    uint8_t* tag);


bool decrypt_packet(
    const uint8_t* ciphertext,
    size_t ciphertext_size,
    const std::vector<unsigned char>& key,
    const uint8_t* nonce,
    const uint8_t* tag,
    uint8_t* plaintext);


#endif // ENCRY_H
