#include "include/cef_app.h"
#include "include/cef_command_line.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    // Structure for passing command-line arguments.
    CefMainArgs main_args(hInstance);

    // Optional implementation of application-level callbacks.
    CefRefPtr<CefApp> app;

    // Parse command-line arguments.
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromString(::GetCommandLineW());

    // Check for the disable-gpu switch.
    if (command_line->HasSwitch("disable-gpu")) {
        // Handle the disable-gpu switch if needed.
    }

    // Execute the sub-process logic. This will block until the sub-process should exit.
    return CefExecuteProcess(main_args, app, nullptr);
}