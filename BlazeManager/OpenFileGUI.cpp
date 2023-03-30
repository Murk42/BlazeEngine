#include "OpenFileGUI.h"

#ifdef _WIN32

#include <Windows.h>
#include <shlobj.h>
#undef LoadLibrary
#include <algorithm>
#include <iostream>

std::string utf8_encode(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::string OpenFileGUI()
{
	OPENFILENAMEA ofn = { 0 };
	char Buffer[300];
	std::fill(Buffer, Buffer + 300, '\0');

	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = Buffer;
	ofn.nMaxFile = 300;
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrFilter = NULL;
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	GetOpenFileNameA(&ofn);

	return std::string(Buffer);
}
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{

    if (uMsg == BFFM_INITIALIZED)
    {
        std::string tmp = (const char*)lpData;
        std::cout << "path: " << tmp << std::endl;
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }

    return 0;
}

std::string OpenDirectoryGUI()
{
    TCHAR path[MAX_PATH];

    const char* path_param = "";

    BROWSEINFO bi = { 0 };
    bi.lpszTitle = (L"Browse for folder...");
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn = BrowseCallbackProc;
    bi.lParam = (LPARAM)path_param;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != 0)
    {
        //get the name of the folder and put it in path
        SHGetPathFromIDList(pidl, path);

        //free memory used
        IMalloc* imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc)))
        {
            imalloc->Free(pidl);
            imalloc->Release();
        }

        return utf8_encode(std::wstring((const wchar_t*)path)) + "/";
    }

    return "";
}
#else
std::string GuiOpenFile()
{
	throw;
	return std::string();
}
#endif