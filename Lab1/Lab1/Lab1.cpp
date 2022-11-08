#pragma comment(lib, "Msimg32.lib")

#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <windowsx.h>
#define MAX_ACCEL 100

void boost(int* accel);
void slowDown(int* accel);

LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HINSTANCE hInst;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX windClassEx; HWND hWnd; MSG msg; BOOL isMessageRetrieved;
	windClassEx.cbSize = sizeof(WNDCLASSEX);
	windClassEx.style = CS_DBLCLKS;
	windClassEx.lpfnWndProc = windowProc;
	windClassEx.cbClsExtra = 0;
	windClassEx.cbWndExtra = 0;
	windClassEx.hInstance = hInstance;
	windClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	windClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windClassEx.lpszMenuName = NULL;
	windClassEx.lpszClassName = L"MainWindowClass";
	windClassEx.hIconSm = windClassEx.hIcon;

	if (!RegisterClassEx(&windClassEx))
	{
		MessageBox(NULL,
			_T("RegisterClassEx failed. Terminating..."),
			_T("Error"),
			MB_ICONERROR);

		return 1;
	}

	hInst = hInstance;
	hWnd = CreateWindow(L"MainWindowClass", L"Sprite",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		900, 700, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("CreateWindow failed. Terminating..."),
			_T("Error"),
			MB_ICONERROR);

		return 1;
	}

	ShowWindow(hWnd, SW_NORMAL);
	UpdateWindow(hWnd);

	while ((isMessageRetrieved = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (isMessageRetrieved == -1)
		{
			MessageBox(NULL,	
				_T("Failed to read the key"),
				_T("Error"),
				MB_ICONERROR);
			continue;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static BOOL isActive = false;
	static HBITMAP bmpPicHandle = NULL, bmpMaskHandle, hTempBitmap;
	static bool left = false, right = false, up = false, down = false;
	static int lAccel, rAccel, uAccel, dAccel;
	static int x = 200, y = 200, wndWidth, wndHeight, spriteWidth = 50, spriteHeight = 50;
	static const int pictureSize = 532;
	switch (msg)
	{
	case WM_CREATE:
	{
		bmpPicHandle = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\nikbo\\Semestr 5\\OSaSP\\osasp-win\\Lab1\\Lab1\\img\\windowsxp_authmask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (bmpPicHandle == NULL)
			MessageBox(NULL,
				_T("Coul't find bitmap image file!"),
				_T("Warning"),
				MB_ICONWARNING);
		bmpMaskHandle = (HBITMAP)LoadImageW(NULL, L"C:\\Users\\nikbo\\Semestr 5\\OSaSP\\osasp-win\\Lab1\\Lab1\\img\\windowsxp_auth.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (bmpMaskHandle == NULL)
			MessageBox(NULL,
				_T("Couldn't find bitmap mask file!"),
				_T("Warning"),
				MB_ICONWARNING);
		SetTimer(hWnd, 1, 10, NULL);
		break;
	}
	case WM_COMMAND:
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC windDispCont, spriteStoreDC;

		windDispCont = BeginPaint(hWnd, &ps);
		spriteStoreDC = CreateCompatibleDC(windDispCont);
		hTempBitmap = CreateCompatibleBitmap(windDispCont, wndWidth, wndHeight);
		SaveDC(spriteStoreDC);
		SelectObject(spriteStoreDC, hTempBitmap);

		//background
		SaveDC(spriteStoreDC);
		HBRUSH hBrush = CreateSolidBrush(RGB(100, 0, 85));
		RECT rect = { 0,0,wndWidth,wndHeight };
		FillRect(spriteStoreDC, &rect, hBrush);
		DeleteObject(hBrush);
		RestoreDC(spriteStoreDC, -1);

		//mask
		HDC hdcSprite = CreateCompatibleDC(spriteStoreDC);
		SelectObject(hdcSprite, bmpMaskHandle);
		StretchBlt(spriteStoreDC, x, y, spriteWidth, spriteHeight, hdcSprite, 0, 0, pictureSize, pictureSize, SRCAND);

		//picture
		SelectObject(hdcSprite, bmpPicHandle);
		StretchBlt(spriteStoreDC, x, y, spriteWidth, spriteHeight, hdcSprite, 0, 0, pictureSize, pictureSize, SRCPAINT);
		DeleteObject(hdcSprite);

		BitBlt(windDispCont, 0, 0, wndWidth, wndHeight, spriteStoreDC, 0, 0, SRCCOPY);
		
		//garbage collection
		if (spriteStoreDC) {
			RestoreDC(spriteStoreDC, -1);
			DeleteObject(spriteStoreDC);
			DeleteObject(hTempBitmap);
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam) {
		case VK_LEFT:
			left = true;
			break;
		case VK_RIGHT:
			right = true;
			break;
		case VK_UP:
			up = true;
			break;
		case VK_DOWN:
			down = true;
			break;
		}
		break;
	}
	case WM_KEYUP:
	{
		switch (wParam) {
		case VK_LEFT:
			left = false;
			break;
		case VK_RIGHT:
			right = false;
			break;
		case VK_UP:
			up = false;
			break;
		case VK_DOWN:
			down = false;
			break;
		}
		break;
	}
	case WM_SIZE:
	{
		wndWidth = LOWORD(lParam);
		wndHeight = HIWORD(lParam);
		break;
	}
	case WM_DESTROY: 
	{
		DeleteObject(bmpPicHandle);
		PostQuitMessage(0);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		int wheelData = GET_WHEEL_DELTA_WPARAM(wParam);
		if (LOWORD(wParam) & MK_SHIFT) {
			if (wheelData > 0)
				x += 10;
				//right = true;
			else
				x -= 10;
				//left = true;
		}
		else {
			if (wheelData > 0)
				//up = true;
			y += 10;
			else
				//down = true;
				y -= 10;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		//break;
	}
	case WM_TIMER:
	{
		if (wParam == 1)
		{
			if (left == true)
				boost(&lAccel);
			else
				slowDown(&lAccel);

			if (right == true)
				boost(&rAccel);
			else
				slowDown(&rAccel);

			if (up == true)
				boost(&uAccel);
			else
				slowDown(&uAccel);
			
			if (down == true)
				boost(&dAccel);
			else
				slowDown(&dAccel);

			x += (rAccel - lAccel);
			y += (dAccel - uAccel);

			if (x < 0) {
				rAccel += lAccel;
				lAccel = 0;
			}
			else if (x > wndWidth - spriteWidth) {
				lAccel += rAccel;
				rAccel = 0;
			}

			if (y < 0) {
				dAccel += uAccel;
				uAccel = 0;
			}
			else if (y > wndHeight - spriteHeight) {
				uAccel += dAccel;
				dAccel = 0;
			}

			x = max(0, min(x, wndWidth - spriteWidth));
			y = max(0, min(y, wndHeight - spriteHeight));

			InvalidateRect(hWnd, NULL, FALSE);

		}
		break;
	}
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void boost(int* spriteAcceleration)
{
	*spriteAcceleration = min(MAX_ACCEL, *spriteAcceleration + 1);
}

void slowDown(int* spriteAcceleration)
{
	int val = abs(*spriteAcceleration);
	int sign = *spriteAcceleration > 0 ? 1 : -1;

	val = max(0, val - 1);

	*spriteAcceleration = val * sign;
}
