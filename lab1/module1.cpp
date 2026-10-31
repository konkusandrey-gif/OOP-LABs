#include "module1.h"
#include "resource.h"

static wchar_t g_inputText[256] = L"";

static INT_PTR CALLBACK Dialog1Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemTextW(hDlg, IDC_EDIT1, g_inputText);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            GetDlgItemTextW(hDlg, IDC_EDIT1, g_inputText, 256);
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

bool ShowDialog1(HWND hWndParent, std::wstring& outText)
{
    INT_PTR result = DialogBoxW(GetModuleHandle(NULL), MAKEINTRESOURCEW(IDD_DIALOG1), hWndParent, Dialog1Proc);
    if (result == -1)
    {
        DWORD err = GetLastError();
        std::wstring errMsg = L"Помилка створення діалогу 1! Код: " + std::to_wstring(err);
        MessageBoxW(hWndParent, errMsg.c_str(), L"Помилка", MB_ICONERROR);
        return false;
    }
    if (result == IDOK)
    {
        outText = g_inputText;
        return true;
    }
    return false;
}
