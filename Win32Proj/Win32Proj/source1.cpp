#include<Windows.h>
#include "monitor_pixel.h"
using namespace std;

char win_class [] = "color-picker";
HWND win_handle;
bool thread_active = true;

int pixel_color = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HPEN hpen=NULL;
	HBRUSH hbrush;

	switch (message)
	{
	case WM_CREATE:
		hpen = CreatePen(PS_DOT, 1, RGB(200, 200, 200));

	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			hbrush = CreateSolidBrush(pixel_color);
			SelectObject(hdc, hpen);
			SelectObject(hdc, hbrush);
			SetBkColor(hdc, RGB(153, 180, 209));

			char str[50] = { 0 };
			int p = pixel_color;
			sprintf_s(str, "RGB : %d %d %d | %x %x %x    Quit - Shift+Q", GetRValue(p), GetGValue(p), GetBValue(p), GetRValue(p), GetGValue(p), GetBValue(p));
			TextOut(hdc, 5, 5, str, strlen(str));

			Rectangle(hdc, 25, 25, 275, 40);
			DeleteObject(hbrush);
			EndPaint(hWnd, &ps);
			break;
		}

	case WM_DESTROY:
		PostQuitMessage(0);
		DeleteObject(hpen);
		thread_active = false;
		break;
	case 0:
		thread_active = false;
		DeleteObject(hpen);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

bool create_window(HINSTANCE hinstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hinstance;
	wcex.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) (COLOR_SCROLLBAR+3);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = win_class;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, "Can not register class", "Failed", MB_OK);
		return false;
	}

	HWND w = CreateWindow(win_class, "Color Picker", WS_POPUP | WS_VISIBLE | WS_SYSMENU, 10, 10, 300, 50, NULL, NULL, hinstance, NULL);

	if (!w)return false;

	ShowWindow(w, SW_SHOW);
	UpdateWindow(w);

	SetWindowPos(w, HWND_TOPMOST, 10, 10, 300, 50, SWP_SHOWWINDOW);

	win_handle = w;

	return true;
}

DWORD WINAPI thPixelPoll(LPVOID p)
{
	ScreenPixel px;
	POINT pt;
	char str[50];

	int screen_y = GetSystemMetrics(SM_CYSCREEN);
	int curr_y = 10;

	int count = 0;

	while (thread_active)
	{
		if (count >= 4)
		{
			count = 0;
			GetCursorPos(&pt);
			int pxl = px.getFrom(pt);
			pixel_color = pxl;

			InvalidateRect(win_handle, NULL, TRUE);

			if (pt.x < 320 && pt.y < 60 && curr_y == 10)
			{
				curr_y = screen_y - 70;
				SetWindowPos(win_handle, HWND_TOPMOST, 10, curr_y, 300, 50, SWP_SHOWWINDOW);
			}
			else if (pt.x > 320 || pt.y > 60 && curr_y != 10)
			{
				curr_y = 10;
				SetWindowPos(win_handle, HWND_TOPMOST, 10, 10, 300, 50, SWP_SHOWWINDOW);
			}
		}
		int qkey = GetAsyncKeyState(0x51), shiftKey = GetAsyncKeyState(VK_SHIFT);

		if (( (qkey & 0x8000) || (qkey & 1)) && (shiftKey & 0x8000))
		{
			PostMessage(win_handle, WM_QUIT, 0, 0);
			break;
		}

		Sleep(50);
		++count;
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE h1,HINSTANCE h2,char* cmd,int ns)
{
	if (!create_window(h1))return 0;

	DWORD thId;
	HANDLE thHandle=CreateThread(NULL, 0, thPixelPoll, NULL, 0, &thId);

	MSG msg;
	POINT pt;

	ScreenPixel px;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	WaitForSingleObject(thHandle, INFINITE);
	CloseHandle(thHandle);

	return 0;
}