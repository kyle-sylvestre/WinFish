#include "WinFishApp.h"
#include <SDL2/SDL.h>
#include <filesystem>

static bool IsGameDirectory(std::filesystem::path p)
{
    std::filesystem::path rpath = p / "properties" / "resources.xml";
    return Sexy::FileExists(rpath.string());
}

#if defined(__APPLE__)
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
void PlatformInit()
{
    SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");
    NSString *pathWithTilde = @"~/Library/Application Support/InsaniquariumPortable";
    NSString *fullPath = [pathWithTilde stringByExpandingTildeInPath];
    const char *path = [fullPath UTF8String];
    Sexy::MkDir(path);
    std::string filename = Sexy::StrFormat("%s/gamefolder.txt", path);

    // check for known game data locations
    if (!Sexy::FileExists(filename))
    {
        // check directory containing app
        NSString *appPath = [[NSBundle mainBundle] bundlePath];
        NSString *appDir = [appPath stringByDeletingLastPathComponent];
        std::string app = [appDir UTF8String];
        if (IsGameDirectory(app))
        {
            std::ofstream outFile(filename, std::ios::binary);
            if (outFile)
            {
                outFile.write(app.c_str(), app.size());
                outFile.close();
            }
        }
    }
    
    for (;;)
    {
        // read insaniquarium file location
        std::string rpath;
        std::ifstream file(filename, std::ios::binary); // open in binary mode
        if (file)
        {
            std::ostringstream contents;
            contents << file.rdbuf(); // read entire file into stream
            rpath = contents.str();
            
            // remove line endings
            while (rpath.size() && (rpath.back() == '\n' || rpath.back() == '\r'))
                rpath.pop_back();
        }

        if (IsGameDirectory(rpath))
        {
            Sexy::SetResourceFolder(rpath);
            Sexy::ChDir(rpath);
            break;
        }
        else
        {
            // THIS WILL NOT WORK AS CONSOLE APP, NEEDS TO BE APP BUNDLE
            NSOpenPanel* openDlg = [NSOpenPanel openPanel];
            [openDlg setMessage:@"Select Insaniquarium Deluxe Folder"];
            [openDlg setCanChooseDirectories:YES];
            [openDlg setCanChooseFiles:NO];
            [openDlg setAllowsMultipleSelection:NO];
            [openDlg setShowsHiddenFiles:YES];
            [openDlg setDirectoryURL:[NSURL URLWithString:[NSString stringWithUTF8String:"."] ] ];
            [openDlg setTreatsFilePackagesAsDirectories:YES];
            auto result = [openDlg runModal];
            if (result == NSModalResponseOK)
            {
                std::string path = [[[[openDlg URLs] objectAtIndex:0] path] UTF8String];
                std::ofstream outFile(filename, std::ios::binary);
                if (outFile)
                {
                    outFile.write(path.c_str(), path.size());
                    outFile.close();
                }
            }
            else
            {
                exit(0);
            }
        }
    }
}
#elif defined(_WIN32)
#include <windows.h>
#include <shlobj.h>         // SHGetFolderPathA, SHBrowseForFolder
#include <shellapi.h>       // SHFileOperationW
#include <shobjidl.h>
#include <shlwapi.h>        // PathIsDirectoryEmptyW
#if !defined(__MINGW32__)
#include <shlobj_core.h>    // SHGetKnownFolderPath
#endif
#include <KnownFolders.h>   // KNOWNFOLDERID
#include <string>
#include <fstream>
#include <sstream>

static bool PickFolder(std::string& outPath)
{
    bool result = false;
    IFileDialog* pDialog = nullptr;

    if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
        return false;

    HRESULT hr = CoCreateInstance(
        CLSID_FileOpenDialog,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pDialog)
    );

    if (FAILED(hr))
        return false;

    DWORD options = 0;
    pDialog->GetOptions(&options);

    // Folder picker mode
    pDialog->SetOptions(options |
        FOS_PICKFOLDERS |
        FOS_FORCEFILESYSTEM |
        FOS_PATHMUSTEXIST |
        FOS_NOCHANGEDIR);

    pDialog->SetTitle(L"Select Insaniquarium Deluxe Folder");

    hr = pDialog->Show(nullptr);

    if (SUCCEEDED(hr))
    {
        IShellItem* item = nullptr;

        if (SUCCEEDED(pDialog->GetResult(&item)))
        {
            PWSTR path = nullptr;

            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)))
            {
                outPath = Sexy::PathToU8(path);
                CoTaskMemFree(path);
                result = true;
            }

            item->Release();
        }
    }

    pDialog->Release();
    return result;
}

