#include "module2.h"
#include "resource.h"
#include <cwchar>
#include <string>

static int g_scrollValue = 50;

static INT_PTR CALLBACK Dialog2Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND hScroll = GetDlgItem(hDlg, IDC_SCROLLBAR1);
        SetScrollRange(hScroll, SB_CTL, 1, 100, TRUE);
        SetScrollPos(hScroll, SB_CTL, g_scrollValue, TRUE);

        wchar_t buf[64];
        swprintf_s(buf, L"Обране число: %d", g_scrollValue);
        SetDlgItemTextW(hDlg, IDC_STATIC_TEXT, buf);
    }
    return (INT_PTR)TRUE;

    case WM_HSCROLL:
    {
        HWND hScroll = (HWND)lParam;
        int curPos = GetScrollPos(hScroll, SB_CTL);

        switch (LOWORD(wParam))
        {
        case SB_LINELEFT:        curPos -= 1; break;
        case SB_LINERIGHT:       curPos += 1; break;
        case SB_PAGELEFT:        curPos -= 10; break;
        case SB_PAGERIGHT:       curPos += 10; break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:      curPos = HIWORD(wParam); break;
        }

        if (curPos < 1) curPos = 1;
        if (curPos > 100) curPos = 100;

        g_scrollValue = curPos;
        SetScrollPos(hScroll, SB_CTL, curPos, TRUE);

        wchar_t buf[64];
        swprintf_s(buf, L"Обране число: %d", curPos);
        SetDlgItemTextW(hDlg, IDC_STATIC_TEXT, buf);
    }
    return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
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

bool ShowDialog2(HWND hWndParent, int& outValue)
{
    INT_PTR result = DialogBoxW(GetModuleHandle(NULL), MAKEINTRESOURCEW(IDD_DIALOG2), hWndParent, Dialog2Proc);
    if (result == -1)
    {
        DWORD err = GetLastError();
        std::wstring errMsg = L"Помилка створення діалогу 2! Код: " + std::to_wstring(err);
        MessageBoxW(hWndParent, errMsg.c_str(), L"Помилка", MB_ICONERROR);
        return false;
    }
    if (result == IDOK)
    {
        outValue = g_scrollValue;
        return true;
    }
    return false;
}
