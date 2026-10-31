#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#include "../MyEditorDll/rect_editor.h"
#include "../MyEditorDll/ellipse_editor.h"
#include "../MyEditorDll/line_oo_editor.h"
#include "../MyEditorDll/cube_editor.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "../x64/Debug/MyEditorDll.lib")

static Shape* pcshape[105];
static int shapeCount = 0;

static RectEditor rectEditor;
static EllipseEditor ellipseEditor;
static LineOOEditor lineOOEditor;
static CubeEditor cubeEditor;

enum ToolType { TOOL_RECT, TOOL_ELLIPSE, TOOL_LINEOO, TOOL_CUBE };
static ToolType currentTool = TOOL_RECT;

static HWND hWndToolbar = NULL;

void UpdateWindowTitle(HWND hWnd) {
    switch (currentTool) {
    case TOOL_RECT: SetWindowTextW(hWnd, L"Lab 4 [Current tool: Rectangle]"); break;
    case TOOL_ELLIPSE: SetWindowTextW(hWnd, L"Lab 4 [Current tool: Ellipse]"); break;
    case TOOL_LINEOO: SetWindowTextW(hWnd, L"Lab 4 [Current tool: Line with Circles]"); break;
    case TOOL_CUBE: SetWindowTextW(hWnd, L"Lab 4 [Current tool: Cube]"); break;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_BAR_CLASSES;
        InitCommonControlsEx(&icex);

        TBBUTTON tbb[4];
        ZeroMemory(tbb, sizeof(tbb));

        tbb[0].iBitmap = STD_FILENEW;
        tbb[0].idCommand = IDM_RECTANGLE;
        tbb[0].fsState = TBSTATE_ENABLED | TBSTATE_CHECKED;
        tbb[0].fsStyle = TBSTYLE_CHECKGROUP;

        tbb[1].iBitmap = STD_FILEOPEN;
        tbb[1].idCommand = IDM_ELLIPSE;
        tbb[1].fsState = TBSTATE_ENABLED;
        tbb[1].fsStyle = TBSTYLE_CHECKGROUP;

        tbb[2].iBitmap = STD_FILESAVE;
        tbb[2].idCommand = IDM_LINEOO;
        tbb[2].fsState = TBSTATE_ENABLED;
        tbb[2].fsStyle = TBSTYLE_CHECKGROUP;

        tbb[3].iBitmap = STD_PRINTPRE;
        tbb[3].idCommand = IDM_CUBE;
        tbb[3].fsState = TBSTATE_ENABLED;
        tbb[3].fsStyle = TBSTYLE_CHECKGROUP;

        hWndToolbar = CreateToolbarEx(
            hWnd,
            WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
            IDR_TOOLBAR,
            4,
            HINST_COMMCTRL,
            IDB_STD_SMALL_COLOR,
            tbb,
            4,
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
            currentTool = TOOL_RECT;
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_RECTANGLE, TRUE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_ELLIPSE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_LINEOO, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_CUBE, FALSE);
            UpdateWindowTitle(hWnd);
            break;

        case IDM_ELLIPSE:
            currentTool = TOOL_ELLIPSE;
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_RECTANGLE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_ELLIPSE, TRUE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_LINEOO, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_CUBE, FALSE);
            UpdateWindowTitle(hWnd);
            break;

        case IDM_LINEOO:
            currentTool = TOOL_LINEOO;
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_RECTANGLE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_ELLIPSE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_LINEOO, TRUE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_CUBE, FALSE);
            UpdateWindowTitle(hWnd);
            break;

        case IDM_CUBE:
            currentTool = TOOL_CUBE;
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_RECTANGLE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_ELLIPSE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_LINEOO, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_CUBE, TRUE);
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

    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (pnmh->code == TTN_GETDISPINFO) {
            LPNMTTDISPINFO pDi = (LPNMTTDISPINFO)lParam;
            if (pDi->hdr.idFrom == IDM_RECTANGLE) pDi->lpszText = (LPWSTR)L"Rectangle tool";
            else if (pDi->hdr.idFrom == IDM_ELLIPSE) pDi->lpszText = (LPWSTR)L"Ellipse tool";
            else if (pDi->hdr.idFrom == IDM_LINEOO) pDi->lpszText = (LPWSTR)L"Line with Circles tool";
            else if (pDi->hdr.idFrom == IDM_CUBE) pDi->lpszText = (LPWSTR)L"Cube tool";
        }
    }
    break;

    case WM_LBUTTONDOWN:
        if (currentTool == TOOL_RECT) rectEditor.OnLBdown(hWnd, lParam);
        else if (currentTool == TOOL_ELLIPSE) ellipseEditor.OnLBdown(hWnd, lParam);
        else if (currentTool == TOOL_LINEOO) lineOOEditor.OnLBdown(hWnd, lParam);
        else if (currentTool == TOOL_CUBE) cubeEditor.OnLBdown(hWnd, lParam);
        break;

    case WM_MOUSEMOVE:
        if (currentTool == TOOL_RECT) rectEditor.OnMouseMove(hWnd, lParam);
        else if (currentTool == TOOL_ELLIPSE) ellipseEditor.OnMouseMove(hWnd, lParam);
        else if (currentTool == TOOL_LINEOO) lineOOEditor.OnMouseMove(hWnd, lParam);
        else if (currentTool == TOOL_CUBE) cubeEditor.OnMouseMove(hWnd, lParam);
        break;

    case WM_LBUTTONUP:
        if (currentTool == TOOL_RECT) rectEditor.OnLBup(hWnd, lParam, pcshape, shapeCount);
        else if (currentTool == TOOL_ELLIPSE) ellipseEditor.OnLBup(hWnd, lParam, pcshape, shapeCount);
        else if (currentTool == TOOL_LINEOO) lineOOEditor.OnLBup(hWnd, lParam, pcshape, shapeCount);
        else if (currentTool == TOOL_CUBE) cubeEditor.OnLBup(hWnd, lParam, pcshape, shapeCount);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        for (int i = 0; i < shapeCount; i++) {
            if (pcshape[i]) pcshape[i]->Draw(hdc);
        }

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        for (int i = 0; i < shapeCount; i++) delete pcshape[i];
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Lab4WindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);

    RegisterClassW(&wc);

    HMENU hMenu = LoadMenuW(hInstance, MAKEINTRESOURCEW(IDR_MYMENU));

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, L"Lab 4 [Current tool: Rectangle]",
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