static std::wstring GetFolder(KNOWNFOLDERID folder_id)
{
    std::wstring result;
    PWSTR path = NULL;
    HRESULT hr = SHGetKnownFolderPath(folder_id, 0, NULL, &path);
    if (SUCCEEDED(hr))
    {
        result = path;
        CoTaskMemFree(path);
    }
    return result;
}

std::wstring GetUTF16(const std::string& utf8)
{
    std::wstring result;
    if (utf8.size() != 0)
    {
        int len = MultiByteToWideChar(CP_UTF8,
                                      MB_ERR_INVALID_CHARS,
                                      utf8.data(),
                                      (int)utf8.size(),
                                      nullptr,
                                      0);

        if (len > 0)
        {
            result.resize(len);
            MultiByteToWideChar(CP_UTF8,
                                MB_ERR_INVALID_CHARS,
                                utf8.data(),
                                (int)utf8.size(),
                                result.data(),
                                len);
        }
    }
    return result;
}

void PlatformInit()
{
    // check if running on wine
    {
        HMODULE hntdll = GetModuleHandle("ntdll.dll");
        if(hntdll)
        {
            FARPROC wine_get_version = GetProcAddress(hntdll, "wine_get_version");
            if (wine_get_version)
            {
                // wasapi causes static/crackle sounds, force directsound
                SDL_SetHint(SDL_HINT_AUDIODRIVER, "directsound");
            }
        }
    }

    // 1. Create settings folder in AppData\Roaming\InsaniquariumPortable
    wchar_t appDataPath[MAX_PATH] = {};
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, appDataPath)))
    {
        std::string path = Sexy::StrFormat("%ls\\InsaniquariumPortable", appDataPath);
        Sexy::MkDir(path.c_str());

        std::string gameFolderFile = path + "\\gamefolder.txt";
        std::string rpath;

        // check for known game data locations
        if (!Sexy::FileExists(gameFolderFile))
        {
            const std::string FOLDERS[] = {
                ".",
                Sexy::StrFormat("%ls\\Steam\\SteamApps\\Common\\Insaniquarium Deluxe", GetFolder(FOLDERID_ProgramFilesX86).c_str()),
                Sexy::StrFormat("%ls\\Steam\\SteamApps\\Common\\Insaniquarium Deluxe", GetFolder(FOLDERID_ProgramFiles).c_str()),
                Sexy::StrFormat("%ls\\PopCap Games\\Insaniquarium Deluxe", GetFolder(FOLDERID_ProgramFilesX86).c_str()),
            };
            for (std::string folder : FOLDERS)
            {
                if (IsGameDirectory(folder))
                {
                    std::ofstream outFile(gameFolderFile, std::ios::binary);
                    if (outFile)
                    {
                        outFile.write(folder.c_str(), folder.size());
                        outFile.close();
                    }
                    break;
                }
            }
        }

        // check for known save locations
        {
            wchar_t appdata[MAX_PATH] = {};
            SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, appdata);

            std::string datapath;
            std::ifstream file(gameFolderFile, std::ios::binary);
            if (file)
            {
                std::ostringstream contents;
                contents << file.rdbuf();
                datapath = contents.str();
                while (!datapath.empty() && (datapath.back() == '\r' || datapath.back() == '\n'))
                    datapath.pop_back();
            }

            const std::string FOLDERS[] = {
                Sexy::StrFormat("%ls\\PopCap Games\\Insaniquarium Deluxe", appdata),
                datapath,
            };
            
            std::string dest = path + "\\userdata";
            for (std::string folder : FOLDERS)
            {
                std::string src = Sexy::StrFormat("%s\\userdata", folder.c_str());
                if (Sexy::FileExists(src) && !Sexy::FileExists(dest))
                {
                    std::wstring wsrc = GetUTF16(src);
                    std::wstring wdest = GetUTF16(dest + "\\..");
                    if (!PathIsDirectoryEmptyW(wsrc.c_str()))
                    {
                        // double null terminated: PCZZWSTR
                        wsrc.push_back(L'\0');
                        wdest.push_back(L'\0');

                        SHFILEOPSTRUCTW s = {};
                        s.wFunc = FO_COPY;
                        s.fFlags = FOF_NO_UI;
                        s.pTo = wdest.c_str();
                        s.pFrom = wsrc.c_str();
                        SHFileOperationW(&s);
                        break;
                    }
                }
            }
        }

        for (;;)
        {
            // 2. Try reading saved path
            std::ifstream file(gameFolderFile, std::ios::binary);
            if (file)
            {
                std::ostringstream contents;
                contents << file.rdbuf();
                rpath = contents.str();
                while (!rpath.empty() && (rpath.back() == '\r' || rpath.back() == '\n'))
                    rpath.pop_back();
            }

            // 3. Check if the game folder is valid
            if (IsGameDirectory(rpath))
            {
                Sexy::SetResourceFolder(rpath);
                Sexy::ChDir(rpath);
                break;
            }
            else
            {
                // 4. Show legacy folder selection dialog
                if (!PickFolder(rpath))
                {
                    exit(0); // user canceled
                }

                // Save path to file
                std::ofstream outFile(gameFolderFile, std::ios::binary);
                if (outFile)
                {
                    outFile.write(rpath.c_str(), rpath.size());
                    outFile.close();
                }
            }
        }
    }
    else
    {
        MessageBoxA(nullptr, "Failed to get AppData folder.", "Error", MB_OK | MB_ICONERROR);
        exit(0);
    }
}

