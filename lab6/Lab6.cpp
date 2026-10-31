#include <windows.h>
#include <string>

#define ID_BTN_START 200
#define ID_BTN_EXIT  201

// Идентификаторы полей ввода в диалоге
#define ID_EDIT_NPOINT 301
#define ID_EDIT_XMIN   302
#define ID_EDIT_XMAX   303
#define ID_EDIT_YMIN   304
#define ID_EDIT_YMAX   305
#define ID_BTN_DLG_OK  306

struct Params {
    int nPoint;
    int xMin;
    int xMax;
    int yMin;
    int yMax;
};

PROCESS_INFORMATION pi2 = {};
PROCESS_INFORMATION pi3 = {};
bool proc2Running = false;
bool proc3Running = false;

HWND g_hBtnStart = NULL;
HWND g_hBtnExit = NULL;
Params g_currentParams = { 10, -50, 50, -100, 100 };

bool StartChildProcess(const wchar_t* exeName, PROCESS_INFORMATION& pi) {
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    wchar_t cmd[260];
    wcscpy_s(cmd, exeName);

    if (CreateProcessW(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        return true;
    }
    return false;
}

void CloseChildProcesses() {
    HWND hObj2 = FindWindowW(NULL, L"Object2");
    HWND hObj3 = FindWindowW(NULL, L"Object3");

    if (hObj2) PostMessageW(hObj2, WM_CLOSE, 0, 0);
    if (hObj3) PostMessageW(hObj3, WM_CLOSE, 0, 0);

    if (proc2Running) { CloseHandle(pi2.hProcess); CloseHandle(pi2.hThread); proc2Running = false; }
    if (proc3Running) { CloseHandle(pi3.hProcess); CloseHandle(pi3.hThread); proc3Running = false; }
}

void CenterControls(HWND hWnd, int width, int height) {
    int btnWidth = 320;
    int btnHeight = 45;
    int spacing = 15;

    int totalHeight = btnHeight * 2 + spacing;
    int startX = (width - btnWidth) / 2;
    int startY = (height - totalHeight) / 2;

    if (startX < 10) startX = 10;
    if (startY < 10) startY = 10;

    if (g_hBtnStart) MoveWindow(g_hBtnStart, startX, startY, btnWidth, btnHeight, TRUE);
    if (g_hBtnExit) MoveWindow(g_hBtnExit, startX, startY + btnHeight + spacing, btnWidth, btnHeight, TRUE);
}

// -------------------------------------------------------------
// Процедура для модального окна ввода параметров
// -------------------------------------------------------------
LRESULT CALLBACK DlgWndProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hN, hXMin, hXMax, hYMin, hYMax;

    switch (msg) {
    case WM_CREATE: {
        // Метки и поля ввода
        CreateWindowW(L"STATIC", L"Кількість точок (nPoint):", WS_VISIBLE | WS_CHILD, 20, 20, 200, 20, hDlg, NULL, NULL, NULL);
        hN = CreateWindowW(L"EDIT", std::to_wstring(g_currentParams.nPoint).c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 220, 18, 100, 24, hDlg, (HMENU)ID_EDIT_NPOINT, NULL, NULL);

        CreateWindowW(L"STATIC", L"X Min:", WS_VISIBLE | WS_CHILD, 20, 60, 80, 20, hDlg, NULL, NULL, NULL);
        hXMin = CreateWindowW(L"EDIT", std::to_wstring(g_currentParams.xMin).c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 58, 80, 24, hDlg, (HMENU)ID_EDIT_XMIN, NULL, NULL);

        CreateWindowW(L"STATIC", L"X Max:", WS_VISIBLE | WS_CHILD, 190, 60, 80, 20, hDlg, NULL, NULL, NULL);
        hXMax = CreateWindowW(L"EDIT", std::to_wstring(g_currentParams.xMax).c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER, 260, 58, 80, 24, hDlg, (HMENU)ID_EDIT_XMAX, NULL, NULL);

        CreateWindowW(L"STATIC", L"Y Min:", WS_VISIBLE | WS_CHILD, 20, 100, 80, 20, hDlg, NULL, NULL, NULL);
        hYMin = CreateWindowW(L"EDIT", std::to_wstring(g_currentParams.yMin).c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 98, 80, 24, hDlg, (HMENU)ID_EDIT_YMIN, NULL, NULL);

        CreateWindowW(L"STATIC", L"Y Max:", WS_VISIBLE | WS_CHILD, 190, 100, 80, 20, hDlg, NULL, NULL, NULL);
        hYMax = CreateWindowW(L"EDIT", std::to_wstring(g_currentParams.yMax).c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER, 260, 98, 80, 24, hDlg, (HMENU)ID_EDIT_YMAX, NULL, NULL);

        // Кнопка подтверждения
        CreateWindowW(L"BUTTON", L"Виконати", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 110, 145, 140, 35, hDlg, (HMENU)ID_BTN_DLG_OK, NULL, NULL);
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == ID_BTN_DLG_OK) {
            wchar_t buf[32];

            GetWindowTextW(hN, buf, 32);     g_currentParams.nPoint = _wtoi(buf);
            GetWindowTextW(hXMin, buf, 32);  g_currentParams.xMin = _wtoi(buf);
            GetWindowTextW(hXMax, buf, 32);  g_currentParams.xMax = _wtoi(buf);
            GetWindowTextW(hYMin, buf, 32);  g_currentParams.yMin = _wtoi(buf);
            GetWindowTextW(hYMax, buf, 32);  g_currentParams.yMax = _wtoi(buf);

            // Валидация
            if (g_currentParams.nPoint <= 0) g_currentParams.nPoint = 10;
            if (g_currentParams.xMin >= g_currentParams.xMax) { g_currentParams.xMin = -50; g_currentParams.xMax = 50; }
            if (g_currentParams.yMin >= g_currentParams.yMax) { g_currentParams.yMin = -100; g_currentParams.yMax = 100; }

            PostMessageW(hDlg, WM_CLOSE, 0, 0);
        }
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hDlg);
        break;
    default:
        return DefWindowProcW(hDlg, msg, wParam, lParam);
    }
    return 0;
}

