//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//

#ifndef WIN32_NO_STATUS
#include <ntstatus.h>
#define WIN32_NO_STATUS
#endif
#include <unknwn.h>

#include "CSampleCredential.h"
#include "CWrappedCredentialEvents.h"
#include "guid.h"

#include <windows.h>
#include <wincred.h>

#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"
#include "../cefpart/simple_app.h"

#include "../helpers/pkce.h"


#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/FileSink.h"
#include <string_view>

#include <locale>
#include <codecvt>

extern PKCE g_pkce;

HWND g_hWnd;
quill::Logger* logger = nullptr; // how to delete it?



std::string WStringToUtf8(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstr);
}

bool GetUsernameFromSerialization(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION& serialization, std::wstring& username) {
    DWORD dwAuthPackage = 0;
    BOOL bSuccess = FALSE;
    DWORD dwMaxUsername = 0;
    DWORD dwMaxDomain = 0;
    DWORD dwMaxPassword = 0;

    // First call to determine the sizes of the buffers
    bSuccess = CredUnPackAuthenticationBuffer(
        0,
        serialization.rgbSerialization,
        serialization.cbSerialization,
        nullptr,
        &dwMaxUsername,
        nullptr,
        &dwMaxDomain,
        nullptr,
        &dwMaxPassword
    );

    if (!bSuccess && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        return false;
    }

    std::vector<wchar_t> usernameBuffer(dwMaxUsername);
    std::vector<wchar_t> domainBuffer(dwMaxDomain);
    std::vector<wchar_t> passwordBuffer(dwMaxPassword);

    // Second call to actually unpack the buffer
    bSuccess = CredUnPackAuthenticationBuffer(
        0,
        serialization.rgbSerialization,
        serialization.cbSerialization,
        usernameBuffer.data(),
        &dwMaxUsername,
        domainBuffer.data(),
        &dwMaxDomain,
        passwordBuffer.data(),
        &dwMaxPassword
    );

    if (bSuccess) {
        username.assign(usernameBuffer.begin(), usernameBuffer.end());
        return true;
    }

    return false;
}

int cefmain(HWND hWnd) {
    int exit_code;
    g_hWnd = hWnd;
        std::string log_level = "info";
        CHAR szPath[MAX_PATH] = {0};
        DWORD dwLen = GetModuleFileNameA(g_hinst, szPath, MAX_PATH);
        szPath[dwLen] = NULL;
        CHAR* pName = strrchr(szPath, '\\');
        if (pName != NULL) {
            pName++;
            strcpy(pName, "dscal.log"); // deepnet security computer azure logon


            // Start the logging backend thread
            quill::Backend::start();

            // Get or create a handler to the file - Write to a different file
            auto file_sink = quill::Frontend::create_or_get_sink<quill::FileSink>(
                szPath);

            logger = quill::Frontend::create_or_get_logger("root", std::move(file_sink));




            strcpy(pName, "config.json");
            g_pkce.readSettings(szPath);
            log_level = g_pkce.get_log_level();
            
            if(log_level != "info"){
                quill::LogLevel level = quill::loglevel_from_string(log_level);
                logger->set_log_level(level);
                LOG_INFO(logger, "log_level is set to {}", log_level);
            }
            else
            {
                LOG_INFO(logger, "log_level is set to Info");
            }            
                
            LOG_INFO(logger, "current typed username: {}", g_pkce.login_name);
        }


    void* sandbox_info = nullptr;

#if defined(CEF_USE_SANDBOX)
    // Manage the life span of the sandbox information object. This is necessary
    // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
    CefScopedSandboxInfo scoped_sandbox;
    sandbox_info = scoped_sandbox.sandbox_info();
#endif

    // Provide CEF with command-line arguments.
    CefMainArgs main_args(g_hinst);

    // SimpleApp implements application-level callbacks for the browser process.
    // It will create the first browser instance in OnContextInitialized() after
    // CEF has initialized.
    CefRefPtr<SimpleApp> app(new SimpleApp());

    // CEF applications have multiple sub-processes (render, GPU, etc) that share
    // the same executable. This function checks the command-line and, if this is
    // a sub-process, executes the appropriate logic.
    exit_code = CefExecuteProcess(main_args, app, sandbox_info);
    if (exit_code >= 0) {
        // The sub-process has completed so return here.
        return exit_code;
    }

    // Specify CEF global settings here.
    CefSettings settings;

#if !defined(CEF_USE_SANDBOX)
    settings.no_sandbox = false;
#endif

    settings.chrome_runtime = false;
    settings.log_severity = LOGSEVERITY_DEBUG;

    // Configure the root cache path
    CefString(&settings.root_cache_path).FromASCII("c:\\temp\\testcache");
    CefString(&settings.cache_path).FromASCII("c:\\temp\\testcache");
    CefString(&settings.log_file).FromASCII("c:\\temp\\testcache\\debug.log");
    
    
    CefString(&settings.browser_subprocess_path).FromASCII("c:\\temp\\subpartner.exe");

//    settings.command_line_args_disabled = true;
    
     CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->AppendSwitch("disable-gpu");
    command_line->AppendSwitch("disable-software-rasterizer");
    command_line->AppendSwitch("disable-gpu-compositing");   
    


    // Initialize the CEF browser process. May return false if initialization
    // fails or if early exit is desired (for example, due to process singleton
    // relaunch behavior).
    if (!CefInitialize(main_args, settings, app.get(), sandbox_info)) {
        return CefGetExitCode();
    }

    // Run the CEF message loop. This will block until CefQuitMessageLoop() is
    // called.
    CefRunMessageLoop();

    // Shut down CEF.
    CefShutdown();

    return 0;
}



