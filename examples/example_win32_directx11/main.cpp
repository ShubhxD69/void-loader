#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_edited.hpp"
#include "wic_texture_loader.h"

#include "image.h"
#include "font.h"


#include <d3d11.h>
#include <tchar.h>
#include <dwmapi.h>

#include <vector>
#include <random>
#include <math.h>
#include <memory>

#include <dwmapi.h>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <tlhelp32.h>
#include <string>
#include <cstdlib> 
#include <thread>
#include <Shlwapi.h>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cstdio>
#include <cwchar>
#include <io.h>
#include "Keyauth.h"
#include "config.h"

#pragma comment(lib, "Shlwapi.lib")

std::vector<ImVec2> circles_pos;
std::vector<ImVec2> circles_dir;
std::vector<int> circles_radius;






void runCommand(const std::string& command) {
    system(command.c_str());
}

// Function to execute command
void ExecuteCommand(const std::string& command) {
    std::system(command.c_str());
}






// Variables globales o dentro de una clase seg�n sea necesario
static bool baseboard = false;
static bool disk = false;
static bool aimbothead = false;
static bool aimbotdrag = false;
static bool chamsmenu = false;
static bool stream = false;
static bool bios = false;
static bool gpu = false;
static bool gpu1 = false;

// Login states for each spoof page
static bool setup_logged_in = false;
static bool auto_bypass_logged_in = false;
static bool manual_bypass_logged_in = false;

// Sub-page states for each main page
static int setup_sub_page = 0;
static int auto_bypass_sub_page = 0;
static int manual_bypass_sub_page = 0;

// Main spoof page selector
static int spoof_page = 0;

// Console log system
static std::string ConsoleLogs = "";
static std::string DisplayedLogs = "";
static float console_alpha = 1.0f; // Always visible
static float console_timer = 0.0f;
static bool console_animating = false;
static const int MAX_LOG_LINES = 15; // Maximum number of log lines before reset

// Function to trigger console log
void TriggerConsoleLog(const std::string& message) {
    // Count current lines in ConsoleLogs
    int line_count = 1;
    for (char c : ConsoleLogs) {
        if (c == '\n') line_count++;
    }
    
    // If we're at max lines, reset the console
    if (line_count >= MAX_LOG_LINES) {
        ConsoleLogs = "[+] Console Reset - Previous logs cleared\n";
        DisplayedLogs = "[+] Console Reset - Previous logs cleared\n";
    }
    
    // Add new message with timestamp
    if (ConsoleLogs != "" && 
        ConsoleLogs != "[+] Console Reset - Previous logs cleared\n") {
        ConsoleLogs += "\n";
    }
    ConsoleLogs += message;
    
    // Start animation for new message only
    console_animating = true;
    console_timer = 0.0f;
}

// Enhanced console log function that integrates with ADB manager
void LogToConsole(const std::string& message) {
    TriggerConsoleLog(message);
}

// Basic console log functions
void LogInfo(const std::string& message) {
    TriggerConsoleLog("[+] " + message);
}

void LogSuccess(const std::string& message) {
    TriggerConsoleLog("[+] " + message);
}

void LogError(const std::string& message) {
    TriggerConsoleLog("[+] " + message);
}

void LogWarning(const std::string& message) {
    TriggerConsoleLog("[+] " + message);
}


// Function to render console log
void RenderConsoleLog() {
    // Always show console, even if empty
    console_timer += ImGui::GetIO().DeltaTime;

    // Update displayed text based on animation
    if (console_animating && !ConsoleLogs.empty()) {
        // Calculate how much of the new content to show
        int total_length = ConsoleLogs.length();
        int displayed_length = DisplayedLogs.length();
        int new_content_length = total_length - displayed_length;
        
        if (new_content_length > 0) {
            // Animate only the new content (slower speed: 15.0f instead of 25.0f)
            int animate_length = (int)(console_timer * 5.0f);
            if (animate_length >= new_content_length) {
                // Animation complete
                DisplayedLogs = ConsoleLogs;
                console_animating = false;
            } else {
                // Show displayed content + partial new content
                DisplayedLogs = ConsoleLogs.substr(0, displayed_length + animate_length);
            }
        } else {
            // No new content to animate
            DisplayedLogs = ConsoleLogs;
            console_animating = false;
        }
    } else if (!ConsoleLogs.empty()) {
        // Show full text if not animating
        DisplayedLogs = ConsoleLogs;
    }

    if (!DisplayedLogs.empty()) {
        // Move text further left (reduced padding from 10 to 5)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
        
        // Use accent color for console text and enable text wrapping
        ImGui::PushTextWrapPos(ImGui::GetContentRegionMax().x - 10);
        ImGui::TextColored(ImColor(c::accent), "%s", DisplayedLogs.c_str());
        ImGui::PopTextWrapPos();
    } else {
        // Show placeholder if no message
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
        ImGui::TextColored(ImColor(c::accent), "");
    }
}


