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
        authorize_url = "https://login.microsoftonline.com/common/oauth2/v2.0/authorize";
        token_url = "https://login.microsoftonline.com/common/oauth2/v2.0/token";
        client_id = "d3590ed6-52b3-4102-aeff-aad2292ab01c";
        native_redirect_url = "ms-appx-web://microsoft.aad.brokerplugin/3ede5b24-7594-465f-9ec5-4b83dbd22b3e";
        bypass = false;
        log_level = 0;
    }

    // Destructor
    ~PKCE() {
        // Clean up resources here
    }

    // Member function
    std::string random_string(std::size_t length);


    void generate_code_verifier();
    std::string generate_code_challenge();

    int readSettings(const char *szFilePath);

    std::string login_name;

    std::string authorize_url;
    std::string token_url;
    std::string client_id;
    std::string native_redirect_url;
    std::string get_authorize_url(){
        return authorize_url;
    }
    std::string get_client_id(){
        return client_id;
    }

private:
    // Private members
    int myVariable;
    bool bypass;
    int log_level;

    std::string code_verifier;
};