#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "resource.h"


#define MAX_LOADSTRING 100
#define COLUMN_COUNT 7
#define ROW_COUNT 15


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND textTable[ROW_COUNT][COLUMN_COUNT];        // table of EditBoxes
const int defaultHeight = 18;
int positionOfScroll = 0;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void createTable(HWND, int, int);
void redrawTable(HWND, int, int);
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
   scrollInfo.nMax = ROW_COUNT - 1;
   scrollInfo.nPage = 1;
   scrollInfo.fMask = SIF_ALL;

   SetScrollInfo(hWnd, SB_VERT, &scrollInfo, TRUE);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    static int windowWidth, windowHeight;
    static char sym;
    switch (message)
    {
    case WM_CREATE:
        GetClientRect(hWnd, &rect);
        windowWidth = rect.right - rect.left;
        windowHeight = rect.bottom - rect.top;
        createTable(hWnd, windowWidth, windowHeight);
        break;
    case WM_SIZE:
        GetClientRect(hWnd, &rect);
        windowWidth = rect.right - rect.left;
        windowHeight = rect.bottom - rect.top;
        redrawTable(hWnd, windowWidth, windowHeight);
        InvalidateRect(hWnd, NULL, 0);
        break;
    case WM_COMMAND:
        if (HIWORD(wParam) == EN_CHANGE) 
        {
            InvalidateRect(hWnd, NULL, 0);
            redrawTable(hWnd, windowWidth, windowHeight);
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
            redrawTable(hWnd,windowWidth,windowHeight);
            SetScrollPos(hWnd, SB_VERT, currPosition, TRUE);
        }
        break;
    }
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
        POINT Min = { 200, 200 };
        pInfo->ptMinTrackSize = Min; 
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
    int cellWidth = width / COLUMN_COUNT;
    int cellHeight = defaultHeight * 2;
    for (int i = 0; i < ROW_COUNT; i++) {
        for (int j = 0; j < COLUMN_COUNT; j++) {
            addEditBox(hwnd, i, j, cellWidth, cellHeight);
        }
    }
}

void addEditBox(HWND hwnd, int rowIndex, int columnIndex, int width, int height) {
    int id = (rowIndex >> 16) | columnIndex;
    int xCoord = width * columnIndex;
    int yCoord = height * rowIndex;
    textTable[rowIndex][columnIndex] = CreateWindow(L"EDIT",
        0,
        WS_BORDER | WS_CLIPSIBLINGS | WS_CHILDWINDOW | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN,
        xCoord, yCoord, width, height,
        hwnd,
        (HMENU)id,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);
}

int getLastLine(HWND textTableRow[]) {
    int lastLine = (int)SendMessage(textTableRow[0], EM_GETLINECOUNT, NULL, NULL);
    for (int i = 1; i < COLUMN_COUNT; i++) {
        int tempLine = (int)SendMessage(textTableRow[i], EM_GETLINECOUNT, NULL, NULL);
        if (tempLine > lastLine) {
            lastLine = tempLine;
        }
    }
    return lastLine;
}

void redrawTable(HWND hwnd, int width, int height) {
    int cellWidth = width / COLUMN_COUNT;
    int cellHeight;
    int y = 0;
    for (int i = 0; i < ROW_COUNT; i++) {
        int numberOfLines = getLastLine(textTable[i]);  
        cellHeight = defaultHeight + (numberOfLines*16);
        for (int j = 0; j < COLUMN_COUNT; j++) {
            int x = j * cellWidth;
            MoveWindow(textTable[i][j], x, y - positionOfScroll, cellWidth, cellHeight, TRUE);
        }
        y += cellHeight;
    }
}

void destroyTable(HWND hwnd) {
    for (int i = 0; i < ROW_COUNT; i++) {
        for (int j = 0; j < COLUMN_COUNT; j++) {
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