std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
static int64_t timeuptd = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

std::string tm_to_readable_time(tm ctx) {
    char buffer[80];

    strftime(buffer, sizeof(buffer), "%m/%d/%y", &ctx);

    return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
    auto cv = strtol(timestamp.c_str(), NULL, 10); // long

    return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
    std::tm context;

    localtime_s(&context, &timestamp);

    return context;
}



void draw_circle(ImVec2 pos, int radius, ImU32 color)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    unsigned char* color_ptr = (unsigned char*)&color;
    draw_list->AddCircleFilled(pos, radius - 1, IM_COL32(color_ptr[0], color_ptr[1], color_ptr[2], 75));
}

void draw_line(ImVec2 pos1, ImVec2 pos2, ImU32 color, int radius)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    float distance = std::sqrt(
        std::pow(pos2.x - pos1.x, 2) + // std:: pow
        std::pow(pos2.y - pos1.y, 2)    // std:: pow
    );
    float alpha;
    if (distance <= 20.0f) {
        alpha = 255.0f;
    }
    else {
        alpha = (1.0f - ((distance - 20.0f) / 25.0f)) * 255.0f;
    }

    int r = (color & 0xFF0000) >> 16; // Extract red component
    int g = (color & 0x00FF00) >> 8;  // Extract green component
    int b = (color & 0x0000FF);       // Extract blue component

    unsigned char* color_ptr = (unsigned char*)&color;

    draw_list->AddLine(pos1, pos2, IM_COL32(color_ptr[0], color_ptr[1], color_ptr[2], static_cast<int>(alpha)), 1.0f);
    if (distance >= 40.0f) {
        draw_list->AddCircleFilled(pos1, radius - 0.96f, IM_COL32(color_ptr[0], color_ptr[1], color_ptr[2], 30));
        draw_list->AddCircleFilled(pos2, radius - 0.96f, IM_COL32(color_ptr[0], color_ptr[1], color_ptr[2], 30));
    }
    else if (distance <= 20.0f) {
        draw_list->AddCircleFilled(pos1, radius, IM_COL32(color_ptr[0], color_ptr[1], color_ptr[2], 30));
        draw_list->AddCircleFilled(pos2, radius, IM_COL32(color_ptr[0], color_ptr[1], color_ptr[2], 30));
    }
    else {
        float radius_factor = 1.0f - ((distance - 20.0f) / 20.0f);
        float offset_factor = 1.0f - radius_factor;
        float offset = (radius - radius * radius_factor) * offset_factor;
        draw_list->AddCircleFilled(pos1, radius - offset, IM_COL32(color_ptr[0], color_ptr[1], color_ptr[2], 30));
        draw_list->AddCircleFilled(pos2, radius - offset, IM_COL32(color_ptr[0], color_ptr[1], color_ptr[2], 30));
    }
}

void move_circles()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 360);

    for (int i = 0; i < circles_pos.size(); i++)
    {
        ImVec2& pos = circles_pos[i];
        ImVec2& dir = circles_dir[i];
        int radius = circles_radius[i];

        pos.x += dir.x * 0.4;
        pos.y += dir.y * 0.4;

        if (pos.x - radius < 0 || pos.x + radius > ImGui::GetWindowWidth())
        {
            dir.x = -dir.x;
            dir.y = dis(gen) % 2 == 0 ? -1 : 1;
        }

        if (pos.y - radius < 0 || pos.y + radius > ImGui::GetWindowHeight())
        {
            dir.y = -dir.y;
            dir.x = dis(gen) % 2 == 0 ? -1 : 1;
        }
    }
}

void draw_circles_and_lines(ImU32 color)
{
    move_circles();

    for (int i = 0; i < circles_pos.size(); i++)
    {
        draw_circle(circles_pos[i], circles_radius[i], color);

        for (int j = i + 1; j < circles_pos.size(); j++)
        {
            float distance = ImGui::GetIO().FontGlobalScale * std::sqrt(std::pow(circles_pos[j].x - circles_pos[i].x, 2) + std::pow(circles_pos[j].y - circles_pos[i].y, 2) );

            if (distance <= 45.0f)
            {
                draw_line(circles_pos[i], circles_pos[j], color, circles_radius[i]);
            }
        }
    }
}