// Показ окна ввода
void ShowInputDialog(HWND hParent) {
    const wchar_t DLG_CLASS[] = L"InputDialogClass";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = DlgWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = DLG_CLASS;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    HWND hDlg = CreateWindowExW(
        WS_EX_DLGMODALFRAME, DLG_CLASS, L"Параметри генерації",
        WS_POPUP | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 370, 230,
        hParent, NULL, GetModuleHandle(NULL), NULL
    );

    RECT rectParent, rectDlg;
    GetWindowRect(hParent, &rectParent);
    GetWindowRect(hDlg, &rectDlg);
    int x = rectParent.left + (rectParent.right - rectParent.left - (rectDlg.right - rectDlg.left)) / 2;
    int y = rectParent.top + (rectParent.bottom - rectParent.top - (rectDlg.bottom - rectDlg.top)) / 2;
    SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    EnableWindow(hParent, FALSE); // Делаем главное окно модально заблокированным
    ShowWindow(hDlg, SW_SHOW);

    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (!IsWindow(hDlg)) break;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    EnableWindow(hParent, TRUE);
    SetActiveWindow(hParent);
}

// -------------------------------------------------------------
// Главная процедура окна Lab6
// -------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        g_hBtnStart = CreateWindowW(L"BUTTON", L"Запустити генерацію та графік",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            0, 0, 0, 0, hWnd, (HMENU)ID_BTN_START, GetModuleHandle(NULL), NULL);

        g_hBtnExit = CreateWindowW(L"BUTTON", L"Вихід",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            0, 0, 0, 0, hWnd, (HMENU)ID_BTN_EXIT, GetModuleHandle(NULL), NULL);
        break;
    }
    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        CenterControls(hWnd, width, height);
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == ID_BTN_START) {
            // 1. Показываем окно диалога для ввода 5 параметров
            ShowInputDialog(hWnd);

            // 2. Запускаем процессы Object2 и Object3
            if (!proc2Running) proc2Running = StartChildProcess(L"Object2.exe", pi2);
            if (!proc3Running) proc3Running = StartChildProcess(L"Object3.exe", pi3);

            Sleep(300);

            HWND hObj2 = FindWindowW(NULL, L"Object2");
            HWND hObj3 = FindWindowW(NULL, L"Object3");

            if (hObj2 && hObj3) {
                MoveWindow(hObj2, 50, 100, 400, 500, TRUE);
                MoveWindow(hObj3, 470, 100, 600, 500, TRUE);
            }

            // 3. Передаем введенные параметры в Object2 через WM_COPYDATA
            if (hObj2) {
                COPYDATASTRUCT cds;
                cds.dwData = 1;
                cds.cbData = sizeof(Params);
                cds.lpData = &g_currentParams;
                SendMessageW(hObj2, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
            }
        }
        else if (LOWORD(wParam) == ID_BTN_EXIT) {
            DestroyWindow(hWnd);
        }
        break;
    }
    case WM_USER + 2: {
        HWND hObj3 = FindWindowW(NULL, L"Object3");
        if (hObj3) {
            PostMessageW(hObj3, WM_USER + 1, 0, 0);
        }
        break;
    }
    case WM_CLOSE: {
        DestroyWindow(hWnd);
        break;
    }
    case WM_DESTROY: {
        CloseChildProcesses();
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"Lab6WindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, L"Lab6 - Менеджер",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 420, 220,
        NULL, NULL, hInstance, NULL
    );

    if (!hWnd) return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}
