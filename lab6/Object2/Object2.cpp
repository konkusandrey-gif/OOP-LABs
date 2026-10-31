#include <windows.h>
#include <string>
#include <vector>
#include <random>
#include <sstream>
#include <fstream>
#include <commdlg.h>

#define ID_BTN_SAVE 301

struct Params {
    int nPoint;
    int xMin;
    int xMax;
    int yMin;
    int yMax;
};

struct Point {
    int x, y;
};

std::vector<Point> g_points;
std::wstring g_displayText = L"Ожидание параметров от Lab6...";
HWND g_hBtnSave = NULL;

void CopyToClipboard(HWND hWnd, const std::string& text) {
    if (!OpenClipboard(hWnd)) return;
    EmptyClipboard();

    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (hGlob) {
        char* pBuf = (char*)GlobalLock(hGlob);
        if (pBuf) {
            memcpy(pBuf, text.c_str(), text.size() + 1);
            GlobalUnlock(hGlob);
            SetClipboardData(CF_TEXT, hGlob);
        }
    }
    CloseClipboard();
}

void SavePointsToFile(HWND hWnd) {
    if (g_points.empty()) {
        MessageBoxW(hWnd, L"Нет сгенерированных точек для сохранения!", L"Ошибка", MB_OK | MB_ICONWARNING);
        return;
    }

    wchar_t szFile[260] = L"points.csv";
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"CSV Files (*.csv)\0*.csv\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = L"csv";

    if (GetSaveFileNameW(&ofn)) {
        std::ofstream file(ofn.lpstrFile);
        if (file.is_open()) {
            file << "Index,X,Y\n";
            for (size_t i = 0; i < g_points.size(); ++i) {
                file << (i + 1) << "," << g_points[i].x << "," << g_points[i].y << "\n";
            }
            file.close();
            MessageBoxW(hWnd, L"Точки успешно сохранены!", L"Сохранение", MB_OK | MB_ICONINFORMATION);
        }
    }
}

void GenerateAndProcess(HWND hWnd, const Params& params) {
    g_points.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distX(params.xMin, params.xMax);
    std::uniform_int_distribution<> distY(params.yMin, params.yMax);

    std::ostringstream ssClipboard;
    std::wostringstream ssDisplay;

    ssDisplay << L"Сгенерировано точек: " << params.nPoint << L"\n\n";

    for (int i = 0; i < params.nPoint; ++i) {
        Point p;
        p.x = distX(gen);
        p.y = distY(gen);
        g_points.push_back(p);

        ssClipboard << p.x << " " << p.y << "\n";
        ssDisplay << L"Точка " << (i + 1) << L": (" << p.x << L", " << p.y << L")\n";
    }

    g_displayText = ssDisplay.str();

    CopyToClipboard(hWnd, ssClipboard.str());
    InvalidateRect(hWnd, NULL, TRUE);

    HWND hLab6 = FindWindowW(NULL, L"Lab6 - Менеджер");
    if (!hLab6) hLab6 = FindWindowW(NULL, L"Lab6");

    if (hLab6) {
        PostMessageW(hLab6, WM_USER + 2, 0, 0);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        g_hBtnSave = CreateWindowW(L"BUTTON", L"Сохранить точки в файл",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            15, 400, 220, 35, hWnd, (HMENU)ID_BTN_SAVE, GetModuleHandle(NULL), NULL);
        break;
    }
    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        // Привязываем кнопку к левому нижнему углу
        if (g_hBtnSave) {
            MoveWindow(g_hBtnSave, 15, height - 45, 220, 35, TRUE);
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == ID_BTN_SAVE) {
            SavePointsToFile(hWnd);
        }
        break;
    }
    case WM_COPYDATA: {
        PCOPYDATASTRUCT pCDS = (PCOPYDATASTRUCT)lParam;
        if (pCDS && pCDS->cbData == sizeof(Params)) {
            Params* pParams = (Params*)pCDS->lpData;
            GenerateAndProcess(hWnd, *pParams);
        }
        return TRUE;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rect;
        GetClientRect(hWnd, &rect);
        rect.left += 15;
        rect.top += 15;
        rect.bottom -= 55; // оставляем место снизу

        SetBkMode(hdc, TRANSPARENT);
        DrawTextW(hdc, g_displayText.c_str(), -1, &rect, DT_LEFT | DT_TOP);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"Object2WindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, L"Object2",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 500,
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