void setup_circles()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);
    std::uniform_int_distribution<> pos_dis(0, static_cast<int>(1980));
    std::uniform_int_distribution<> pos_dis_y(0, static_cast<int>(1080));

    for (int i = 0; i < 150; i++)
    {
        circles_pos.push_back(ImVec2(pos_dis(gen), pos_dis_y(gen)));
        circles_dir.push_back(ImVec2(dis(gen) == 0 ? -1 : 1, dis(gen) == 0 ? -1 : 1));
        circles_radius.push_back(3);
    }
}


namespace ImGui
{
    int rotation_start_index;
    void ImRotateStart()
    {
        rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
    }

    ImVec2 ImRotationCenter()
    {
        ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

        const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
        for (int i = rotation_start_index; i < buf.Size; i++)
            l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

        return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
    }


    void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
    {
        float s = sin(rad), c = cos(rad);
        center = ImRotate(center, s, c) - center;

        auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
        for (int i = rotation_start_index; i < buf.Size; i++)
            buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
    }
}
void Trinage_background()
{
    ImVec2 screen_size = { (float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN) };

    static ImVec2 partile_pos[100];
    static ImVec2 partile_target_pos[100];
    static float partile_speed[100];
    static float partile_size[100];
    static float partile_radius[100];
    static float partile_rotate[100];

    for (int i = 1; i < 70; i++)
    {
        if (partile_pos[i].x == 0 || partile_pos[i].y == 0)
        {
            partile_pos[i].x = rand() % (int)screen_size.x + 1;
            partile_pos[i].y = 15.f;
            partile_speed[i] = 1 + rand() % 35;
            partile_radius[i] = rand() % 4;
            partile_size[i] = rand() % 8;

            partile_target_pos[i].x = rand() % (int)screen_size.x;
            partile_target_pos[i].y = screen_size.y * 2;
        }

        partile_pos[i] = ImLerp(partile_pos[i], partile_target_pos[i], ImGui::GetIO().DeltaTime * (partile_speed[i] / 60));
        partile_rotate[i] += ImGui::GetIO().DeltaTime;

        if (partile_pos[i].y > screen_size.y)
        {
            partile_pos[i].x = 0;
            partile_pos[i].y = 0;
            partile_rotate[i] = 0;
        }
        ImGui::ImRotateStart();
        ImGui::GetWindowDrawList()->AddCircleFilled(partile_pos[i], partile_size[i], ImColor(c::accent), 1);
        ImGui::GetWindowDrawList()->AddShadowCircle(partile_pos[i], 8.f, ImColor(c::accent), 58.f + partile_size[i], ImVec2(0, 0), 0, 1);
        ImGui::ImRotateEnd(partile_rotate[i]);
    }
}

static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int tabs = 0;

int quest_count;
std::vector<std::string> quest_text;
char quest[250] = { "" };

float timer = 0;

using namespace ImGui;

namespace font
{
    ImFont* lexend_regular = nullptr;
    ImFont* lexend_medium = nullptr;
    ImFont* lexend_medium_x = nullptr;
    ImFont* lexend_bold = nullptr;

    // Loop below assigns icomoon[14]..icomoon[40] — need 41 slots (was [19]: buffer overflow corrupted textures / draw state).
    ImFont* icomoon[41]{};

}

void ExecuteCommandInVisibleWindow(const std::string& command) {
    std::string full_command = "start cmd /c " + command;
    std::system(full_command.c_str());
}

static std::filesystem::path GetRememberFilePath()
{
    wchar_t module_path[MAX_PATH] = {};
    if (GetModuleFileNameW(nullptr, module_path, MAX_PATH) == 0)
        return std::filesystem::path("remember.dat");

    std::filesystem::path exe_path(module_path);
    return exe_path.parent_path() / "remember.dat";
}

static void SaveRememberedLicense(const char* license_key, bool remember)
{
    const std::filesystem::path remember_file = GetRememberFilePath();

    if (!remember || !license_key || license_key[0] == '\0') {
        std::error_code ec;
        std::filesystem::remove(remember_file, ec);
        return;
    }

    std::ofstream out(remember_file, std::ios::trunc);
    if (!out)
        return;

    out << "1\n" << license_key << '\n';
}

static void LoadRememberedLicense(char* out_license_key, size_t out_size, bool& remember)
{
    if (!out_license_key || out_size == 0)
        return;

    out_license_key[0] = '\0';
    remember = false;

    std::ifstream in(GetRememberFilePath());
    if (!in)
        return;

    std::string remember_line;
    std::string key_line;
    std::getline(in, remember_line);
    std::getline(in, key_line);

    remember = (remember_line == "1");
    if (remember && !key_line.empty()) {
        strncpy_s(out_license_key, out_size, key_line.c_str(), _TRUNCATE);
    }
}

