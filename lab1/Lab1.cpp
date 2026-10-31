#include <windows.h>
#include <string>
#include "resource.h"
#include "module1.h"
#include "module2.h"

static std::wstring g_displayText = L"Результат виводу буде тут";

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_WORK1:
        {
            std::wstring resultText;
            if (ShowDialog1(hWnd, resultText))
            {
                g_displayText = L"Текст з Робота1: " + resultText;
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        break;

        case IDM_WORK2:
        {
            int val = 0;
            if (ShowDialog2(hWnd, val))
            {
                g_displayText = L"Число з Робота2: " + std::to_wstring(val);
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

        default:
            return DefWindowProcW(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_SIZE:
        // Перерисовываем окно при изменении размера, чтобы текст пересчитывал центр
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rect;
        GetClientRect(hWnd, &rect);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));

        // Отрисовка текста строго по центру окна
        DrawTextW(hdc, g_displayText.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Lab1WindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    HMENU hMenu = LoadMenuW(hInstance, MAKEINTRESOURCEW(IDR_MYMENU));

    if (!hMenu)
    {
        HMENU hSubMenu = CreatePopupMenu();
        AppendMenuW(hSubMenu, MF_STRING, IDM_WORK1, L"Робота1");
        AppendMenuW(hSubMenu, MF_STRING, IDM_WORK2, L"Робота2");
        AppendMenuW(hSubMenu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(hSubMenu, MF_STRING, IDM_EXIT, L"Вихід");

        hMenu = CreateMenu();
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"Меню");
    }

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, L"Лабораторна робота №1",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL, hMenu, hInstance, NULL
    );

    if (hWnd == NULL) return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}
