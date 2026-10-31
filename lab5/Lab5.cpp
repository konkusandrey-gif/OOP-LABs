#include <windows.h>
#define _WIN32_IE 0x0500
#include <commctrl.h>
#include <fstream>
#include "resource.h"
#include "my_table.h"

#include "rect_editor.h"
#include "ellipse_editor.h"
#include "line_oo_editor.h"
#include "cube_editor.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "MyEditorDll.lib")

class MyEditor {
public:
    enum ToolType { TOOL_RECT, TOOL_ELLIPSE, TOOL_LINEOO, TOOL_CUBE };

private:
    static MyEditor* pInstance;

    Shape* pcshape[105];
    int shapeCount;

    RectEditor rectEditor;
    EllipseEditor ellipseEditor;
    LineOOEditor line00Editor;
    CubeEditor cubeEditor;

    ToolType currentTool;
    MyTable tableWindow;

    MyEditor() {
        shapeCount = 0;
        currentTool = TOOL_RECT;
        for (int i = 0; i < 105; i++) pcshape[i] = nullptr;
    }

public:
    static MyEditor* getInstance() {
        if (!pInstance) {
            pInstance = new MyEditor();
        }
        return pInstance;
    }

    static void destroyInstance() {
        if (pInstance) {
            delete pInstance;
            pInstance = nullptr;
        }
    }

    ~MyEditor() {
        for (int i = 0; i < shapeCount; i++) {
            if (pcshape[i]) delete pcshape[i];
        }
    }

    void SetTool(ToolType tool) { currentTool = tool; }
    ToolType GetTool() const { return currentTool; }

    void OnLBdown(HWND hWnd, LPARAM lParam) {
        if (currentTool == TOOL_RECT) rectEditor.OnLBdown(hWnd, lParam);
        else if (currentTool == TOOL_ELLIPSE) ellipseEditor.OnLBdown(hWnd, lParam);
        else if (currentTool == TOOL_LINEOO) line00Editor.OnLBdown(hWnd, lParam);
        else if (currentTool == TOOL_CUBE) cubeEditor.OnLBdown(hWnd, lParam);
    }

    void OnMouseMove(HWND hWnd, LPARAM lParam) {
        if (currentTool == TOOL_RECT) rectEditor.OnMouseMove(hWnd, lParam);
        else if (currentTool == TOOL_ELLIPSE) ellipseEditor.OnMouseMove(hWnd, lParam);
        else if (currentTool == TOOL_LINEOO) line00Editor.OnMouseMove(hWnd, lParam);
        else if (currentTool == TOOL_CUBE) cubeEditor.OnMouseMove(hWnd, lParam);
    }

    void OnLBup(HWND hWnd, LPARAM lParam) {
        if (currentTool == TOOL_RECT) rectEditor.OnLBup(hWnd, lParam, pcshape, shapeCount);
        else if (currentTool == TOOL_ELLIPSE) ellipseEditor.OnLBup(hWnd, lParam, pcshape, shapeCount);
        else if (currentTool == TOOL_LINEOO) line00Editor.OnLBup(hWnd, lParam, pcshape, shapeCount);
        else if (currentTool == TOOL_CUBE) cubeEditor.OnLBup(hWnd, lParam, pcshape, shapeCount);

        tableWindow.Update(pcshape, shapeCount);
    }

    void OnPaint(HDC hdc) {
        for (int i = 0; i < shapeCount; i++) {
            if (pcshape[i]) pcshape[i]->Draw(hdc);
        }
    }

    void ToggleTable(HWND hWndParent) {
        tableWindow.Show(hWndParent);
        tableWindow.Update(pcshape, shapeCount);
    }

    void SaveToCSV(HWND hWndParent) {
        wchar_t szFile[260] = L"shapes.csv";

        OPENFILENAMEW ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hWndParent;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
        ofn.lpstrFilter = L"CSV Files (*.csv)\0*.csv\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
        ofn.lpstrDefExt = L"csv";

        if (GetSaveFileNameW(&ofn)) {
            std::ofstream file(ofn.lpstrFile);
            if (file.is_open()) {
                file << "Name,x1,y1,x2,y2\n";
                for (int i = 0; i < shapeCount; i++) {
                    if (pcshape[i]) {
                        file << "Object,"
                            << pcshape[i]->GetX1() << ","
                            << pcshape[i]->GetY1() << ","
                            << pcshape[i]->GetX2() << ","
                            << pcshape[i]->GetY2() << "\n";
                    }
                }
                file.close();
                MessageBoxW(hWndParent, L"Об'єкти успішно збережено!", L"Повідомлення", MB_OK | MB_ICONINFORMATION);
            }
        }
    }

    void LoadFromCSV(HWND hWndParent) {
        wchar_t szFile[260] = L"";

        OPENFILENAMEW ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hWndParent;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
        ofn.lpstrFilter = L"CSV Files (*.csv)\0*.csv\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameW(&ofn)) {
            std::ifstream file(ofn.lpstrFile);
            if (file.is_open()) {
                file.close();
                tableWindow.Update(pcshape, shapeCount);
                InvalidateRect(hWndParent, NULL, TRUE);
                MessageBoxW(hWndParent, L"Файл прочитан!", L"Загрузка", MB_OK | MB_ICONINFORMATION);
            }
        }
    }
};

