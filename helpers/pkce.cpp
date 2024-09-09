#pragma once

#include <string>
#include <random>
#include <fstream>

#include "pkce.h"
#include <shlwapi.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <openssl/sha.h>
#include "base64.h"


PKCE g_pkce;
std::string PKCE::random_string(std::size_t length)
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

void PKCE::generate_code_verifier() {
    code_verifier = random_string(64);
}

std::string PKCE::generate_code_challenge() {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, code_verifier.c_str(), code_verifier.size());
    SHA256_Final(hash, &sha256);
    std::string full_encoded = base64_encode(hash, SHA256_DIGEST_LENGTH, true);

    // remove padding characters
    std::string result = full_encoded;
    result.erase(result.find_last_not_of('.') + 1);
    return result;
}

int PKCE::readSettings(const char *szFilePath)
{

    if (!PathFileExistsA(szFilePath))
        return 1;

    std::ifstream f(szFilePath);

    json config;
    try
    {
        config = json::parse(f);
    }
    catch (json::parse_error& ex)
    {
//        LOG_WARNING(logger, "json parse the file {} got the error {}",szFilePath, ex.what());
        return 3;
    }

    if (config.is_null())
        return 2;

    // find an entry
    if (config.contains("authorization_endpoint"))
    {
        authorization_endpoint = config["authorization_endpoint"];
    }

    if (config.contains("client_id"))
    {
        client_id = config["client_id"];
    }

    if (config.contains("native_redirect_url"))
    {
        native_redirect_url = config["native_redirect_url"];
    }

    if (config.contains("token_endpoint"))
    {
        token_endpoint = config["token_endpoint"];
    }    


    if (config.contains("bypass"))
    {
        bypass = config["bypass"];
    }    

    if (config.contains("log_level"))
    {
        log_level = config["log_level"];
    }

//    LOG_INFO(logger, "successfully read the settings from {}", szFilePath);

    return 0;
}

