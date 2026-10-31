#include <windows.h>
#include <commctrl.h> // Библиотека для работы с ToolBar
#include "resource.h"
#include "rect_editor.h"
#include "ellipse_editor.h"

// Подключаем стили для современных кнопок Windows
#pragma comment(lib, "comctl32.lib")

static Shape* pcshape[105]; // Массив на 105 элементов
static int shapeCount = 0;

static RectEditor rectEditor;
static EllipseEditor ellipseEditor;

static bool isRectToolSelected = true;
static HWND hWndToolbar = NULL;

// Функция обновления заголовка окна по 5-му варианту
void UpdateWindowTitle(HWND hWnd) {
    if (isRectToolSelected) {
        SetWindowTextW(hWnd, L"Lab 3 [Current tool: Rectangle]");
    }
    else {
        SetWindowTextW(hWnd, L"Lab 3 [Current tool: Ellipse]");
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // Инициализация библиотеки элементов управления
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_BAR_CLASSES;
        InitCommonControlsEx(&icex);

        // Описание кнопок панели инструментов (ToolBar)
        TBBUTTON tbb[2];
        ZeroMemory(tbb, sizeof(tbb));

        // Кнопка 1: Прямоугольник (используем стандартную системную иконку STD_FILENEW)
        tbb[0].iBitmap = STD_FILENEW;
        tbb[0].idCommand = IDM_RECTANGLE;
        tbb[0].fsState = TBSTATE_ENABLED | TBSTATE_CHECKED; // Зажата по умолчанию
        tbb[0].fsStyle = TBSTYLE_CHECKGROUP;

        // Кнопка 2: Эллипс (системная иконка STD_FILEOPEN)
        tbb[1].iBitmap = STD_FILEOPEN;
        tbb[1].idCommand = IDM_ELLIPSE;
        tbb[1].fsState = TBSTATE_ENABLED;
        tbb[1].fsStyle = TBSTYLE_CHECKGROUP;

        // Создаем ToolBar с поддержкой всплывающих подсказок (TBSTYLE_TOOLTIPS)
        hWndToolbar = CreateToolbarEx(
            hWnd,
            WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
            IDR_TOOLBAR,
            2,
            HINST_COMMCTRL,
            IDB_STD_SMALL_COLOR,
            tbb,
            2,
            0, 0, 0, 0,
            sizeof(TBBUTTON)
        );
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_RECTANGLE:
            isRectToolSelected = true;
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_RECTANGLE, TRUE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_ELLIPSE, FALSE);
            UpdateWindowTitle(hWnd);
            break;

        case IDM_ELLIPSE:
            isRectToolSelected = false;
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_RECTANGLE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_ELLIPSE, TRUE);
            UpdateWindowTitle(hWnd);
            break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    // Обработка всплывающих подсказок (Tooltips)
    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (pnmh->code == TTN_GETDISPINFO) {
            LPNMTTDISPINFO pDi = (LPNMTTDISPINFO)lParam;
            if (pDi->hdr.idFrom == IDM_RECTANGLE) {
                pDi->lpszText = (LPWSTR)L"Rectangle tool";
            }
            else if (pDi->hdr.idFrom == IDM_ELLIPSE) {
                pDi->lpszText = (LPWSTR)L"Ellipse tool";
            }
        }
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
    const wchar_t CLASS_NAME[] = L"Lab3WindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);

    RegisterClassW(&wc);

    HMENU hMenu = LoadMenuW(hInstance, MAKEINTRESOURCEW(IDR_MYMENU));

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, L"Lab 3 [Current tool: Rectangle]",
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
