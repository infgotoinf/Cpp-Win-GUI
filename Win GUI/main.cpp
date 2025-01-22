#include <windows.h>
#include <random>
#include <string>

HWND button[10][10];

// Функция генерации минного поля
void generate_bombs(int** field)
{
	do {
		std::random_device dev;
		std::mt19937 rnd(dev());
		std::uniform_int_distribution<int> dist(0, 9);
		int random = dist(rnd);
		int random2 = dist(rnd);
		if (field[random][random2] == 0)
		{
			field[random][random2] = -1;
			break;
		}
	} while (true);
}

// Функция изменения 0 на другие цифры
void change_nums(int** field)
{
	for (int i = 0; i < 10; i++) {
		for (int i2 = 0; i2 < 10; i2++) {
			if (field[i][i2] == -1) continue;
			int counter = 0;
			for (int j = -1; j <= 1; j++) {
				for (int j2 = -1; j2 <= 1; j2++) {
					if ((j == 0 && j2 == 0) || i + j < 0 || i + j > 9 || i2 + j2 < 0 || i2 + j2 > 9) continue;
					if (field[i + j][i2 + j2] == -1) counter++;
				}
			}
			field[i][i2] = counter;
		}
	}
}


LRESULT CALLBACK ProcessMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	int** field = new int* [10];
	for (int i = 0; i < 10; i++)
	{
		field[i] = new int[10] { 0 };
	}
	for (int i = 0; i < 25; i++) generate_bombs(field);
	change_nums(field);

	bool game_started = false;

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT pn{};
			HDC hdc = BeginPaint(hwnd, &pn);

			FillRect(hdc, &pn.rcPaint, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 9));

			EndPaint(hwnd, &pn);
		}
		break;
	case WM_COMMAND:
		{
			WORD buttonId = LOWORD(wp), msgCode = HIWORD(wp);
			if (msgCode == BN_CLICKED)
			{
				
				SetWindowTextW(button[buttonId % 10][buttonId / 10], std::to_wstring(field[buttonId % 10][buttonId / 10]).c_str());
				EnableWindow(button[buttonId % 10][buttonId / 10], false);
			
			}
		}
		for (int i = 0; i < 10; i++) {
			delete[] field[i];
			field[i] = nullptr;
		}
		delete[] field;
		field = nullptr;
		break;
	default:
		return DefWindowProcW(hwnd, msg, wp, lp);
	}

	return 0;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	WCHAR mainWindow[] = L"Main Window";

	WNDCLASS wnd{};
	wnd.hInstance = hInstance;
	wnd.lpszClassName = mainWindow;
	wnd.lpfnWndProc = ProcessMessage;

	RegisterClassW(&wnd);


	HWND hwnd = CreateWindowExW(
		0,
		mainWindow,
		L"Saper",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 515, 538,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);


	for (int i = 0; i < 10; i++) {
		for (int i2 = 0; i2 < 10; i2++) {
			button[i][i2] = CreateWindowExW(
				0,
				L"BUTTON",
				L"",
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				50*i, 50*i2, 50, 50,
				hwnd,
				reinterpret_cast<HMENU>(i + i2*10),
				reinterpret_cast<HINSTANCE>(GetClassLongPtr(hwnd, GWLP_HINSTANCE)),
				nullptr
			);
		}
	}


	if (hwnd == nullptr) return 0;

	ShowWindow(hwnd, nCmdShow);

	MSG msg{};
	while (GetMessageW(&msg, hwnd, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return 0;
}