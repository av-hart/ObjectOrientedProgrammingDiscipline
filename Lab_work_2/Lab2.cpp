#define UNICODE
#define _UNICODE

#include "Lib.h"
#include <fstream>

vector<shared_ptr<GraphicObject>> objects;
shared_ptr<GraphicObject> selected = nullptr;

ULONG_PTR token;

// ================= ІНІЦІАЛІЗАЦІЯ GDI =================
void InitGDI() {
    GdiplusStartupInput input;
    GdiplusStartup(&token, &input, nullptr);
}

void ShutdownGDI() {
    GdiplusShutdown(token);
}

// ================= ЗАВАНТАЖЕННЯ КОНФІГУРАЦІЇ =================
void InitObjects() {

    objects.push_back(make_shared<TriangleObj>(
        100, 100, 60, 4, 2
    ));

    objects.push_back(make_shared<TriangleObj>(
        300, 200, 80, -1, 3
    ));

    objects.push_back(make_shared<CircleObj>(
        100, 100, 40, 5, 4
    ));

    objects.push_back(make_shared<RectangleObj>(
        300, 200, 120, 80, -1, 2
    ));

    objects.push_back(make_shared<CircleObj>(
        500, 300, 30, 3, -2
    ));

    objects.push_back(make_shared<RectangleObj>(
        200, 400, 150, 60, 2, -1
    ));
}

// ================= ОБРОБНИК ВІКНА =================
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

    case WM_CREATE:
        SetTimer(hwnd, 1, 30, NULL);
        InitObjects();
        srand((unsigned)time(NULL));
        return 0;

    case WM_TIMER: {

        RECT rc;
        GetClientRect(hwnd, &rc);

        int W = rc.right;
        int H = rc.bottom;

        for (auto& o : objects)
            o->update(W, H);

        for (size_t i = 0; i < objects.size(); i++) {
            for (size_t j = i + 1; j < objects.size(); j++) {
                if (objects[i]->collide(objects[j].get())) {
                    objects[i]->onCollision(objects[j].get());
                    objects[j]->onCollision(objects[i].get());
                }
            }
        }

        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        Graphics g(hdc);
        g.Clear(Color(255, 25, 25, 25));

        for (auto& o : objects)
            o->draw(g);

        EndPaint(hwnd, &ps);
        return 0;
    }

    // ================= ЛКМ — створення випадкового об'єкта =================
    case WM_LBUTTONDOWN: {

        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        int type = rand() % 3;

        float vx = (rand() % 7 - 3);
        float vy = (rand() % 7 - 3);
        float size = 20 + rand() % 50;

        if (type == 0) {
            objects.push_back(make_shared<CircleObj>(x, y, size, vx, vy));
        }
        else if (type == 1) {
            objects.push_back(make_shared<RectangleObj>(x, y, size, size, vx, vy));
        }
        else {
            objects.push_back(make_shared<TriangleObj>(x, y, size, vx, vy));
        }

        return 0;
    }

    // ================= ПКМ — видалення об'єкта =================
    case WM_RBUTTONDOWN: {

        int mx = LOWORD(lParam);
        int my = HIWORD(lParam);

        selected = nullptr;

        for (auto it = objects.begin(); it != objects.end(); ++it) {

            float x = (*it)->x;
            float y = (*it)->y;
            float w = (*it)->w;
            float h = (*it)->h;

            if (mx >= x && mx <= x + w &&
                my >= y && my <= y + h) {

                selected = *it;
                objects.erase(it);
                break;
            }
        }

        return 0;
    }

    // ================= 1 — коло =================
    case WM_KEYDOWN:

        if (wParam == '1') {

            int x = rand() % 700;
            int y = rand() % 500;

            float vx = (rand() % 7 - 3);
            float vy = (rand() % 7 - 3);
            float r = 20 + rand() % 40;

            objects.push_back(make_shared<CircleObj>(x, y, r, vx, vy));
        }

        // ================= 2 — трикутник =================
        else if (wParam == '2') {

            int x = rand() % 700;
            int y = rand() % 500;

            float vx = (rand() % 7 - 3);
            float vy = (rand() % 7 - 3);

            float size = 30 + rand() % 50;

            objects.push_back(make_shared<TriangleObj>(x, y, size, vx, vy));
        }

        // ================= 3 — прямокутник =================
        else if (wParam == '3') {

            int x = rand() % 700;
            int y = rand() % 500;

            float vx = (rand() % 7 - 3);
            float vy = (rand() % 7 - 3);

            float w = 30 + rand() % 60;
            float h = 30 + rand() % 60;

            objects.push_back(make_shared<RectangleObj>(x, y, w, h, vx, vy));
        }

        // ================= F1 ДОПОМОГА =================
        else if (wParam == VK_F1) {

            MessageBoxW(hwnd,
                L"Керування:\n\n"
                L"ЛКМ - створити випадковий об'єкт\n"
                L"1 - коло\n"
                L"2 - трикутник\n"
                L"3 - прямокутник\n"
                L"ПКМ - видалити об'єкт\n",
                L"Довідка",
                MB_OK | MB_ICONINFORMATION
            );
        }

        return 0;

    // ================= КОЛЕСО МИШІ =================
    case WM_MOUSEWHEEL: {

        if (!selected) return 0;

        int delta = GET_WHEEL_DELTA_WPARAM(wParam);

        selected->vx += (delta > 0 ? 1 : -1);
        selected->vy += (delta > 0 ? 1 : -1);

        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ================= ГОЛОВНА ФУНКЦІЯ =================
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {

    InitGDI();

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"LabWindow";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        L"LabWindow",
        L"GDI+ Lab OOP",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        900, 600,
        nullptr, nullptr, hInstance, nullptr
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ShutdownGDI();
    return 0;
}