MyEditor* MyEditor::pInstance = nullptr;

static HWND hWndToolbar = NULL;

void UpdateWindowTitle(HWND hWnd) {
    MyEditor* editor = MyEditor::getInstance();
    switch (editor->GetTool()) {
    case MyEditor::TOOL_RECT: SetWindowTextW(hWnd, L"Lab 5 [Current tool: Rectangle]"); break;
    case MyEditor::TOOL_ELLIPSE: SetWindowTextW(hWnd, L"Lab 5 [Current tool: Ellipse]"); break;
    case MyEditor::TOOL_LINEOO: SetWindowTextW(hWnd, L"Lab 5 [Current tool: Line with Circles]"); break;
    case MyEditor::TOOL_CUBE: SetWindowTextW(hWnd, L"Lab 5 [Current tool: Cube]"); break;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MyEditor* editor = MyEditor::getInstance();

    switch (message)
    {
    case WM_CREATE:
    {
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES;
        InitCommonControlsEx(&icex);

        hWndToolbar = CreateWindowExW(0, TOOLBARCLASSNAMEW, NULL,
            WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
            0, 0, 0, 0, hWnd, (HMENU)100, GetModuleHandle(NULL), NULL);

        SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

        TBADDBITMAP tbab;
        tbab.hInst = HINST_COMMCTRL;
        tbab.nID = 0;
        SendMessage(hWndToolbar, TB_ADDBITMAP, 0, (LPARAM)&tbab);

        TBBUTTON tbb[7] = {};

        tbb[0].iBitmap = STD_FILENEW;
        tbb[0].idCommand = IDM_SAVE; // Используем существующий IDM_SAVE
        tbb[0].fsState = TBSTATE_ENABLED;
        tbb[0].fsStyle = BTNS_BUTTON;

        tbb[1].iBitmap = STD_FILESAVE;
        tbb[1].idCommand = IDM_SAVE;
        tbb[1].fsState = TBSTATE_ENABLED;
        tbb[1].fsStyle = BTNS_BUTTON;

        tbb[2].iBitmap = STD_PROPERTIES;
        tbb[2].idCommand = IDM_TABLE;
        tbb[2].fsState = TBSTATE_ENABLED;
        tbb[2].fsStyle = BTNS_BUTTON;

        tbb[3].iBitmap = 0;
        tbb[3].idCommand = IDM_RECTANGLE;
        tbb[3].fsState = TBSTATE_ENABLED | TBSTATE_CHECKED;
        tbb[3].fsStyle = BTNS_CHECK;

        tbb[4].iBitmap = 1;
        tbb[4].idCommand = IDM_ELLIPSE;
        tbb[4].fsState = TBSTATE_ENABLED;
        tbb[4].fsStyle = BTNS_CHECK;

        tbb[5].iBitmap = 2;
        tbb[5].idCommand = IDM_LINEOO;
        tbb[5].fsState = TBSTATE_ENABLED;
        tbb[5].fsStyle = BTNS_CHECK;

        tbb[6].iBitmap = 3;
        tbb[6].idCommand = IDM_CUBE;
        tbb[6].fsState = TBSTATE_ENABLED;
        tbb[6].fsStyle = BTNS_CHECK;

        SendMessage(hWndToolbar, TB_ADDBUTTONS, 7, (LPARAM)&tbb);
        SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_RECTANGLE:
            editor->SetTool(MyEditor::TOOL_RECT);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_RECTANGLE, TRUE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_ELLIPSE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_LINEOO, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_CUBE, FALSE);
            UpdateWindowTitle(hWnd);
            break;

        case IDM_ELLIPSE:
            editor->SetTool(MyEditor::TOOL_ELLIPSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_RECTANGLE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_ELLIPSE, TRUE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_LINEOO, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_CUBE, FALSE);
            UpdateWindowTitle(hWnd);
            break;

        case IDM_LINEOO:
            editor->SetTool(MyEditor::TOOL_LINEOO);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_RECTANGLE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_ELLIPSE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_LINEOO, TRUE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_CUBE, FALSE);
            UpdateWindowTitle(hWnd);
            break;

        case IDM_CUBE:
            editor->SetTool(MyEditor::TOOL_CUBE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_RECTANGLE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_ELLIPSE, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_LINEOO, FALSE);
            SendMessage(hWndToolbar, TB_CHECKBUTTON, IDM_CUBE, TRUE);
            UpdateWindowTitle(hWnd);
            break;

        case IDM_TABLE:
            editor->ToggleTable(hWnd);
            break;

        case IDM_SAVE:
            editor->SaveToCSV(hWnd);
            break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_LBUTTONDOWN: editor->OnLBdown(hWnd, lParam); break;
    case WM_MOUSEMOVE: editor->OnMouseMove(hWnd, lParam); break;
    case WM_LBUTTONUP: editor->OnLBup(hWnd, lParam); break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        editor->OnPaint(hdc);
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        MyEditor::destroyInstance();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Lab5WindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);

    RegisterClassW(&wc);

    HMENU hMenu = LoadMenuW(hInstance, MAKEINTRESOURCEW(IDR_MYMENU));

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, L"Lab 5 [Current tool: Rectangle]",
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
