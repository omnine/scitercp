// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_app.h"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"
#include "tests/cefsimple/simple_handler.h"

namespace {

// When using the Views framework this object provides the delegate
// implementation for the CefWindow that hosts the Views-based browser.
class SimpleWindowDelegate : public CefWindowDelegate {
 public:
  SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view,
                       cef_runtime_style_t runtime_style,
                       cef_show_state_t initial_show_state)
      : browser_view_(browser_view),
        runtime_style_(runtime_style),
        initial_show_state_(initial_show_state) {}

  void OnWindowCreated(CefRefPtr<CefWindow> window) override {
    // Add the browser view and show the window.
    window->AddChildView(browser_view_);

    if (initial_show_state_ != CEF_SHOW_STATE_HIDDEN) {
      window->Show();
    }

    if (initial_show_state_ != CEF_SHOW_STATE_MINIMIZED &&
        initial_show_state_ != CEF_SHOW_STATE_HIDDEN) {
      // Give keyboard focus to the browser view.
      browser_view_->RequestFocus();
    }
  }

  void OnWindowDestroyed(CefRefPtr<CefWindow> window) override {
    browser_view_ = nullptr;
  }

  bool CanClose(CefRefPtr<CefWindow> window) override {
    // Allow the window to close if the browser says it's OK.
    CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
    if (browser) {
      return browser->GetHost()->TryCloseBrowser();
    }
    return true;
  }

  CefSize GetPreferredSize(CefRefPtr<CefView> view) override {
    return CefSize(800, 600);
  }

  cef_show_state_t GetInitialShowState(CefRefPtr<CefWindow> window) override {
    return initial_show_state_;
  }

  cef_runtime_style_t GetWindowRuntimeStyle() override {
    return runtime_style_;
  }

 private:
  CefRefPtr<CefBrowserView> browser_view_;
  const cef_runtime_style_t runtime_style_;
  const cef_show_state_t initial_show_state_;

  IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
  DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
};

class SimpleBrowserViewDelegate : public CefBrowserViewDelegate {
 public:
  explicit SimpleBrowserViewDelegate(cef_runtime_style_t runtime_style)
      : runtime_style_(runtime_style) {}

  bool OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browser_view,
                                 CefRefPtr<CefBrowserView> popup_browser_view,
                                 bool is_devtools) override {
    // Create a new top-level Window for the popup. It will show itself after
    // creation.
    CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(
        popup_browser_view, runtime_style_, CEF_SHOW_STATE_NORMAL));

    // We created the Window.
    return true;
  }

  cef_runtime_style_t GetBrowserRuntimeStyle() override {
    return runtime_style_;
  }

 private:
  const cef_runtime_style_t runtime_style_;

  IMPLEMENT_REFCOUNTING(SimpleBrowserViewDelegate);
  DISALLOW_COPY_AND_ASSIGN(SimpleBrowserViewDelegate);
};

}  // namespace

SimpleApp::SimpleApp() = default;

void SimpleApp::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();

  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();

#if !defined(DISABLE_ALLOY_BOOTSTRAP)
  const bool enable_chrome_runtime =
      !command_line->HasSwitch("disable-chrome-runtime");
#endif

  // Check if Alloy style will be used. Alloy style is always used with the
  // Alloy runtime bootstrap and optional with the Chrome runtime bootstrap.
  bool use_alloy_style = true;
  cef_runtime_style_t runtime_style = CEF_RUNTIME_STYLE_DEFAULT;
#if !defined(DISABLE_ALLOY_BOOTSTRAP)
  if (enable_chrome_runtime)
#endif
  {
    use_alloy_style = command_line->HasSwitch("use-alloy-style");
    if (use_alloy_style) {
      runtime_style = CEF_RUNTIME_STYLE_ALLOY;
    }
  }

  // SimpleHandler implements browser-level callbacks.
  CefRefPtr<SimpleHandler> handler(new SimpleHandler(use_alloy_style));

  // Specify CEF browser settings here.
  CefBrowserSettings browser_settings;

  std::string url;

  // Check if a "--url=" value was provided via the command-line. If so, use
  // that instead of the default URL.
  url = command_line->GetSwitchValue("url");
  if (url.empty()) {
    url = "https://www.google.com";
  }

