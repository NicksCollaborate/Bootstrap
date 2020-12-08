#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>

#include <metahost.h>
#include <string>

#pragma comment(lib, "mscoree.lib")

using namespace std;

//
// Parses arguments used to invoke a managed assembly
//
struct ClrArgs
{
    static const LPCWSTR DELIM;

    explicit ClrArgs(LPCWSTR command)
    {
        int i = 0;
        wstring s(command);
        wstring* ptrs[] = { &pwzAssemblyPath, &pwzTypeName, &pwzMethodName };

        while (s.find(DELIM) != wstring::npos && i < 3)
        {
            *ptrs[i++] = s.substr(0, s.find(DELIM));
            s.erase(0, s.find(DELIM) + 1);
        }

        if (s.length() > 0)
            pwzArgument = s;
    }

    wstring pwzAssemblyPath;
    wstring pwzTypeName;
    wstring pwzMethodName;
    wstring pwzArgument;
};

const LPCWSTR ClrArgs::DELIM = L"\t"; // delimiter

//
// Function to start the DotNet runtime and invoke a managed assembly
//
extern "C" [[maybe_unused]] __declspec(dllexport) HRESULT ImplantDotNetAssembly(_In_ LPCTSTR lpCommand)
{
    HRESULT hr;
    ICLRMetaHost *pMetaHost = nullptr;
    ICLRRuntimeInfo *pRuntimeInfo = nullptr;
    ICLRRuntimeHost *pClrRuntimeHost = nullptr;

    // build runtime
    hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&pMetaHost));
    hr = pMetaHost->GetRuntime(L"v4.0.30319", IID_PPV_ARGS(&pRuntimeInfo));
    hr = pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_PPV_ARGS(&pClrRuntimeHost));

    // start runtime
    hr = pClrRuntimeHost->Start();

    // parse the arguments
    ClrArgs args(lpCommand);

    // execute managed assembly
    DWORD pReturnValue;
    hr = pClrRuntimeHost->ExecuteInDefaultAppDomain(
            args.pwzAssemblyPath.c_str(),
            args.pwzTypeName.c_str(),
            args.pwzMethodName.c_str(),
            args.pwzArgument.c_str(),
            &pReturnValue);

    // (optional) unload the .net runtime; note it cannot be restarted if stopped without restarting the process
    //hr = pClrRuntimeHost->Stop();

    // free resources
    pMetaHost->Release();
    pRuntimeInfo->Release();
    pClrRuntimeHost->Release();

    return hr;
}
