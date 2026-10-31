#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

struct Point {
    int id;   // Номер точки
    int x, y; // Координаты
};

std::vector<Point> g_points;
bool g_hasData = false;

void ReadFromClipboard(HWND hWnd) {
    if (!OpenClipboard(hWnd)) return;

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData) {
        char* pText = (char*)GlobalLock(hData);
        if (pText) {
            std::string text(pText);
            GlobalUnlock(hData);

            std::stringstream ss(text);
            g_points.clear();
            Point p;
            int counter = 1;
            while (ss >> p.x >> p.y) {
                p.id = counter++;
                g_points.push_back(p);
            }

            // Сортировка по возрастанию X
            std::sort(g_points.begin(), g_points.end(), [](const Point& a, const Point& b) {
                return a.x < b.x;
                });

            g_hasData = !g_points.empty();
        }
    }
    CloseClipboard();
    InvalidateRect(hWnd, NULL, TRUE);
}

void DrawGraph(HDC hdc, RECT rect) {
    // 1. Фон "Тетрадь в клетку"
    HBRUSH hGridBg = CreateSolidBrush(RGB(250, 252, 255));
    FillRect(hdc, &rect, hGridBg);
    DeleteObject(hGridBg);

    HPEN hGridPen = CreatePen(PS_SOLID, 1, RGB(225, 235, 245));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hGridPen);

    int step = 25;
    for (int x = rect.left; x < rect.right; x += step) {
        MoveToEx(hdc, x, rect.top, NULL);
        LineTo(hdc, x, rect.bottom);
    }
    for (int y = rect.top; y < rect.bottom; y += step) {
        MoveToEx(hdc, rect.left, y, NULL);
        LineTo(hdc, rect.right, y);
    }
    DeleteObject(hGridPen);

    SetBkMode(hdc, TRANSPARENT);

    if (!g_hasData || g_points.empty()) {
        const wchar_t* msg = L"Ожидание данных из буфера обмена...";
        SetTextColor(hdc, RGB(80, 80, 80));
        DrawTextW(hdc, msg, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, hOldPen);
        return;
    }

    int margin = 70; // Увеличенный отступ под длинные числа
    int left = rect.left + margin;
    int top = rect.top + margin;
    int right = rect.right - margin;
    int bottom = rect.bottom - margin;

    // 2. ДИНАМИЧЕСКИЙ РАСЧЕТ ГРАНИЦ под зашедшие точки
    int minX = g_points[0].x, maxX = g_points[0].x;
    int minY = g_points[0].y, maxY = g_points[0].y;

    for (const auto& p : g_points) {
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
    }

    // Запас по краям 10%, чтобы точки не липли к границам
    int spanX = (maxX - minX == 0) ? 20 : (maxX - minX);
    int spanY = (maxY - minY == 0) ? 20 : (maxY - minY);

    int padX = spanX / 10 + 1;
    int padY = spanY / 10 + 1;

    int boundMinX = minX - padX;
    int boundMaxX = maxX + padX;
    int boundMinY = minY - padY;
    int boundMaxY = maxY + padY;

    // Маппинг координат
    auto ToScreenX = [&](int x) {
        return left + (int)((double)(x - boundMinX) / (boundMaxX - boundMinX) * (right - left));
        };
    auto ToScreenY = [&](int y) {
        return bottom - (int)((double)(y - boundMinY) / (boundMaxY - boundMinY) * (bottom - top));
        };

    // Рассчитываем где проходить осям X и Y
    int originX = ToScreenX(0);
    int originY = ToScreenY(0);

    // Ограничение осей, если ноль за пределами экрана
    if (originX < left) originX = left;
    if (originX > right) originX = right;
    if (originY < top) originY = top;
    if (originY > bottom) originY = bottom;

    // 3. Рисуем оси
    HPEN hAxisPen = CreatePen(PS_SOLID, 2, RGB(50, 50, 50));
    SelectObject(hdc, hAxisPen);

    // Ось Y
    MoveToEx(hdc, originX, bottom, NULL);
    LineTo(hdc, originX, top - 15);
    LineTo(hdc, originX - 4, top - 5);
    MoveToEx(hdc, originX, top - 15, NULL);
    LineTo(hdc, originX + 4, top - 5);

    // Ось X
    MoveToEx(hdc, left, originY, NULL);
    LineTo(hdc, right + 15, originY);
    LineTo(hdc, right + 5, originY - 4);
    MoveToEx(hdc, right + 15, originY, NULL);
    LineTo(hdc, right + 5, originY + 4);

    SetTextColor(hdc, RGB(0, 0, 0));

    // Названия осей
    TextOutW(hdc, originX + 10, top - 20, L"Y", 1);
    TextOutW(hdc, right + 20, originY - 15, L"X", 1);

    // Реальные граничные значения диапазона на осях
    std::wstring strMinX = std::to_wstring(minX), strMaxX = std::to_wstring(maxX);
    std::wstring strMinY = std::to_wstring(minY), strMaxY = std::to_wstring(maxY);

    TextOutW(hdc, ToScreenX(minX) - 10, originY + 5, strMinX.c_str(), strMinX.length());
    TextOutW(hdc, ToScreenX(maxX) - 15, originY + 5, strMaxX.c_str(), strMaxX.length());
    TextOutW(hdc, originX + 5, ToScreenY(minY) - 5, strMinY.c_str(), strMinY.length());
    TextOutW(hdc, originX + 5, ToScreenY(maxY) - 10, strMaxY.c_str(), strMaxY.length());

    // 4. Линия графика
    HPEN hGraphPen = CreatePen(PS_SOLID, 2, RGB(220, 20, 60));
    SelectObject(hdc, hGraphPen);

    int startX = ToScreenX(g_points[0].x);
    int startY = ToScreenY(g_points[0].y);
    MoveToEx(hdc, startX, startY, NULL);

    HBRUSH hDotBrush = CreateSolidBrush(RGB(0, 102, 204));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hDotBrush);

    for (size_t i = 0; i < g_points.size(); ++i) {
        int sx = ToScreenX(g_points[i].x);
        int sy = ToScreenY(g_points[i].y);

        LineTo(hdc, sx, sy);
        Ellipse(hdc, sx - 4, sy - 4, sx + 4, sy + 4);

        std::wstring pLabel = L"T" + std::to_wstring(g_points[i].id) + L": (" +
            std::to_wstring(g_points[i].x) + L"," +
            std::to_wstring(g_points[i].y) + L")";

        TextOutW(hdc, sx + 5, sy - 15, pLabel.c_str(), pLabel.length());
    }

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hAxisPen);
    DeleteObject(hGraphPen);
    DeleteObject(hDotBrush);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_USER + 1:
        ReadFromClipboard(hWnd);
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rect;
        GetClientRect(hWnd, &rect);

        DrawGraph(hdc, rect);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_SIZE:
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"Object3WindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, L"Object3",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 650, 500,
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