// Usage


namespace texture
{
    ID3D11ShaderResourceView* background = nullptr;
    ID3D11ShaderResourceView* spoof_icon = nullptr;
    ID3D11ShaderResourceView* bluestackoxcy = nullptr;
    ID3D11ShaderResourceView* msioxcy = nullptr;
    ID3D11ShaderResourceView* logo = nullptr;
    ID3D11ShaderResourceView* empty1 = nullptr;
    ID3D11ShaderResourceView* empty2 = nullptr;
    ID3D11ShaderResourceView* empty3 = nullptr;
}

void CricleProgress(const char* name, float progress, float max, float radius)
{
    static float tickness = 3.f;
    static float position = 0.f;
    static float alpha_text = 1.f;

    ImVec4 circle_loading = ImColor(0, 0, 0, 0);

    position = progress / max * 6.28f;

    ImVec2 vecCenter = { ImGui::GetContentRegionMax() / 2 };

    ImGui::GetForegroundDrawList()->PathClear();
    ImGui::GetForegroundDrawList()->PathArcTo(vecCenter, radius, 0.f, 2.f * IM_PI, 120.f);
    ImGui::GetForegroundDrawList()->PathStroke(ImGui::GetColorU32(circle_loading), 0, tickness);

    ImGui::GetForegroundDrawList()->PathClear();
    ImGui::GetForegroundDrawList()->PathArcTo(vecCenter, radius, IM_PI * 1.5f, IM_PI * 1.5f + position, 120.f);
    ImGui::GetForegroundDrawList()->PathStroke(ImGui::GetColorU32(c::accent), 0, tickness);

    int procent = progress / (int)max * 100;

    std::string procent_str = std::to_string(procent) + "%";

    ImGui::PushFont(font::lexend_regular);

    ImGui::GetForegroundDrawList()->AddText(vecCenter - ImGui::CalcTextSize(procent_str.c_str()) / 2, ImGui::GetColorU32(c::text::text_active), procent_str.c_str());

    alpha_text = ImLerp(alpha_text, procent > 80 ? 0.f : 1.f, ImGui::GetIO().DeltaTime * 6);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha_text);
    if (procent != 0) ImGui::GetForegroundDrawList()->AddText(ImVec2((ImGui::GetContentRegionMax().x - ImGui::CalcTextSize(name).x) / 2, ImGui::GetContentRegionMax().y - alpha_text * 60), ImGui::GetColorU32(c::similarColor), name);
    ImGui::PopStyleVar();

    ImGui::PopFont();

}

HWND hwnd;
RECT rc;



void move_window() {

    ImGui::SetCursorPos(ImVec2(0, 0));
    if (ImGui::InvisibleButton("Move_detector", ImVec2(c::background::size)));
    if (ImGui::IsItemActive()) {

        GetWindowRect(hwnd, &rc);
        MoveWindow(hwnd, rc.left + ImGui::GetMouseDragDelta().x, rc.top + ImGui::GetMouseDragDelta().y, c::background::size.x, c::background::size.y, TRUE);
    }

}

void ShowLastError(const std::string& message) {
    DWORD error = GetLastError();
    std::string fullMessage = message + " | Error Code: " + std::to_string(error);
    // MessageBox(NULL, fullMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
    std::cout << fullMessage << std::endl; // Log to console for now
}

// Drop injected DLL only under %TEMP% with a random name so nothing shows next to the .exe.
// (LoadLibraryW still needs a path on disk; true RAM-only inject would need manual mapping.)
static std::filesystem::path MakeTempInjectDllPath()
{
    wchar_t tmp[MAX_PATH] = {};
    if (GetTempPathW((DWORD)(sizeof(tmp) / sizeof(tmp[0])), tmp) == 0)
        return {};
    unsigned long long u = (unsigned long long)GetTickCount64();
    u ^= (unsigned long long)GetCurrentProcessId() << 20;
    wchar_t fname[72] = {};
    swprintf_s(fname, L"%.16llx.dll", u);
    return std::filesystem::path(tmp) / fname;
}