#elif defined(__ANDROID__)
#include <jni.h>
#include <unistd.h>
#include <string>
std::string android_data_path;
extern "C"
JNIEXPORT void JNICALL
Java_io_itch_ksylvestre_insaniquariumportable_InsaniquariumPortableActivity_nativeSetWorkingDir(
        JNIEnv* env,
        jobject /* this */,
        jstring path_) {

    const char *path = env->GetStringUTFChars(path_, nullptr);
    if (path)
    {
        android_data_path = path;
        env->ReleaseStringUTFChars(path_, path);
    }
}
void PlatformInit() 
{
    SDL_Log("ANDROID_DATA_PATH:%s", android_data_path.c_str());
    Sexy::SetResourceFolder(android_data_path);
    Sexy::ChDir(android_data_path);
}

#if __ANDROID_API__ < 23
extern "C" ssize_t __write_chk(int fd, const void* buf, size_t count, size_t buf_size)
{
    //__check_count("write", "count", count);
    //__check_buffer_access("write", "read from", count, buf_size);
    return write(fd, buf, count);
}
#undef stderr
FILE *stderr = &__sF[2];
#endif

#elif defined(__SWITCH__)
#include <switch.h>
void PlatformInit() 
{
    const char *path = "sdmc:/switch/insaniquariumportable";
    Sexy::SetResourceFolder(path);
    Sexy::SetAppDataFolder(path);
    Sexy::ChDir(path);
    std::string logpath = Sexy::StrFormat("%s/log.txt", path);
    FILE *logfile = fopen(logpath.c_str(), "wb");
    const auto LogFunction = [](void *userdata, int category, SDL_LogPriority priority, const char *message)
    {
        if (userdata && message)
        {
            fprintf((FILE*)userdata, "%s\n", message);
            fflush((FILE*)userdata);
        }
    };
    SDL_LogSetOutputFunction(LogFunction, logfile);

    // don't change button positions to match switch layout
    SDL_SetHint(SDL_HINT_GAMECONTROLLER_USE_BUTTON_LABELS, "0");
}
extern "C" {
	u32 __nx_stack_size = 4 * 1024 * 1024; // 4MB
}

#else
void PlatformInit() 
{
}
#endif

int main(int argc, char *argv[])
{
    PlatformInit();
    Sexy::WinFishApp app;

    SDL_Log("Init");
    app.Init();
    SDL_Log("Start");
    app.Start();
    SDL_Log("Shutdown");
    app.Shutdown();
    SDL_Log("Return from main");
    return 0;
}
