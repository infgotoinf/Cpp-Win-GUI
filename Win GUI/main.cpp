#include <windows.h>
#include <random>
#include <string>

HWND button[10][10];

LRESULT CALLBACK ProcessMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
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
				//MessageBoxW(hwnd, L"Hello, World!", L"Test Caption", MB_OK | MB_ICONASTERISK) == IDOK;

				SetWindowTextW(button[buttonId % 10][buttonId / 10], L"Yo");
				EnableWindow(button[buttonId % 10][buttonId / 10], false);
			
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
	std::random_device dev;
	std::mt19937 rnd(dev());
	std::uniform_int_distribution<unsigned> dist(0, 65535);
	dist(rnd);

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
				std::to_wstring(i-i2).c_str(),
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