//  url = "https://login.microsoftonline.com/common/oauth2/v2.0/authorize?client_id=3ede5b24-7594-465f-9ec5-4b83dbd22b3e&response_type=code%20id_token&scope=openid%20profile&response_mode=form_post&nonce=638603568248318546.NmZhMjg2NjgtYWIxNi00YTkxLWE5NmEtODA0ZDI3YWEyMTkzYWMyMTllMGMtNWQ4ZS00YWM5LWIzYmItZGZjYTI3M2Q2Y2M4&ui_locales=en-US&mkt=en-US&client-request-id=907569df-d2cb-4023-8696-0ab7e21939ae&state=y_snrSuhhKnMflHNjAnH3vykiE8XkvoSmy8ZD0G16Xg61ubi4Uxgfn6-a1iyUlb2oTBbigz6AMP1Qfwos51-7UfV3BJjkqYHmHXk-lZSVwrgNmrsPB8nlPA1gpuufrYRoZOr0aWuaWR5U-R0Bs69_jcx-Svu7lpwBsWm04749eimp8b-tz85-dgIVoNbC5eLYEMxsg3-ikIswkc53al8DVh7Ub7g63u0lq7pUJAl8y85dy9Hq0e2rduLj_YMChc4vcE0g6i7_6zFKEjYiHCV0A&x-client-SKU=ID_NET8_0&x-client-ver=7.5.1.0&login_hint=sophie.rock@opensid.net";
  url = "https://login.microsoftonline.com/common/oauth2/v2.0/authorize?client_id=3ede5b24-7594-465f-9ec5-4b83dbd22b3e&response_type=code&scope=openid%20profile&response_mode=fragment&code_challenge=5vEtIy2T-G65yXHc8g5zcJDQXICBzZMrtERq0zhx7hM&code_challenge_method=S256&&login_hint=sophie.rock@opensid.net";
  // Views is enabled by default with the Chrome bootstrap (add `--use-native`
  // to disable). Views is disabled by default with the Alloy bootstrap (add
  // `--use-views` to enable).
#if !defined(DISABLE_ALLOY_BOOTSTRAP)
  const bool use_views =
      (enable_chrome_runtime && !command_line->HasSwitch("use-native")) ||
      (!enable_chrome_runtime && command_line->HasSwitch("use-views"));
#else
  const bool use_views = !command_line->HasSwitch("use-native");
#endif

  // If using Views create the browser using the Views framework, otherwise
  // create the browser using the native platform framework.
  if (use_views) {
    // Create the BrowserView.
    CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(
        handler, url, browser_settings, nullptr, nullptr,
        new SimpleBrowserViewDelegate(runtime_style));

    // Optionally configure the initial show state.
    cef_show_state_t initial_show_state = CEF_SHOW_STATE_NORMAL;
    const std::string& show_state_value =
        command_line->GetSwitchValue("initial-show-state");
    if (show_state_value == "minimized") {
      initial_show_state = CEF_SHOW_STATE_MINIMIZED;
    } else if (show_state_value == "maximized") {
      initial_show_state = CEF_SHOW_STATE_MAXIMIZED;
    }
#if defined(OS_MAC)
    // Hidden show state is only supported on MacOS.
    else if (show_state_value == "hidden") {
      initial_show_state = CEF_SHOW_STATE_HIDDEN;
    }
#endif

    // Create the Window. It will show itself after creation.
    CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(
        browser_view, runtime_style, initial_show_state));
  } else {
    // Information used when creating the native window.
    CefWindowInfo window_info;

#if defined(OS_WIN)
    // On Windows we need to specify certain flags that will be passed to
    // CreateWindowEx().
    window_info.SetAsPopup(nullptr, "cefsimple");
#endif

    // Alloy runtime style will create a basic native window. Chrome runtime
    // style will create a fully styled Chrome UI window.
    window_info.runtime_style = runtime_style;

    // Create the first browser window.
    CefBrowserHost::CreateBrowser(window_info, handler, url, browser_settings,
                                  nullptr, nullptr);
  }
}

CefRefPtr<CefClient> SimpleApp::GetDefaultClient() {
  // Called when a new browser window is created via the Chrome runtime UI.
  return SimpleHandler::GetInstance();
}