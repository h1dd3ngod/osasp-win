// Lab2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Lab2.h"
/*#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <commctrl.h>
#include <tchar.h>*/


#define MAX_LOADSTRING 100
#define COUNT_OF_COLUMNS 7
#define COUNT_OF_ROWS 5


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND textTable[COUNT_OF_ROWS][COUNT_OF_COLUMNS]; // table of EditBoxes
const int defaultHeight = 15;
int positionOfScroll = 0;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void createTable(HWND, int, int);
void updateTable(HWND, int, int);
void destroyTable(HWND);
void addEditBox(HWND, int, int, int, int);
int getLastLine(HWND);
int getRowsHeight(int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VSCROLL,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   SCROLLINFO scrollInfo;
   scrollInfo.nPos = 0;
   scrollInfo.nMin = 0;
   scrollInfo.nMax = COUNT_OF_ROWS - 1;
   scrollInfo.nPage = 1;
   scrollInfo.fMask = SIF_ALL;

   SetScrollInfo(hWnd, SB_VERT, &scrollInfo, TRUE);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    static int winWidth, winHeight;
    static char sym;
    switch (message)
    {
    case WM_CREATE:
        GetClientRect(hWnd, &rect);
        winWidth = rect.right - rect.left;
        winHeight = rect.bottom - rect.top;
        createTable(hWnd, winWidth, winHeight);
        break;
    case WM_SIZE:
        GetClientRect(hWnd, &rect);
        winWidth = rect.right - rect.left;
        winHeight = rect.bottom - rect.top;
        updateTable(hWnd, winWidth, winHeight);
        InvalidateRect(hWnd, NULL, 0);
        break;
    case WM_COMMAND:
        if (HIWORD(wParam) == EN_CHANGE) 
        {
            InvalidateRect(hWnd, NULL, 0);
            updateTable(hWnd, winWidth, winHeight);
        }
        break;
    case WM_VSCROLL: 
    {
        SCROLLINFO scrollInfo;

        scrollInfo.cbSize = sizeof(SCROLLINFO);
        scrollInfo.fMask = SIF_ALL;

        GetScrollInfo(hWnd, SB_VERT, &scrollInfo);
        int currPosition = scrollInfo.nPos;

        switch (LOWORD(wParam)) {
            case SB_THUMBPOSITION:
                currPosition = HIWORD(wParam);
                break;
            case SB_LINEDOWN:
                currPosition++;
                break;
            case SB_LINEUP:
                currPosition--;
                break;
        }
        if (scrollInfo.nPos != currPosition) {
            positionOfScroll = getRowsHeight(currPosition);
            updateTable(hWnd,winWidth,winHeight);
            SetScrollPos(hWnd, SB_VERT, currPosition, TRUE);
        }
        break;
    }
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
        POINT Min = { 200, 200 };
        POINT  Max = { 800, 800 };
        pInfo->ptMinTrackSize = Min; 
        pInfo->ptMaxTrackSize = Max; 
        return 0;
    }
    case WM_CLOSE:
        destroyTable(hWnd);
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void createTable(HWND hwnd,int width,int height) {
    int widthOfElement = width / COUNT_OF_COLUMNS;
    int heightOfElement = defaultHeight * 2;
    for (int i = 0; i < COUNT_OF_ROWS; i++) {
        for (int j = 0; j < COUNT_OF_COLUMNS; j++) {
            addEditBox(hwnd, i, j, widthOfElement, heightOfElement);
        }
    }
}

void addEditBox(HWND hwnd, int rowIndex, int columnIndex, int width, int height) {
    int id = (rowIndex >> 16) | columnIndex;
    int XCoord = width * columnIndex;
    int YCoord = height * rowIndex;
    textTable[rowIndex][columnIndex] = CreateWindow(L"EDIT",
        0,
        WS_BORDER | WS_CLIPSIBLINGS | WS_CHILDWINDOW | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN,
        XCoord, YCoord, width, height,
        hwnd,
        (HMENU)id,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);
}

int getLastLine(HWND textTableRow[]) {
    int lastLine = (int)SendMessage(textTableRow[0], EM_GETLINECOUNT, NULL, NULL);
    for (int i = 0; i < COUNT_OF_COLUMNS; i++) {
        int tempLine = (int)SendMessage(textTableRow[i], EM_GETLINECOUNT, NULL, NULL);
        if (tempLine > lastLine) {
            lastLine = tempLine;
        }
    }
    return lastLine;
}

void updateTable(HWND hwnd, int width, int height) {
    int widthOfElement = width / COUNT_OF_COLUMNS;
    int heightOfElement;
    int y = 0;
    for (int i = 0; i < COUNT_OF_ROWS; i++) {
        int lastLine = getLastLine(textTable[i]);  
        heightOfElement = defaultHeight * (lastLine + 1);
        for (int j = 0; j < COUNT_OF_COLUMNS; j++) {
            int x = j * widthOfElement;
            MoveWindow(textTable[i][j], x, y - positionOfScroll, widthOfElement, heightOfElement, TRUE);
        }
        y += heightOfElement;
    }
}

void destroyTable(HWND hwnd) {
    for (int i = 0; i < COUNT_OF_ROWS; i++) {
        for (int j = 0; j < COUNT_OF_COLUMNS; j++) {
            DestroyWindow(textTable[i][j]);
        }
    }
}

int getRowsHeight(int rowsHeight) {
    RECT rect;
    int height = 0;

    for (int i = 0; i < rowsHeight; i++) {
        GetWindowRect(textTable[i][0], &rect);
        height += rect.bottom - rect.top;
    }
    return height;
}