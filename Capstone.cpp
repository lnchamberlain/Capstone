// Capstone.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Capstone.h"

#define MAX_LOADSTRING 100
#define FILE_MENU_FILE 1
#define FILE_MENU_OPEN_CONFIGURATION 2
#define FILE_MENU_ABOUT 3
#define FILE_MENU_SAVE_CONFIGURATION 4


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND ConfigurationWindow;
HWND MainWindow;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void AddMenu(HWND);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // 
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   MainWindow = hWnd;

   // Create Configuration Window and store in global variable
   HWND config = CreateWindow(szWindowClass, L"Configuration", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, hWnd, NULL, hInstance, NULL);
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
        AddMenu(hWnd);
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections
            switch (wmId)
            {
             // File Tab
            case FILE_MENU_FILE:
                //MessageBeep(MB_OK);
                break;
            // Configuration Tab -> Open Config Panel
            case FILE_MENU_OPEN_CONFIGURATION:
                MessageBeep(MB_OK);
                break;
            // Configuration Tab -> Save config 
            case FILE_MENU_SAVE_CONFIGURATION:
                break;
            // About Tab
            case FILE_MENU_ABOUT:
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
    case WM_DESTROY:
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
    HMENU menuMain = CreateMenu();
    HMENU menuConfig = CreateMenu();

    // Submenus for Main
    HMENU menuMainSubFile = CreateMenu();
    HMENU menuMainSubConfig = CreateMenu();
   
    // Populate drop-down menus
    AppendMenu(menuMainSubFile, MF_STRING, IDM_EXIT, L"Exit");
    //TODO Make the first a button and the second do something with the config file
    AppendMenu(menuMainSubConfig, MF_STRING, FILE_MENU_OPEN_CONFIGURATION, L"Open Configuration Panel");
    AppendMenu(menuMainSubConfig, MF_STRING, FILE_MENU_SAVE_CONFIGURATION, L"Save config file");


    //Populate primary menu items
    AppendMenu(menuMain, MF_POPUP, (UINT_PTR)menuMainSubFile, L"File");
    AppendMenu(menuMain, MF_POPUP, (UINT_PTR)menuMainSubConfig, L"Configuration");
    AppendMenu(menuMain, MF_STRING, FILE_MENU_ABOUT, L"About");

    AppendMenu(menuConfig, MF_STRING, NULL, L"Testing");

    // TODO figure out how to differentiate between the two calling windows 
    //Specify which menu to add depending on the calling window
    if (IsChild(MainWindow, hWnd)) 
    {
        hmenu = menuConfig;
    }
   else if (!IsChild(MainWindow, hWnd)) {
        hmenu = menuMain;
    }
    //hmenu = M_main;
    SetMenu(hWnd, hmenu);
    



}
