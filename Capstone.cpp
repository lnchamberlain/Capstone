// UnderCover Recovery 
// Windows Desktop Application for scraping and storing keywords and flagged users from social media sites 
// Developed by Tyler and Logan 
// UAA CSCE Capstone Project Fall 2022
// 
//

#include "framework.h"
#include "Capstone.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HMENU main_menu; //Menu for the main window
HMENU config_menu; // Menu for the configuration window
HWND ConfigurationWindow;
HWND MainWindow;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void AddMenu(HWND);


/*
Button Creation: 

HWND hwndButton = CreateWindow(
    L"BUTTON",  // Predefined class; Unicode assumed 
    L"OK",      // Button text 
    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
    10,         // x position 
    10,         // y position 
    100,        // Button width
    100,        // Button height
    m_hwnd,     // Parent window
    NULL,       // No menu.
    (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
    NULL);      // Pointer not needed.

Window Creation: 

HWND CreateWindowExA(
  [in]           DWORD     dwExStyle,
  [in, optional] LPCSTR    lpClassName,
  [in, optional] LPCSTR    lpWindowName,
  [in]           DWORD     dwStyle,
  [in]           int       X,
  [in]           int       Y,
  [in]           int       nWidth,
  [in]           int       nHeight,
  [in, optional] HWND      hWndParent,
  [in, optional] HMENU     hMenu,
  [in, optional] HINSTANCE hInstance,
  [in, optional] LPVOID    lpParam
);
*/


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

   // Create Configuration Window
      //LPCSTR ConfigWin = nullptr;
      //HWND ConfigWindow = CreateWindowExA(WS_EX_APPWINDOW, nullptr, ConfigWin, WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);
      //ConfigurationWindow = ConfigWindow;

   //Create buttons 
     //HWND testButton = CreateWindow(L"BUTTON", L"Testing", BS_DEFPUSHBUTTON, 10, 10, 100, 100, nullptr, NULL, (HINSTANCE)GetWindowLongPtr(nullptr, GWLP_HINSTANCE), NULL);

    


    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CAPSTONE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);




    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CAPSTONE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            //Dispatch messages sends the message to be process through the WndProc function
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CAPSTONE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CAPSTONE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main and configuration windows
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle global variable

   HWND hWnd = CreateWindowW(szWindowClass, L"UnderCover Recovery", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   MainWindow = hWnd;

   // Create Configuration Window and store in global variable
   HWND config = CreateWindow(szWindowClass, L"Configuration", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 400, 400, 800, 600, hWnd, NULL, hInstance, NULL);
   ConfigurationWindow = config;





   if (!hWnd)
   {
      return FALSE;
   }
   
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
    switch (message)
    {
    case WM_CREATE:
    {   
        AddMenu(hWnd);
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
             // File Tab
            case 1:
                MessageBeep(MB_OK);
                break;
            // Configuration Tab
            case 2:
                //STUB
                break;
            // About Tab
            case 3:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    //Manages the Red 'X'
    case WM_DESTROY:
        //In case of closing the Configuration Window, should not exit program only that window
        if (hWnd == ConfigurationWindow) {
            break;
        }
        //Makes 'Get message' return false and terminate program
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


// Called in WndProc WM_CREATE case
void AddMenu(HWND hWnd)
{    

    
    HMENU hmenu = CreateMenu();
    HMENU M_main = CreateMenu();
    HMENU M_config = CreateMenu();

    AppendMenu(M_main, MF_STRING, 1, L"File");
    AppendMenu(M_main, MF_STRING, 2, L"Configuration");
    AppendMenu(M_main, MF_STRING, 3, L"About");

    AppendMenu(M_config, MF_STRING, NULL, L"Testing");


    // TODO figure out how to differentiate between the two calling windows 
    //Specify which menu to add depending on the calling window
    if (hWnd == ConfigurationWindow) 
    {
        hmenu = M_config;
    }
   /* if (hWnd == MainWindow) {
        hmenu = M_main;
    }
    */
    hmenu = M_main;
    SetMenu(hWnd, hmenu);
    



}