void ExtractAndWriteDLL(int resourceID, const std::filesystem::path& outputPath) {
    if (outputPath.empty())
        return;

    if (std::filesystem::exists(outputPath)) {
        std::error_code ec;
        std::filesystem::remove(outputPath, ec);
        if (ec) {
            ShowLastError("Failed to delete existing file");
            return;
        }
    }

    HRSRC hRes = FindResource(nullptr, MAKEINTRESOURCE(resourceID), RT_RCDATA);
    if (!hRes) {
        ShowLastError("FindResource failed (rebuild with src/VOID XITERS INTERNAL.dll embedded as RCDATA 101)");
        return;
    }

    HGLOBAL hResData = LoadResource(nullptr, hRes);
    if (!hResData) {
        ShowLastError("LoadResource failed!");
        return;
    }

    DWORD resSize = SizeofResource(nullptr, hRes);
    void* resData = LockResource(hResData);
    if (!resData || resSize == 0) {
        ShowLastError("LockResource failed!");
        return;
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        ShowLastError("Failed to open temp file for DLL extract");
        return;
    }

    outFile.write(static_cast<char*>(resData), resSize);
    outFile.close();
}

void SafeRename(const std::string& from, const std::string& to) {
    std::error_code ec;

    if (std::filesystem::exists(to)) {
        std::filesystem::remove(to, ec);
        if (ec) {
            ShowLastError("Failed to delete old " + to);
            return;
        }
    }

    std::filesystem::rename(from, to, ec);
    if (ec) {
        ShowLastError("Failed to rename " + from + " to " + to);
    }
}

bool InjectDLL1(const std::wstring& fileName, const std::wstring& targetProcessName) {

    PROCESSENTRY32W processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32W);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    if (Process32FirstW(hSnapshot, &processEntry)) {
        do {
            if (_wcsicmp(processEntry.szExeFile, targetProcessName.c_str()) != 0)
                continue;

                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
                if (!hProcess)
                    continue;

                BOOL targetWow64 = FALSE;
                BOOL selfWow64 = FALSE;
                IsWow64Process(hProcess, &targetWow64);
                IsWow64Process(GetCurrentProcess(), &selfWow64);
                if (targetWow64 != selfWow64) {
                    CloseHandle(hProcess);
                    continue;
                }

                LPVOID allocMemAddress = VirtualAllocEx(hProcess, NULL, (fileName.length() + 1) * sizeof(wchar_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                if (!allocMemAddress) {
                    CloseHandle(hProcess);
                    continue;
                }

                if (!WriteProcessMemory(hProcess, allocMemAddress, fileName.c_str(), (fileName.length() + 1) * sizeof(wchar_t), NULL)) {
                    VirtualFreeEx(hProcess, allocMemAddress, 0, MEM_RELEASE);
                    CloseHandle(hProcess);
                    continue;
                }

                HMODULE hModule = GetModuleHandleW(L"kernel32.dll");
                FARPROC loadLibraryAddr = GetProcAddress(hModule, "LoadLibraryW");
                if (!loadLibraryAddr) {
                    VirtualFreeEx(hProcess, allocMemAddress, 0, MEM_RELEASE);
                    CloseHandle(hProcess);
                    continue;
                }

                HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, allocMemAddress, 0, NULL);
                if (!hThread) {
                    VirtualFreeEx(hProcess, allocMemAddress, 0, MEM_RELEASE);
                    CloseHandle(hProcess);
                    continue;
                }

                CloseHandle(hThread);
                CloseHandle(hProcess);
                CloseHandle(hSnapshot);
                return true;
        } while (Process32NextW(hSnapshot, &processEntry));
    }

    CloseHandle(hSnapshot);
    return false;
}
void IntBasicInject()
{
    std::filesystem::path dllPath = MakeTempInjectDllPath();
    if (dllPath.empty()) {
        TriggerConsoleLog("[-] Could not get TEMP folder for DLL");
        return;
    }
    ExtractAndWriteDLL(101, dllPath);
    if (!std::filesystem::exists(dllPath)) {
                TriggerConsoleLog("[-] DLL extract failed — rebuild with RCDATA 101 (src/VOID XITERS INTERNAL.dll at build time)");
        return;
    }

    std::wstring fileName = dllPath.wstring();

    static const wchar_t* bluestacks_exes[] = {
        L"HD-Player.exe",
        L"Bluestacks.exe",
        L"HD-PlayerMultiInstance.exe",
    };

    for (const wchar_t* exe : bluestacks_exes) {
        if (InjectDLL1(fileName, exe)) {
            char nameUtf8[128] = {};
            WideCharToMultiByte(CP_UTF8, 0, exe, -1, nameUtf8, (int)sizeof(nameUtf8) - 1, nullptr, nullptr);
            TriggerConsoleLog(std::string("[+] Injected into ") + nameUtf8);
            TriggerConsoleLog("[+] Internal started successfully...");
            return;
        }
    }

    TriggerConsoleLog("[-] Inject failed: open emulator first (HD-Player / BlueStacks). x64 loader only works with x64 emulator. Run as Administrator.");
}





static std::wstring to_wlower(const std::wstring& s) {
    std::wstring out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::towlower);
    return out;
}

