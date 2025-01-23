#include <windows.h>
#include <random>
#include <string>

HWND button[10][10];
bool game_started = false;

int** field = new int* [10];

// Функция генерации минного поля
void generate_bombs(int** field, int buttonId)
{
	do {
		std::random_device dev;
		std::mt19937 rnd(dev());
		std::uniform_int_distribution<int> dist(0, 9);
		int random = dist(rnd);
		int random2 = dist(rnd);

		/*bool do_continue = false;
		for (int j = -1; j <= 1; j++)
		{
			for (int j2 = -1; j2 <= 1; j2++)
			{
				do_continue = ((buttonId / 10 + j == random && buttonId % 10 + j2 == random2) ? false : true);
				if (do_continue) break;
			}
			if (do_continue) break;
		}
		if (do_continue) continue;*/

		if (buttonId / 10 == random && buttonId % 10 == random2) continue;

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
	for (int i = 0; i < 10; i++)
	{
		for (int i2 = 0; i2 < 10; i2++)
		{
			int counter = 0;
			if (field[i][i2] == -1) continue;
			else for (int j = -1; j <= 1; j++) // Легендарный элс фор
			{
				for (int j2 = -1; j2 <= 1; j2++)
				{
					if ((j == 0 && j2 == 0) || i + j < 0 || i + j > 9 || i2 + j2 < 0 || i2 + j2 > 9) continue;
					else if (field[i + j][i2 + j2] == -1) counter++;
				}
			}
			field[i][i2] = counter;
		}
	}
}

// Функция открытия пустых полей
void field_opener(int** field, int buttonId)
{
	int i = buttonId / 10, i2 = buttonId % 10;
	if (field[buttonId / 10][buttonId % 10] == 0) {
		for (int j = -1; j <= 1; j++)
		{
			for (int j2 = -1; j2 <= 1; j2++)
			{
				if ((j == 0 && j2 == 0) || i + j < 0 || i + j > 9 || i2 + j2 < 0 || i2 + j2 > 9) continue;
				SetWindowTextW(button[i + j][i2 + j2], std::to_wstring(field[i + j][i2 + j2]).c_str());

				if (EnableWindow(button[i + j][i2 + j2], false) == true) continue;
				field_opener(field, (i + j) * 10 + i2 + j2);
			}
		}
	}
}

LRESULT CALLBACK ProcessMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	/*while (1) {
		if (did) break;

		for (int i = 0; i < 10; i++)
		{
			field[i] = new int[10] { 0 };
		}
		for (int i = 0; i < 25; i++) generate_bombs(field);
		change_nums(field);

		did = true;
	}*/

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
				if (!game_started) {
					for (int i = 0; i < 25; i++) generate_bombs(field, buttonId);
					change_nums(field);
					game_started = true;
				}

				SetWindowTextW(button[buttonId / 10][buttonId % 10], std::to_wstring(field[buttonId / 10][buttonId % 10]).c_str());
				EnableWindow(button[buttonId / 10][buttonId % 10], false);
				field_opener(field, buttonId);
				if (field[buttonId / 10][buttonId % 10] == -1) {
					MessageBoxW(NULL, L"YOU LOST!", L"End Screen", NULL);
				}
			}
		}
		break;
	default:
		return DefWindowProcW(hwnd, msg, wp, lp);
	}

	return 0;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	for (int i = 0; i < 10; i++)
	{
		field[i] = new int[10] { 0 };
	}
	
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
				50*i2, 50*i, 50, 50,
				hwnd,
				reinterpret_cast<HMENU>(i2 + i*10),
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
	for (int i = 0; i < 10; i++) {
		delete[] field[i];
		field[i] = nullptr;
	}
	delete[] field;
	field = nullptr;
	return 0;
}