// CSampleCredential ////////////////////////////////////////////////////////

// NOTE: Please read the readme.txt file to understand when it's appropriate to
// wrap an another credential provider and when it's not.  If you have questions
// about whether your scenario is an appropriate use of wrapping another credprov,
// please contact credprov@microsoft.com
CSampleCredential::CSampleCredential():
    _cRef(1)
{
    DllAddRef();
    _hWnd = NULL;

    ZeroMemory(_rgCredProvFieldDescriptors, sizeof(_rgCredProvFieldDescriptors));
    ZeroMemory(_rgFieldStatePairs, sizeof(_rgFieldStatePairs));
    ZeroMemory(_rgFieldStrings, sizeof(_rgFieldStrings));

    _pWrappedCredential = NULL;
    _pWrappedCredentialEvents = NULL;
    _pCredProvCredentialEvents = NULL;

    _dwWrappedDescriptorCount = 0;
    _dwDatabaseIndex = 0;
}

CSampleCredential::~CSampleCredential()
{
    for (int i = 0; i < ARRAYSIZE(_rgFieldStrings); i++)
    {
        CoTaskMemFree(_rgFieldStrings[i]);
        CoTaskMemFree(_rgCredProvFieldDescriptors[i].pszLabel);
    }

    _CleanupEvents();
    
    if (_pWrappedCredential)
    {
        _pWrappedCredential->Release();
    }

    DllRelease();
}

// Initializes one credential with the field information passed in. We also keep track
// of our wrapped credential and how many fields it has.
HRESULT CSampleCredential::Initialize(
    __in const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR* rgcpfd,
    __in const FIELD_STATE_PAIR* rgfsp,
    __in ICredentialProviderCredential *pWrappedCredential,
    __in DWORD dwWrappedDescriptorCount
    )
{
    HRESULT hr = S_OK;

    // Grab the credential we're wrapping for future reference.
    if (_pWrappedCredential != NULL)
    {
        _pWrappedCredential->Release();
    }
    _pWrappedCredential = pWrappedCredential;
    _pWrappedCredential->AddRef();

    // We also need to remember how many fields the inner credential has.
    _dwWrappedDescriptorCount = dwWrappedDescriptorCount;

    // Copy the field descriptors for each field. This is useful if you want to vary the field
    // descriptors based on what Usage scenario the credential was created for.
    for (DWORD i = 0; SUCCEEDED(hr) && i < ARRAYSIZE(_rgCredProvFieldDescriptors); i++)
    {
        _rgFieldStatePairs[i] = rgfsp[i];
        hr = FieldDescriptorCopy(rgcpfd[i], &_rgCredProvFieldDescriptors[i]);
    }

    // Initialize the String value of all of our fields.
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"I Work In:", &_rgFieldStrings[SFI_I_WORK_IN_STATIC]);
    }
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"Database", &_rgFieldStrings[SFI_DATABASE_COMBOBOX]);
    }

    return hr;
}

