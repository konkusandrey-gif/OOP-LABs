#include <windows.h>
#include "resource.h"
#include "rect_editor.h"
#include "ellipse_editor.h"

static Shape* pcshape[104];
static int shapeCount = 0;

static RectEditor rectEditor;
static EllipseEditor ellipseEditor;

static bool isRectToolSelected = true; // true = Прямоугольник, false = Эллипс

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_RECTANGLE:
            isRectToolSelected = true;
            break;

        case IDM_ELLIPSE:
            isRectToolSelected = false;
            break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    // Требование 4-го варианта: индикация выбора инструмента галочкой в меню
    case WM_INITMENUPOPUP:
    {
        HMENU hMenu = (HMENU)wParam;
        CheckMenuItem(hMenu, IDM_RECTANGLE, isRectToolSelected ? MF_CHECKED : MF_UNCHECKED);
        CheckMenuItem(hMenu, IDM_ELLIPSE, !isRectToolSelected ? MF_CHECKED : MF_UNCHECKED);
    }
    break;

    case WM_LBUTTONDOWN:
        if (isRectToolSelected) rectEditor.OnLBdown(hWnd, lParam);
        else ellipseEditor.OnLBdown(hWnd, lParam);
        break;

    case WM_MOUSEMOVE:
        if (isRectToolSelected) rectEditor.OnMouseMove(hWnd, lParam);
        else ellipseEditor.OnMouseMove(hWnd, lParam);
        break;

    case WM_LBUTTONUP:
        if (isRectToolSelected) rectEditor.OnLBup(hWnd, lParam, pcshape, shapeCount);
        else ellipseEditor.OnLBup(hWnd, lParam, pcshape, shapeCount);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        for (int i = 0; i < shapeCount; i++) {
            if (pcshape[i]) {
                pcshape[i]->Draw(hdc);
            }
        }

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        for (int i = 0; i < shapeCount; i++) {
            delete pcshape[i];
        }
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Lab2WindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);

    RegisterClassW(&wc);

    HMENU hMenu = LoadMenuW(hInstance, MAKEINTRESOURCEW(IDR_MYMENU));

    if (!hMenu)
    {
        HMENU hSubMenu = CreatePopupMenu();
        AppendMenuW(hSubMenu, MF_STRING, IDM_RECTANGLE, L"Rectangle");
        AppendMenuW(hSubMenu, MF_STRING, IDM_ELLIPSE, L"Ellipse");
        AppendMenuW(hSubMenu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(hSubMenu, MF_STRING, IDM_EXIT, L"Exit");

        hMenu = CreateMenu();
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"Shapes");
    }

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, L"Lab 2",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, hMenu, hInstance, NULL
    );

    if (hWnd == NULL) return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
