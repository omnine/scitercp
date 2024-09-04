# Overview
--------
This sample demonstrates how to incorporate a windows(dialog) created with the [sciter library](https://sciter.com/) into the credential provider.

# How to build these samples
--------------------------------
The top-level directory contains a solution file, CredentialProviderSamples.sln, that can be opened in Visual Studio or built using msbuild.exe in the SDK command line environment.

# How to run this sample
--------------------------------
Each subfolder (for example, SampleCredentialProvider\ or SampleCredentialFilter\) contains a readme.txt file discussing how to run that sample.

Contents of this sample's subfolders:
-------------------
Helpers: common files for the other subfolders.

SampleWrapExistingCredentialProvider: demonstrates how a credential provider can "wrap" or contain another credential provider in order to add functionality.


[OAuth2 PKCE Flow](https://developer.constantcontact.com/api_guide/pkce_flow.html)  
[Proof Key for Code Exchange (RFC 7636)](https://www.authlete.com/developers/pkce/)  
[Enable login using the Authorization Code flow and PKCE for public clients](https://is.docs.wso2.com/en/6.1.0/guides/login/oidc-auth-code-pkce/)