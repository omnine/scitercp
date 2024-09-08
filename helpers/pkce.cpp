#pragma once

#include <string>
#include <random>

#include <openssl/sha.h>
#include "base64.h"

class PKCE {
public:
    // Constructor
    PKCE() {
        // Initialize class members here
    }

    // Destructor
    ~PKCE() {
        // Clean up resources here
    }

    // Member function
    std::string random_string(std::size_t length)
    {
        const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

        std::random_device random_device;
        std::mt19937 generator(random_device());
        std::uniform_int_distribution<> distribution(0, characters.size() - 1);

        std::string random_string;

        for (std::size_t i = 0; i < length; ++i)
        {
            random_string += characters[distribution(generator)];
        }

        return random_string;
    }

    void generate_code_verifier() {
        code_verifier = random_string(64);
    }

    std::string generate_code_challenge() {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, code_verifier.c_str(), code_verifier.size());
        SHA256_Final(hash, &sha256);

        return base64_encode(sha256, SHA256_DIGEST_LENGTH, true);

    }

private:
    // Private members
    int myVariable;

    std::string code_verifier;
};