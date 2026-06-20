#include <windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <io.h>
#include <fcntl.h>

using namespace std;

// IDs
#define ID_PHONE     1
#define ID_NAME      2
#define ID_TIME      3
#define ID_ADD       4
#define ID_SHOW      5
#define ID_EDIT      6
#define ID_INDEX     7
#define ID_SEARCH    8
#define ID_DELETE    9
#define ID_CLEAR     10
#define ID_CLEAR_P   11
#define ID_CLEAR_N   12
#define ID_CLEAR_T   13
#define ID_CLEAR_I   14


vector<wstring> records;

HWND hPhone, hName, hTime, hIndex;

bool editMode = false;
int editIndex = -1;

void ParseRecord(const wstring& rec, wstring& phone, wstring& name, wstring& time) {
    size_t p1 = rec.find(L'|');
    size_t p2 = rec.rfind(L'|');

    phone = rec.substr(0, p1 - 1);
    name  = rec.substr(p1 + 2, p2 - p1 - 3);
    time  = rec.substr(p2 + 2);
}

wstring RemoveSpaces(const wstring& s) {
    wstring res;
    for (wchar_t c : s) {
        if (c != L' ' && c != L'\t')
            res.push_back(c);
    }
    return res;
}


void SearchNumbers(
    HWND hwnd,
    HWND hPhone,
    HWND hName,
    HWND hTime,
    HWND hIndex,
    const wstring& nameInput,
    bool hasName,
    bool hasPhone,
    bool hasTime,
    const vector<wstring>& records,
    bool& editMode,
    int& editIndex
) {
        if (hasName && !hasPhone && !hasTime) {

        vector<int> matches;

        for (int i = 0; i < (int)records.size(); i++) {

            wstring p, n, t;
            ParseRecord(records[i], p, n, t);

            bool fullName =
                RemoveSpaces(n) == RemoveSpaces(nameInput);

            wstring surname = n.substr(0, n.find(L' '));

            bool surnameOnly =
                RemoveSpaces(surname) == RemoveSpaces(nameInput);

            if (fullName || surnameOnly) {
                matches.push_back(i);
            }
        }
        
        if (matches.empty()) {
            MessageBoxW(hwnd, L"Абонента не знайдено", L"Пошук", MB_OK);
            return;
        }

        // один номер телефону
        if (matches.size() == 1) {

            int i = matches[0];

            wstring p, n, t;
            ParseRecord(records[i], p, n, t);

            SetWindowTextW(hPhone, p.c_str());
            SetWindowTextW(hName, n.c_str());
            SetWindowTextW(hTime, t.c_str());

            wchar_t buf[10];
            _itow(i + 1, buf, 10);
            SetWindowTextW(hIndex, buf);

            editMode = true;
            editIndex = i;

            return;
        }

        // багато номерів телефону
        wstring msg = L"Було знайдено " + to_wstring(matches.size())
            + L" номерів абонента:\n";

        for (int idx : matches) {

            wstring p, n, t;
            ParseRecord(records[idx], p, n, t);

            msg += p + L"\n";
        }

        msg += L"\nСкорегуйте пошуковий запит";

        MessageBoxW(hwnd, msg.c_str(), L"Пошук", MB_OK);
        return;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    
    // Unicode console
    AllocConsole();

    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"ClientApp";

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowW(
        L"ClientApp",
        L"ClientApp",
        WS_OVERLAPPEDWINDOW,
        200, 200, 470, 350,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

    case WM_CREATE:

        CreateWindowW(L"BUTTON", L"Очистити усі поля",
            WS_VISIBLE | WS_CHILD,
            220, 20, 200, 20, hwnd, (HMENU)ID_CLEAR, NULL, NULL);

        CreateWindowW(L"STATIC", L"Номер телефону:",
            WS_VISIBLE | WS_CHILD,
            10, 60, 200, 20, hwnd, NULL, NULL, NULL);

        hPhone = CreateWindowW(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            220, 60, 200, 20, hwnd, (HMENU)ID_PHONE, NULL, NULL);

        CreateWindowW(L"BUTTON", L"X",
            WS_VISIBLE | WS_CHILD,
            420, 60, 20, 20, hwnd, (HMENU)ID_CLEAR_P, NULL, NULL);

        CreateWindowW(L"STATIC", L"Прізвище та ініціали:",
            WS_VISIBLE | WS_CHILD,
            10, 100, 200, 20, hwnd, NULL, NULL, NULL);

        hName = CreateWindowW(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            220, 100, 200, 20, hwnd, (HMENU)ID_NAME, NULL, NULL);

        CreateWindowW(L"BUTTON", L"X",
            WS_VISIBLE | WS_CHILD,
            420, 100, 20, 20, hwnd, (HMENU)ID_CLEAR_N, NULL, NULL);

        CreateWindowW(L"STATIC", L"Час розмов за місяць (хв.):",
            WS_VISIBLE | WS_CHILD,
            10, 140, 200, 20, hwnd, NULL, NULL, NULL);

        hTime = CreateWindowW(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            220, 140, 200, 20, hwnd, (HMENU)ID_TIME, NULL, NULL);

        CreateWindowW(L"BUTTON", L"X",
            WS_VISIBLE | WS_CHILD,
            420, 140, 20, 20, hwnd, (HMENU)ID_CLEAR_T, NULL, NULL);

        CreateWindowW(L"STATIC", L"№ абонента в списку:",
            WS_VISIBLE | WS_CHILD,
            10, 180, 200, 20, hwnd, NULL, NULL, NULL);

        hIndex = CreateWindowW(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            220, 180, 200, 20, hwnd, (HMENU)ID_INDEX, NULL, NULL);

        CreateWindowW(L"BUTTON", L"X",
            WS_VISIBLE | WS_CHILD,
            420, 180, 20, 20, hwnd, (HMENU)ID_CLEAR_I, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Додати",
            WS_VISIBLE | WS_CHILD,
            10, 220, 70, 30, hwnd, (HMENU)ID_ADD, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Показати оновлений список абонентів",
            WS_VISIBLE | WS_CHILD,
            90, 220, 300, 30, hwnd, (HMENU)ID_SHOW, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Знайти",
            WS_VISIBLE | WS_CHILD,
            10, 260, 70, 30, hwnd, (HMENU)ID_SEARCH, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Зберегти зміни",
            WS_VISIBLE | WS_CHILD,
            90, 260, 120, 30, hwnd, (HMENU)ID_EDIT, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Видалити",
            WS_VISIBLE | WS_CHILD,
            220, 260, 90, 30, hwnd, (HMENU)ID_DELETE, NULL, NULL);

        break;

    case WM_COMMAND:

        // Додавання абонента
        if (LOWORD(wParam) == ID_ADD) {

            wchar_t phone[50], name[100], time[20];

            GetWindowTextW(hPhone, phone, 50);
            GetWindowTextW(hName, name, 100);
            GetWindowTextW(hTime, time, 20);

            wstring phoneInput(phone);

            // Перевірка на дублювання телефону
            for (int i = 0; i < (int)records.size(); i++) {

                wstring p, n, t;
                ParseRecord(records[i], p, n, t);

                if (p == phoneInput) {

                    wstring msg =
                        L"Цей номер телефону вже зареєстрований на іншу особу!\nПеревірте правильність введених даних";

                    MessageBoxW(hwnd, msg.c_str(), L"Помилка", MB_ICONERROR);

                    return 0; // блокуємо додавання
                }
            }

            // додавання, якщо все ок
            wstring rec = wstring(phone) + L" | " + name + L" | " + time;
            records.push_back(rec);

            MessageBoxW(hwnd, L"Додано!", L"OK", MB_OK);
        }

        // Демонстрація списку абонентів
        if (LOWORD(wParam) == ID_SHOW) {

            system("chcp 65001");
            system("cls");
            
            wcout << L"\n===== СПИСОК АБОНЕНТІВ =====\n";

            for (int i = 0; i < records.size(); i++) {
                wcout << i + 1 << L") " << records[i] << endl;
            }
        }

        // Редагування даних абонента
        if (LOWORD(wParam) == ID_EDIT && editMode) {

            wchar_t phone[50], name[100], time[20];

            GetWindowTextW(hPhone, phone, 50);
            GetWindowTextW(hName, name, 100);
            GetWindowTextW(hTime, time, 20);

            records[editIndex] =
                wstring(phone) + L" | " + name + L" | " + time;

            SetWindowTextW(hPhone, L"");
            SetWindowTextW(hName, L"");
            SetWindowTextW(hTime, L"");
            SetWindowTextW(hIndex, L"");

            editMode = false;
            editIndex = -1;

            MessageBoxW(hwnd, L"Дані абонента оновлено", L"OK", MB_OK);
        }

        // Пошук абонента за одним з критерієв
        if (LOWORD(wParam) == ID_SEARCH) {

            wchar_t phone[50], name[100], time[20];

            GetWindowTextW(hPhone, phone, 50);
            GetWindowTextW(hName, name, 100);
            GetWindowTextW(hTime, time, 20);

            wstring phoneInput(phone);
            wstring nameInput(name);
            wstring timeInput(time);

            bool hasPhone = !phoneInput.empty();
            bool hasTime  = !timeInput.empty();
            bool hasName  = !nameInput.empty();

            vector<int> phoneMatches;
            vector<int> surnameMatches;

            // =============================
            // 1. Пошук за індексом
            // =============================
            wchar_t idxBuf[10];
            GetWindowTextW(hIndex, idxBuf, 10);

            if (wcslen(idxBuf) > 0) {

                int idx = _wtoi(idxBuf) - 1;

                if (idx >= 0 && idx < records.size()) {

                    wstring p, n, t;
                    ParseRecord(records[idx], p, n, t);

                    SetWindowTextW(hPhone, p.c_str());
                    SetWindowTextW(hName, n.c_str());
                    SetWindowTextW(hTime, t.c_str());

                    editMode = true;
                    editIndex = idx;

                    return 0;
                }
            }                
            
            // ================================
            // 2. Пошук за номером телефону
            // ================================
            if (hasPhone && !hasName && !hasTime) {

                for (int i = 0; i < records.size(); i++) {

                    wstring p, n, t;
                    ParseRecord(records[i], p, n, t);

                    if (p == phoneInput) {

                        SetWindowTextW(hPhone, p.c_str());
                        SetWindowTextW(hName, n.c_str());
                        SetWindowTextW(hTime, t.c_str());

                        wchar_t buf[10];
                        _itow(i + 1, buf, 10);
                        SetWindowTextW(hIndex, buf);

                        editMode = true;
                        editIndex = i;

                        return 0;
                    }
                }

                MessageBoxW(hwnd,
                    L"Номер телефону не знайдено",
                    L"Пошук",
                    MB_OK);

                return 0;
            }            

            // ================================
            // 3. Пошук лише ТІЛЬКИ за прізвищем
            // ================================
            bool onlySurname = hasName && !hasPhone && !hasTime;

            for (int i = 0; i < records.size(); i++) {

                wstring p, n, t;
                ParseRecord(records[i], p, n, t);

                wstring surname = n.substr(0, n.find(L' '));
                if (RemoveSpaces(surname) == RemoveSpaces(nameInput)) {
                    surnameMatches.push_back(i);
                }
            }

            if (onlySurname) {
                
                if (surnameMatches.size() > 1) { // якщо знайдено однофамільців

                    wstring msg = L"Було знайдено " + to_wstring(surnameMatches.size());
                    
                    if (surnameMatches.size() < 5) {
                        msg += L" абоненти:\n\n";
                    }
                    else {
                        msg += L" абонентів:\n\n";
                    }

                    for (int idx : surnameMatches) {
                        wstring p, n, t;
                        ParseRecord(records[idx], p, n, t);
                        msg += n + L" | " + p + L"\n";
                    }

                    msg += L"\nСкорегуйте пошуковий запит";

                    MessageBoxW(hwnd, msg.c_str(), L"Пошук", MB_OK);
                    return 0;
                }

                if (surnameMatches.size() == 1) { // якщо однофамільців не знайдено
                    SearchNumbers(
                        hwnd,
                        hPhone,
                        hName,
                        hTime,
                        hIndex,
                        nameInput,
                        hasName,
                        hasPhone,
                        hasTime,
                        records,
                        editMode,
                        editIndex
                    );
                    return 0;
                }
            }

            // ================================
            // інші випадки
            // ================================
            for (int i = 0; i < records.size(); i++) {

                wstring p, n, t;
                ParseRecord(records[i], p, n, t);

                bool match =
                    (!phoneInput.empty() && p == phoneInput) ||
                    (!timeInput.empty() && t == timeInput) ||
                    (!nameInput.empty() && RemoveSpaces(n) == RemoveSpaces(nameInput));

                if (match) {
                    SearchNumbers(
                        hwnd,
                        hPhone,
                        hName,
                        hTime,
                        hIndex,
                        nameInput,
                        hasName,
                        hasPhone,
                        hasTime,
                        records,
                        editMode,
                        editIndex
                    );
                    return 0;
                }
            }

            MessageBoxW(hwnd,
                L"Абонента не знайдено",
                L"Пошук",
                MB_OK);

        }

        // Видалення абонента
        if (LOWORD(wParam) == ID_DELETE) {

            wchar_t buf[10];
            GetWindowTextW(hIndex, buf, 10);

            int idx = _wtoi(buf) - 1;

            if (idx < 0 || idx >= records.size()) {
                MessageBoxW(hwnd, L"Невірний номер", L"Помилка", MB_OK);
                break;
            }

            records.erase(records.begin() + idx);

            SetWindowTextW(hIndex, L"");

            MessageBoxW(hwnd, L"Видалено", L"OK", MB_OK);
        }

        // Очищення полів

        if (LOWORD(wParam) == ID_CLEAR) {
            SetWindowTextW(hPhone, L"");
            SetWindowTextW(hName, L"");
            SetWindowTextW(hTime, L"");
            SetWindowTextW(hIndex, L"");

            editMode = false;
            editIndex = -1;
        }

        if (LOWORD(wParam) == ID_CLEAR_P) {
            SetWindowTextW(hPhone, L"");
        }

        if (LOWORD(wParam) == ID_CLEAR_N) {
            SetWindowTextW(hName, L"");
        }

        if (LOWORD(wParam) == ID_CLEAR_T) {
            SetWindowTextW(hTime, L"");
        }

        if (LOWORD(wParam) == ID_CLEAR_I) {
            SetWindowTextW(hIndex, L"");
        }

        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}