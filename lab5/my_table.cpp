#include "my_table.h"
#include <string>

static const wchar_t TABLE_CLASS_NAME[] = L"MyTableWindowClass";

LRESULT CALLBACK TableWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CLOSE:
        ShowWindow(hWnd, SW_HIDE);
        return 0;
    case WM_SIZE: {
        HWND hWndList = GetDlgItem(hWnd, 1001);
        if (hWndList) {
            MoveWindow(hWndList, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        }
    }
                break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

MyTable::MyTable() {
    hWndTable = NULL;
    hWndList = NULL;
}

void MyTable::Show(HWND hWndParent) {
    if (!hWndTable) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = TableWndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = TABLE_CLASS_NAME;
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassW(&wc);

        hWndTable = CreateWindowExW(
            0, TABLE_CLASS_NAME, L"Objects Table",
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 450, 300,
            hWndParent, NULL, GetModuleHandle(NULL), NULL
        );

        RECT rc;
        GetClientRect(hWndTable, &rc);

        hWndList = CreateWindowExW(
            0, WC_LISTVIEWW, L"",
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
            0, 0, rc.right, rc.bottom,
            hWndTable, (HMENU)1001, GetModuleHandle(NULL), NULL
        );

        ListView_SetExtendedListViewStyle(hWndList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

        LVCOLUMNW lvc = {};
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

        lvc.iSubItem = 0; lvc.pszText = (LPWSTR)L"Name"; lvc.cx = 120;
        SendMessage(hWndList, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

        lvc.iSubItem = 1; lvc.pszText = (LPWSTR)L"x1"; lvc.cx = 60;
        SendMessage(hWndList, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc);

        lvc.iSubItem = 2; lvc.pszText = (LPWSTR)L"y1"; lvc.cx = 60;
        SendMessage(hWndList, LVM_INSERTCOLUMN, 2, (LPARAM)&lvc);

        lvc.iSubItem = 3; lvc.pszText = (LPWSTR)L"x2"; lvc.cx = 60;
        SendMessage(hWndList, LVM_INSERTCOLUMN, 3, (LPARAM)&lvc);

        lvc.iSubItem = 4; lvc.pszText = (LPWSTR)L"y2"; lvc.cx = 60;
        SendMessage(hWndList, LVM_INSERTCOLUMN, 4, (LPARAM)&lvc);
    }

    ShowWindow(hWndTable, SW_SHOW);
}

void MyTable::Hide() {
    if (hWndTable) ShowWindow(hWndTable, SW_HIDE);
}

void MyTable::Update(Shape** pcshape, int shapeCount) {
    if (!hWndList) return;

    SendMessage(hWndList, LVM_DELETEALLITEMS, 0, 0);

    for (int i = 0; i < shapeCount; i++) {
        if (!pcshape[i]) continue;

        wchar_t buf1[16], buf2[16], buf3[16], buf4[16];
        swprintf_s(buf1, L"%ld", pcshape[i]->GetX1());
        swprintf_s(buf2, L"%ld", pcshape[i]->GetY1());
        swprintf_s(buf3, L"%ld", pcshape[i]->GetX2());
        swprintf_s(buf4, L"%ld", pcshape[i]->GetY2());

        LVITEMW lvi = {};
        lvi.mask = LVIF_TEXT;
        lvi.iItem = i;
        lvi.iSubItem = 0;
        lvi.pszText = (LPWSTR)L"Object";
        SendMessage(hWndList, LVM_INSERTITEM, 0, (LPARAM)&lvi);

        LVITEMW subItem = {};
        subItem.iItem = i;

        subItem.iSubItem = 1; subItem.pszText = buf1;
        SendMessageW(hWndList, LVM_SETITEMTEXT, i, (LPARAM)&subItem);

        subItem.iSubItem = 2; subItem.pszText = buf2;
        SendMessageW(hWndList, LVM_SETITEMTEXT, i, (LPARAM)&subItem);

        subItem.iSubItem = 3; subItem.pszText = buf3;
        SendMessageW(hWndList, LVM_SETITEMTEXT, i, (LPARAM)&subItem);

        subItem.iSubItem = 4; subItem.pszText = buf4;
        SendMessageW(hWndList, LVM_SETITEMTEXT, i, (LPARAM)&subItem);
    }
}
