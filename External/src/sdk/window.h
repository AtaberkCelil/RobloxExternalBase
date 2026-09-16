#pragma once

#include <windows.h>

namespace WindowUtil {

    struct SearchData {
        DWORD pid;
        HWND result;
    };

    inline BOOL CALLBACK FindByProcess(HWND hwnd, LPARAM param) {
        auto* data = reinterpret_cast<SearchData*>(param);
        DWORD windowPid = 0;
        GetWindowThreadProcessId(hwnd, &windowPid);

        if (windowPid == data->pid && IsWindowVisible(hwnd) && GetWindow(hwnd, GW_OWNER) == nullptr) {
            data->result = hwnd;
            return FALSE;
        }
        return TRUE;
    }

    inline HWND FindForProcess(DWORD pid) {
        if (!pid)
            return nullptr;

        SearchData data{ pid, nullptr };
        EnumWindows(FindByProcess, reinterpret_cast<LPARAM>(&data));
        return data.result;
    }
}