void KillEmulator()
{
    const std::vector<std::wstring> targets = {
        L"hd-player.exe",
        L"hd-adb.exe"
    };

    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateToolhelp32Snapshot failed. Error: " << GetLastError() << '\n';
        return;
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);

    if (!Process32FirstW(snap, &pe)) {
        std::cerr << "Process32FirstW failed. Error: " << GetLastError() << '\n';
        CloseHandle(snap);
        return;
    }

    do {
        std::wstring exeName = to_wlower(pe.szExeFile);
        for (const auto& t : targets) {
            if (exeName == t) {
                DWORD pid = pe.th32ProcessID;
                // Try to open process with terminate rights
                HANDLE hProc = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
                if (!hProc) {
                    // fallback: try with lesser access (on some systems)
                    hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
                }

                if (hProc) {
                    if (TerminateProcess(hProc, 1)) {
                        TriggerConsoleLog("Killed Emulator Successfully....");
                    }
                    else {
                        TriggerConsoleLog("Failed To Kill Emulator");
                    }
                    CloseHandle(hProc);
                }
                else {
                    std::wcerr << L"Unable to open " << pe.szExeFile << L" (PID " << pid
                        << L") for termination. Try running as Administrator. Error: " << GetLastError() << L"\n";
                }
            }
        }
    } while (Process32NextW(snap, &pe));

    CloseHandle(snap);
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    WNDCLASSEXW wc;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = nullptr;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = L"ImGui";
    wc.lpszClassName = L"Example";
    wc.hIconSm = LoadIcon(0, IDI_APPLICATION);

    RegisterClassExW(&wc);
    hwnd = CreateWindowExW(NULL, wc.lpszClassName, L"Void Xiters", WS_POPUP, (GetSystemMetrics(SM_CXSCREEN) / 2) - (c::background::size.x / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (c::background::size.y / 2), c::background::size.x, c::background::size.y, 0, 0, 0, 0);

    SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    POINT mouse;
    rc = { 0 };
    GetWindowRect(hwnd, &rc);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    

    ImFontConfig cfg;

    font::lexend_medium = io.Fonts->AddFontFromMemoryTTF(lexend_medium, sizeof(lexend_medium), 14.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::lexend_regular = io.Fonts->AddFontFromMemoryTTF(lexend_regular, sizeof(lexend_regular), 14.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::lexend_medium_x = io.Fonts->AddFontFromMemoryTTF(lexend_medium, sizeof(lexend_medium), 13.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    for(int i = 14; i <= 40; i++) font::icomoon[i] = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), i, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    if (texture::background == nullptr) CreateShaderResourceViewFromMemory(g_pd3dDevice, background, sizeof(background), &texture::background);
    if (texture::spoof_icon == nullptr) CreateShaderResourceViewFromMemory(g_pd3dDevice, spoof_icon, sizeof(spoof_icon), &texture::spoof_icon);
    if (texture::bluestackoxcy == nullptr) CreateShaderResourceViewFromMemory(g_pd3dDevice, oxcyvloedtack, sizeof(oxcyvloedtack), &texture::bluestackoxcy);
    if (texture::msioxcy == nullptr) CreateShaderResourceViewFromMemory(g_pd3dDevice, msioxcy666, sizeof(msioxcy666), &texture::msioxcy);
    if (texture::logo == nullptr) CreateShaderResourceViewFromMemory(g_pd3dDevice, logo, sizeof(logo), &texture::logo);

    bool done = false;

 

    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done) break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(c::background::size));

            ImGuiStyle* style = &ImGui::GetStyle();

            style->WindowPadding = ImVec2(0, 0);
            style->ItemSpacing = ImVec2(0, 0);
            style->WindowBorderSize = 0;
            style->ScrollbarSize = 7.f;

            Begin("IMGUI", nullptr, ImGuiWindowFlags_NoDecoration);                          
            {
                Trinage_background();

            

                const ImVec2& pos = ImGui::GetWindowPos();
                const ImVec2& region = ImGui::GetContentRegionMax();
                const ImVec2& spacing = style->ItemSpacing;

                if (texture::background)
                    GetBackgroundDrawList()->AddImage(texture::background, ImVec2(0, 0), ImVec2(c::background::size), ImVec2(0, 0), ImVec2(1, 1), ImColor(48, 48, 52, 255));

                static float tab_alpha = 0.f; /* */ static float tab_add; /* */ static int active_tab = 0;

                tab_alpha = ImClamp(tab_alpha + (4.f * ImGui::GetIO().DeltaTime * (tabs == active_tab ? 1.f : -1.f)), 0.f, 1.f);

                if (tab_alpha == 0.f && tab_add == 0.f)
                {
                    timer++;

                    CricleProgress("Connecting....", timer, 100, 60);

                    if (timer > 100) {
                        active_tab = tabs;
                        timer = 0;
                    }
                }

                PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * style->Alpha);

                if (active_tab == 0)
                {
                   
                    
                    edited::BeginChild("Container##1", ImVec2(c::background::size.x / 2, c::background::size.y));
                    {


                        if (edited::Spoof_panel(texture::empty1 , 0 == spoof_page, "INTERNAL PANEL", "Aimbots & Snipers", "", ImVec2(GetContentRegionMax().x - style->WindowPadding.x, 100), ImVec2(44, 44), 0)) {
                            spoof_page = 0;
                         //   current_hwinfo = "Velocira Basic";
                        }

                        //if (edited::Spoof_panel(texture::empty2, 1 == spoof_page, "Internal Max[v1.1.0]", "Brutal Internal Features", "", ImVec2(GetContentRegionMax().x - style->WindowPadding.x, 100), ImVec2(44, 44), 0)) {
                        //    spoof_page = 1;
                        //   // current_hwinfo = "Msi 5.9";
                        //}

                        //if (edited::Spoof_panel(texture::empty3, 2 == spoof_page, "Bypass Setup", "Install One-Time Setup", "", ImVec2(GetContentRegionMax().x - style->WindowPadding.x, 100), ImVec2(44, 44), 0)) {
                        //    spoof_page = 2;
                        //  //  current_hwinfo = "Bluestack 4.240";
                        //}
                    }
                    edited::EndChild();

  
                        PushFont(font::icomoon[40]);
                        GetWindowDrawList()->AddText(pos + ImVec2(c::background::size.x + (c::background::size.x / 2 - CalcTextSize("V").x), 80) / 2, GetColorU32(c::accent), "f");
                        PopFont();

                        GetWindowDrawList()->AddText(pos + ImVec2(c::background::size.x + (c::background::size.x / 2 - CalcTextSize("Void Xiters").x), 190) / 2, GetColorU32(c::accent), "Void Xiters");

                        SetCursorPos(ImVec2(c::background::size.x - (c::background::size.x / 2 + 280) / 2, 110));


                        edited::BeginChild("Container##2", ImVec2(280,  250), ImVec2(0, 15));
                        {
                            static char license_key[128] = { "" };
                            static bool remember = true;
                            static bool remember_loaded = false;
                            if (!remember_loaded) {
                                LoadRememberedLicense(license_key, IM_ARRAYSIZE(license_key), remember);
                                remember_loaded = true;
                            }

                            InputTextEx("a", "License Key", license_key, IM_ARRAYSIZE(license_key), ImVec2(280, 40), NULL);
                            edited::Checkbox("Remember me", &remember);

                            if (edited::Button("LAUNCH", ImVec2(280, 40))) {
                                if (!remember) {
                                    SaveRememberedLicense("", false);
                                }

                                // Handle login based on current spoof page
                                if (spoof_page == 0) {
                                    std::thread taskThread([&]()
                                        {
                                            std::string key = license_key;
                                            TriggerConsoleLog("[+] Connecting to KeyAuth...");

                                            KeyAuthClient::EnsureInit();
                                            {
                                                std::string imsg = KeyAuthClient::Internal.response.message;
                                                if (!KeyAuthClient::Internal.response.success && !imsg.empty()) {
                                                    TriggerConsoleLog("[-] KeyAuth init: " + imsg);
                                                    return;
                                                }
                                            }

                                            KeyAuthClient::Internal.license(key);

                                            if (KeyAuthClient::Internal.response.success) {
                                                TriggerConsoleLog("[+] License key accepted (KeyAuth OK)");
                                                SaveRememberedLicense(license_key, remember);
                                                manual_bypass_logged_in = true;
                                                DisplayedLogs.clear();
                                                ConsoleLogs.clear();
                                            }
                                            else {
                                                std::string msg = KeyAuthClient::Internal.response.message;
                                                if (msg.empty()) msg = "invalid license key, HWID lock, or subscription issue";
                                                TriggerConsoleLog("[-] License check failed: " + msg);
                                            }

                                        });

                                    taskThread.detach();
                                  
                                } 
                                tabs = 1;
                            }
                        
                            SetCursorPosX(GetCursorPosX() + (270 - CalcTextSize("Forgot your passowrd? Restore").x) / 2);
                       

                        }
                        edited::EndChild();

                }
                else if (active_tab == 1)
                {

                    Trinage_background();

                  


                    GetWindowDrawList()->AddRectFilled(pos, pos + ImVec2(c::background::size.x, 50), GetColorU32(c::element::panel), c::element::rounding, ImDrawFlags_RoundCornersTop);
                    GetWindowDrawList()->AddRectFilled(pos + ImVec2(0, 50), pos + ImVec2(100, c::background::size.y), GetColorU32(c::element::panel), c::element::rounding, ImDrawFlags_RoundCornersBottomLeft);

                    PushFont(font::icomoon[22]);
                    GetWindowDrawList()->AddText(pos + (ImVec2(100, 50) - CalcTextSize("f")) / 2, GetColorU32(c::accent), "f");
                    PopFont();

                    GetWindowDrawList()->AddText(pos + (ImVec2(c::background::size.x + 50, 48) - CalcTextSize("Void Xiters")) / 2, GetColorU32(c::accent), "Void Xiters");

                    SetCursorPos(ImVec2((100 - 60) / 2, 70));

                    BeginGroup();
                    {
                        if (edited::page(1 == tabs, "d", ImVec2(60, 60))) tabs = 1;

                        SetCursorPosY(280);

                        if (edited::page(0 == tabs, "c", ImVec2(60, 60))) tabs = 0;
                    }
                    EndGroup();

                    ImGui::SetCursorPos(ImVec2(120, 70));

                    {
                        // Check if user is logged in for the current spoof page
                        bool is_logged_in = false;
                        std::string page_name = "";
                        
                        if (spoof_page == 0 && manual_bypass_logged_in) {
                            is_logged_in = true;
                            page_name = "Silent Aim Max";
                        } else if (spoof_page == 1 && auto_bypass_logged_in) {
                            is_logged_in = true;
                            page_name = "Brutal Max";
                        } /*else if (spoof_page == 2 && setup_logged_in) {
                            is_logged_in = true;
                            page_name = "Setup";
                        }*/
                        
                        if (!is_logged_in) {
                            // Show login prompt
                            edited::BeginChild("Container##1", ImVec2((c::background::size.x - 160) / 2, c::background::size.y - 90), ImVec2(18, 18), true);
                            {
                                edited::info_bar("Status:", "Please login first");
                           
                                edited::Separator_line();
                                
                                TextColored(ImColor(0.8f, 0.8f, 0.8f, 1.0f), "Go back to login page and click LAUNCH");
                                TextColored(ImColor(0.8f, 0.8f, 0.8f, 1.0f), "to access this section.");
                            }
                            edited::EndChild();
                            
                            SameLine(0, 20);
                            
                            edited::BeginChild("Container##2", ImVec2((c::background::size.x - 160) / 2, c::background::size.y - 90), ImVec2(18, 18), true);
                            {
                                RenderConsoleLog();
                            }
                            edited::EndChild();
                        } else {
                            // Show content based on spoof page
                            edited::BeginChild("Container##1", ImVec2((c::background::size.x - 160) / 2, c::background::size.y - 90), ImVec2(18, 18), true);
                            {
                                if (spoof_page == 0) {
                                    // Manual Bypass content
                                    edited::info_bar("Plan:", "Internal");
                                    edited::info_bar("Status:", "Ready");
                                    edited::info_bar("Detection:", "Undetectable");
                                    
                                    edited::Separator_line();
                                 
                                    if (edited::Button("Start Internal", ImVec2(GetContentRegionMax().x - style->WindowPadding.x, 30))) {
                                        std::thread([=]()
                                            {
                                                IntBasicInject();
                                            }).detach();
                                    }
                               
                                    if (edited::Button("Kill Emulator", ImVec2(GetContentRegionMax().x - style->WindowPadding.x, 30))) {
                                        std::thread([=]()
                                            {                                            
                                                KillEmulator();
                                            }).detach();
                                       
                                    }
                                    
                                 
                                } 
                            }
                            edited::EndChild();

                            SameLine(0, 20);

                            edited::BeginChild("Container##2", ImVec2((c::background::size.x - 160) / 2, c::background::size.y - 90), ImVec2(18, 18), true);
                            {
                                RenderConsoleLog();
                            }
                            edited::EndChild();
                        }
                    }
                }
               

                PopStyleVar();

                SetCursorPos(ImVec2(c::background::size.x - 50, 0));
                if (edited::IconButton("k", ImVec2(50, 50), NULL)) {
                    ShowWindow(hwnd, SW_HIDE);
                    PostQuitMessage(0);
                }

                draw_circles_and_lines(GetColorU32(c::accent, 0.2f));
                move_window();

            }
            End();
        
        }
        Render();

        const float clear_color_with_alpha[4] = { 0.f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }
    
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}


bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