// LogonUI calls this in order to give us a callback in case we need to notify it of 
// anything. We'll also provide it to the wrapped credential.
HRESULT CSampleCredential::Advise(
    __in ICredentialProviderCredentialEvents* pcpce
    )
{
    HRESULT hr = S_OK;

    _CleanupEvents();

    // We keep a strong reference on the real ICredentialProviderCredentialEvents
    // to ensure that the weak reference held by the CWrappedCredentialEvents is valid.
    _pCredProvCredentialEvents = pcpce;
    _pCredProvCredentialEvents->AddRef();
    _pCredProvCredentialEvents->OnCreatingWindow(&_hWnd);

    _pWrappedCredentialEvents = new CWrappedCredentialEvents();
    
    if (_pWrappedCredentialEvents != NULL)
    {
        _pWrappedCredentialEvents->Initialize(this, pcpce);
    
        if (_pWrappedCredential != NULL)
        {
            hr = _pWrappedCredential->Advise(_pWrappedCredentialEvents);
        }
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

// LogonUI calls this to tell us to release the callback. 
// We'll also provide it to the wrapped credential.
HRESULT CSampleCredential::UnAdvise()
{
    HRESULT hr = S_OK;
    
    if (_pWrappedCredential != NULL)
    {
        _pWrappedCredential->UnAdvise();
    }

    _CleanupEvents();

    return hr;
}

// LogonUI calls this function when our tile is selected (zoomed)
// If you simply want fields to show/hide based on the selected state,
// there's no need to do anything here - you can set that up in the 
// field definitions. In fact, we're just going to hand it off to the
// wrapped credential in case it wants to do something.
HRESULT CSampleCredential::SetSelected(__out BOOL* pbAutoLogon)  
{
    HRESULT hr = E_UNEXPECTED;

    if (_pWrappedCredential != NULL)
    {
        hr = _pWrappedCredential->SetSelected(pbAutoLogon);
    }

    return hr;
}

// Similarly to SetSelected, LogonUI calls this when your tile was selected
// and now no longer is. We'll let the wrapped credential do anything it needs.
HRESULT CSampleCredential::SetDeselected()
{
    HRESULT hr = E_UNEXPECTED;

    if (_pWrappedCredential != NULL)
    {
        hr = _pWrappedCredential->SetDeselected();
    }

    return hr;
}

// Get info for a particular field of a tile. Called by logonUI to get information to 
// display the tile. We'll check to see if it's for us or the wrapped credential, and then
// handle or route it as appropriate.
HRESULT CSampleCredential::GetFieldState(
    __in DWORD dwFieldID,
    __out CREDENTIAL_PROVIDER_FIELD_STATE* pcpfs,
    __out CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE* pcpfis
    )
{
    HRESULT hr = E_UNEXPECTED;

    // Make sure we have a wrapped credential.
    if (_pWrappedCredential != NULL)
    {
        // Validate parameters.
        if ((pcpfs != NULL) && (pcpfis != NULL))
        {
            // If the field is in the wrapped credential, hand it off.
            if (_IsFieldInWrappedCredential(dwFieldID))
            {
                hr = _pWrappedCredential->GetFieldState(dwFieldID, pcpfs, pcpfis);
            }
            // Otherwise, we need to see if it's one of ours.
            else
            {
                FIELD_STATE_PAIR *pfsp = _LookupLocalFieldStatePair(dwFieldID);
                // If the field ID is valid, give it info it needs.
                if (pfsp != NULL)
                {
                    *pcpfs = pfsp->cpfs;
                    *pcpfis = pfsp->cpfis;

                    hr = S_OK;
                }
                else
                {
                    hr = E_INVALIDARG;
                }
            }
        }
        else
        {
            hr = E_INVALIDARG;
        }
    }
    return hr;
}

// Sets ppwsz to the string value of the field at the index dwFieldID. We'll check to see if 
// it's for us or the wrapped credential, and then handle or route it as appropriate.
HRESULT CSampleCredential::GetStringValue(
    __in DWORD dwFieldID, 
    __deref_out PWSTR* ppwsz
    )
{
    HRESULT hr = E_UNEXPECTED;

    // Make sure we have a wrapped credential.
    if (_pWrappedCredential != NULL)
    {
        // If this field belongs to the wrapped credential, hand it off.
        if (_IsFieldInWrappedCredential(dwFieldID))
        {
            hr = _pWrappedCredential->GetStringValue(dwFieldID, ppwsz);
        }
        // Otherwise determine if we need to handle it.
        else
        {
            FIELD_STATE_PAIR *pfsp = _LookupLocalFieldStatePair(dwFieldID);
            if (pfsp != NULL)
            {
                hr = SHStrDupW(_rgFieldStrings[SFI_I_WORK_IN_STATIC], ppwsz);
            }
            else
            {
                hr = E_INVALIDARG;
            }
        }
    }
    return hr;
}

// Returns the number of items to be included in the combobox (pcItems), as well as the 
// currently selected item (pdwSelectedItem). We'll check to see if it's for us or the 
// wrapped credential, and then handle or route it as appropriate.
HRESULT CSampleCredential::GetComboBoxValueCount(
    __in DWORD dwFieldID, 
    __out DWORD* pcItems, 
    __out_range(<,*pcItems) DWORD* pdwSelectedItem
    )
{
    HRESULT hr = E_UNEXPECTED;

    // Make sure we have a wrapped credential.
    if (_pWrappedCredential != NULL)
    {
        // If this field belongs to the wrapped credential, hand it off.
        if (_IsFieldInWrappedCredential(dwFieldID))
        {
            hr = _pWrappedCredential->GetComboBoxValueCount(dwFieldID, pcItems, pdwSelectedItem);
        }
        // Otherwise determine if we need to handle it.
        else
        {
            FIELD_STATE_PAIR *pfsp = _LookupLocalFieldStatePair(dwFieldID);
            if (pfsp != NULL)
            {
                *pcItems = ARRAYSIZE(s_rgDatabases);
                *pdwSelectedItem = _dwDatabaseIndex;
                hr = S_OK;
            }
            else
            {
                hr = E_INVALIDARG;
            }
        }
    }

    return hr;
}

// Called iteratively to fill the combobox with the string (ppwszItem) at index dwItem.
// We'll check to see if it's for us or the wrapped credential, and then handle or route 
// it as appropriate.
HRESULT CSampleCredential::GetComboBoxValueAt(
    __in DWORD dwFieldID, 
    __in DWORD dwItem,
    __deref_out PWSTR* ppwszItem
    )
{
    HRESULT hr = E_UNEXPECTED;

    // Make sure we have a wrapped credential.
    if (_pWrappedCredential != NULL)
    {
        // If this field belongs to the wrapped credential, hand it off.
        if (_IsFieldInWrappedCredential(dwFieldID))
        {
            hr = _pWrappedCredential->GetComboBoxValueAt(dwFieldID, dwItem, ppwszItem);
        }
        // Otherwise determine if we need to handle it.
        else
        {
            FIELD_STATE_PAIR *pfsp = _LookupLocalFieldStatePair(dwFieldID);
            if ((pfsp != NULL) && (dwItem < ARRAYSIZE(s_rgDatabases)))
            {
                hr = SHStrDupW(s_rgDatabases[dwItem], ppwszItem);
            }
            else
            {
                hr = E_INVALIDARG;
            }
        }
    }

    return hr;
}

// Called when the user changes the selected item in the combobox. We'll check to see if 
// it's for us or the wrapped credential, and then handle or route it as appropriate.
HRESULT CSampleCredential::SetComboBoxSelectedValue(
    __in DWORD dwFieldID,
    __in DWORD dwSelectedItem
    )
{
    HRESULT hr = E_UNEXPECTED;

    // Make sure we have a wrapped credential.
    if (_pWrappedCredential != NULL)
    {
        // If this field belongs to the wrapped credential, hand it off.
        if (_IsFieldInWrappedCredential(dwFieldID))
        {
            hr = _pWrappedCredential->SetComboBoxSelectedValue(dwFieldID, dwSelectedItem);
        }
        // Otherwise determine if we need to handle it.
        else
        {
            FIELD_STATE_PAIR *pfsp = _LookupLocalFieldStatePair(dwFieldID);
            if ((pfsp != NULL) && (dwSelectedItem < ARRAYSIZE(s_rgDatabases)))
            {
                _dwDatabaseIndex = dwSelectedItem;
                hr = S_OK;
            }
            else
            {
                hr = E_INVALIDARG;
            }
        }
    }

    return hr;
}

//------------- 
// The following methods are for logonUI to get the values of various UI elements and 
// then communicate to the credential about what the user did in that field. Even though
// we don't offer these field types ourselves, we need to pass along the request to the
// wrapped credential.

HRESULT CSampleCredential::GetBitmapValue(
    __in DWORD dwFieldID, 
    __out HBITMAP* phbmp
    )
{
    HRESULT hr = E_UNEXPECTED;

    if (_pWrappedCredential != NULL)
    {
        hr = _pWrappedCredential->GetBitmapValue(dwFieldID, phbmp);
    }

    return hr;
}

HRESULT CSampleCredential::GetSubmitButtonValue(
    __in DWORD dwFieldID,
    __out DWORD* pdwAdjacentTo
    )
{
    HRESULT hr = E_UNEXPECTED;

    if (_pWrappedCredential != NULL)
    {
        hr = _pWrappedCredential->GetSubmitButtonValue(dwFieldID, pdwAdjacentTo);
    }

    return hr;
}

HRESULT CSampleCredential::SetStringValue(
    __in DWORD dwFieldID,
    __in PCWSTR pwz
    )
{
    HRESULT hr = E_UNEXPECTED;

    if (_pWrappedCredential != NULL)
    {
        hr = _pWrappedCredential->SetStringValue(dwFieldID, pwz);
    }

    return hr;

}

HRESULT CSampleCredential::GetCheckboxValue(
    __in DWORD dwFieldID, 
    __out BOOL* pbChecked,
    __deref_out PWSTR* ppwszLabel
    )
{
    HRESULT hr = E_UNEXPECTED;

    if (_pWrappedCredential != NULL)
    {
        if (_IsFieldInWrappedCredential(dwFieldID))
        {
            hr = _pWrappedCredential->GetCheckboxValue(dwFieldID, pbChecked, ppwszLabel);
        }
    }

    return hr;
}

HRESULT CSampleCredential::SetCheckboxValue(
    __in DWORD dwFieldID, 
    __in BOOL bChecked
    )
{
    HRESULT hr = E_UNEXPECTED;

    if (_pWrappedCredential != NULL)
    {
        hr = _pWrappedCredential->SetCheckboxValue(dwFieldID, bChecked);
    }

    return hr;
}

HRESULT CSampleCredential::CommandLinkClicked(__in DWORD dwFieldID)
{
    HRESULT hr = E_UNEXPECTED;

    if (_pWrappedCredential != NULL)
    {
        hr = _pWrappedCredential->CommandLinkClicked(dwFieldID);
    }

    return hr;
}
//------ end of methods for controls we don't have ourselves ----//


//
// Collect the username and password into a serialized credential for the correct usage scenario 
// (logon/unlock is what's demonstrated in this sample).  LogonUI then passes these credentials 
// back to the system to log on.
//
HRESULT CSampleCredential::GetSerialization(
    __out CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE* pcpgsr,
    __out CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcs, 
    __deref_out_opt PWSTR* ppwszOptionalStatusText, 
    __out CREDENTIAL_PROVIDER_STATUS_ICON* pcpsiOptionalStatusIcon
    )
{
    HRESULT hr = E_UNEXPECTED;

    if (_pWrappedCredential != NULL)
    {
        //This one actually doesn't do authentication. If the wrong password is given, hr could be still S_OK
        hr = _pWrappedCredential->GetSerialization(pcpgsr, pcpcs, ppwszOptionalStatusText, pcpsiOptionalStatusIcon);
    }

    if (hr != S_OK)
        return hr;


    std::wstring username;
    GetUsernameFromSerialization(*pcpcs, username);
    g_pkce.login_name = WStringToUtf8(username);

    // should get from pcpcs
    //g_pkce.login_name = "sophie.rock@opensid.net";

    cefmain(NULL);  // _hWnd


    return hr;
}



// ReportResult is completely optional. However, we will hand it off to the wrapped
// credential in case they want to handle it.
HRESULT CSampleCredential::ReportResult(
    __in NTSTATUS ntsStatus, 
    __in NTSTATUS ntsSubstatus,
    __deref_out_opt PWSTR* ppwszOptionalStatusText, 
    __out CREDENTIAL_PROVIDER_STATUS_ICON* pcpsiOptionalStatusIcon
    )
{
    HRESULT hr = E_UNEXPECTED;

    if (_pWrappedCredential != NULL)
    {
        hr = _pWrappedCredential->ReportResult(ntsStatus, ntsSubstatus, ppwszOptionalStatusText, pcpsiOptionalStatusIcon);
    }

    return hr;
}

BOOL CSampleCredential::_IsFieldInWrappedCredential(
    __in DWORD dwFieldID
    )
{
    return (dwFieldID < _dwWrappedDescriptorCount);
}

FIELD_STATE_PAIR *CSampleCredential::_LookupLocalFieldStatePair(
    __in DWORD dwFieldID
    )
{
    // Offset into the ID to account for the wrapped fields.
    dwFieldID -= _dwWrappedDescriptorCount;

    // If the index if valid, give it the info it wants.
    if (dwFieldID < SFI_NUM_FIELDS)
    {
        return &(_rgFieldStatePairs[dwFieldID]);
    }
    
    return NULL;
}

void CSampleCredential::_CleanupEvents()
{
    // Call Uninitialize before releasing our reference on the real 
    // ICredentialProviderCredentialEvents to avoid having an
    // invalid reference.
    if (_pWrappedCredentialEvents != NULL)
    {
        _pWrappedCredentialEvents->Uninitialize();
        _pWrappedCredentialEvents->Release();
        _pWrappedCredentialEvents = NULL;
    }

    if (_pCredProvCredentialEvents != NULL)
    {
        _pCredProvCredentialEvents->Release();
        _pCredProvCredentialEvents = NULL;
    }
}
