// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_handler.h"

#include <windows.h>

#include <string>

#include "include/cef_browser.h"
#include "include/base/cef_logging.h"

#include "quill/LogMacros.h"
#include "quill/Logger.h"
extern quill::Logger* logger;

#include "../helpers/pkce.h"
extern PKCE g_pkce;

#include <iostream>

void SimpleHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser,
                                        const CefString& title) {
  CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
  if (hwnd) {
    SetWindowText(hwnd, std::wstring(title).c_str());
  }
}

void SetInputElementValue(CefRefPtr<CefBrowser> browser,
                          const std::string& elementId,
                          const std::string& value) {
  std::stringstream jsCode;
  jsCode << "document.getElementById('" << elementId << "').value = '" << value
         << "';";

  // Execute the JavaScript code in the main frame of the browser.
  browser->GetMainFrame()->ExecuteJavaScript(
      jsCode.str(), browser->GetMainFrame()->GetURL(), 0);
}


void SimpleHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              int httpStatusCode) {
  // Call your JavaScript injection code here
//  SetInputElementValue(browser, "i0118", "New Value");
  /*
  std::string script = R"(
                var checkExist = setInterval(function() {
                    let elements = document.getElementsByName("passwd");
                    console.log(elements.length);
                    if (elements.length > 0) {
                        elements[0].value = 'deep&net1';
                        document.forms['f1'].submit();
                        clearInterval(checkExist);
                    }
                }, 1000); // Check every 100ms
            )";

  // Execute the script
  frame->ExecuteJavaScript(script, frame->GetURL(), 0);  
  
  */



}


cef_return_value_t SimpleHandler::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback> callback) {

    std::string url = request->GetURL();
  std::cout << "current url: " << url << std::endl;
        // Check if the method is POST
  if (request->GetMethod() == "POST") {
    CefRefPtr<CefPostData> postData = request->GetPostData();
    if (postData) {
      CefPostData::ElementVector elements;
      postData->GetElements(elements);

      for (size_t i = 0; i < elements.size(); ++i) {
        CefRefPtr<CefPostDataElement> element = elements[i];
        if (element->GetType() == PDE_TYPE_BYTES) {
          size_t size = element->GetBytesCount();
          std::vector<char> bytes(size);
          element->GetBytes(size, bytes.data());

          // Convert bytes to a string for parsing
          std::string postDataStr(bytes.begin(), bytes.end());

          // Process the postDataStr here, for example:
          // Parse it as form data, JSON, etc.
          // For example, you could log it or pass it to the host application
          std::cout << "POST Data: " << postDataStr << std::endl;
        }
      }
    }
  }

  return RV_CONTINUE;
}


    // Implement OnResourceLoadComplete to monitor network requests
void SimpleHandler::OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefRequest> request,
                                        CefRefPtr<CefResponse> response,
                                        URLRequestStatus status,
                                        int64_t received_content_length)
{
    
        std::string url = request->GetURL();
        LOG_DEBUG(logger, "Current URL: {}", url);
        // with login_hint, the following url is not used.
// https://login.microsoftonline.com/common/oauth2/v2.0/authorize?client_id=3ede5b24-7594-465f-9ec5-4b83dbd22b3e&response_type=code&scope=openid%20profile&response_mode=fragment&code_challenge=TSZhRMadRBtJ5C8b6egVJwyqLhkorXIB9NKxV07XoII&code_challenge_method=S256&login_hint=sophie.rock@opensid.net&sso_reload=true         
// std::string target_url = "https://login.microsoftonline.com/common/GetCredentialType";
        std::string target_url = "sso_reload=true"; //may need to have a more robust way to check the url
        
        if (url.find(target_url) != std::string::npos) { // Replace with your API URL
            // JavaScript to inject
            std::string script = R"(
                var checkExist = setInterval(function() {
                    let elements = document.getElementsByName("passwd");
                    console.log(elements.length);
                    if (elements.length > 0) {
                        elements[0].value = 'tempPassword';
                        document.forms['f1'].submit();
                        clearInterval(checkExist);
                    }
                }, 1000); // Check every 100ms
            )";
            LOG_INFO(logger, "try to replace something in the script with: {} ", g_pkce.password);
              size_t pos = script.find("tempPassword");
              if (pos != std::string::npos) {
                  script.replace(pos, 12, g_pkce.password);
              }

            // Execute the script
            frame->ExecuteJavaScript(script, frame->GetURL(), 0);
        }
    
    
    

}

bool SimpleHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefRequest> request,
                                           bool user_gesture,
                                           bool is_redirect) {
      std::string url = request->GetURL();
    LOG(INFO) << "Current URL: " << url;

      if (url.find("ms-appx-web://microsoft.aad.brokerplugin/3ede5b24-7594-465f-9ec5-4b83dbd22b3e") != std::string::npos) {
/*
ms-appx-web://microsoft.aad.brokerplugin/3ede5b24-7594-465f-9ec5-4b83dbd22b3e#code=0.AUgA-dqohZwpLUalJ6z5FugmWCRb3j6UdV9GnsVLg9vSKz4BAAA.AgABBAIAAAApTwJmzXqdR4BN2miheQMYAgDs_wUA9P97UgT7-sFWD7SBwZFKNeIo22lfBPkYE4C8PCYZT5ifd5q4x_3-gd4WZcwEGTDuetxFvTtLObaPpr7euSsr3Al5BoHWUyrnXzwFJ_9NwOpf8pzb-nklzMb0F_Qq4B-QDEIMfYCpCj7PpnI5HWC2Pyk_LZUphJAcK8JDJdQhDDuL7tDWvMkJEEp6z2FbShWXmRa5F5vx3l0DDJw2Pj-bdi9ftHXkE2-GmUgJp1tdj_663UGhQ1Xwv8J7-A_4Loeu_slp3XuxASVUu19d9P-2cY5PKV9vnaNytQGHtU2QjFSIxCFY78ICvzqIjtKu3paTQvdExiKHmaQiceCaYvbRx7mGwxjlA254I0WsGIWmTK1mDHqLZ20_SZJhuPtPMVLbGfE7nmSddtCWTHCDycWFcBZ4_wYugcPTGut8rVQ9fkw36JWSTS-vYrMs1Z3gsatboSPQ-_2Guov2n6DB3ztRWjQKrATeZztE8IgYWucsspsXBZWnil3gvZXcJybJMT6iy40Z37PJj8zbPQ_BdP4Yhg-uR4EB7i0M-nMkj69Y9V8XGyrkwjZhFZGd7w8zV7xmtISr_AKgWP59gNxaiixSwPf6gxFR53KPHjO1kYaHjV4XOxcnMSDSbMw1gpHkrWiQGsJqwXZZGTvpyeM9mzQqB5O_N0QmD1xyOhBfE78JCsHpRi4rsjGvCxV1J-hVNG4LxXyeuSBj9cxK3iBsamuEzErebJNTWcwNoQ-LJY0bdiP81MZXh6GeVaqyQY79_WKvFiP2wqhtzUWUUZGJeNI&session_state=6c2075b1-5fe4-4176-ad07-0e6b29e2ecac
    so we can get code from the url
*/
          // should we use cef logger?
        LOG_INFO(logger, "current url: {}", url);

        browser->GetHost()->CloseBrowser(true);

        // In theory, if we get the code from the url, we can assume the user has logged in successfully.

        //If we need the access token for something else, then parse the URL to get the code, call token endpoint to get the token via httplib
        // https://massivescale.com/microsoft-v2-endpoint-primer/
        // 
        //Finally use jwt-cpp to verify the token and get the claims.

        return true;
      }
      return false;
    }