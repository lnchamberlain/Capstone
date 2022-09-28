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
#include <wchar.h>
#include <string.h>
#include <cmath>

#define MAX_LOADSTRING 100
#define FILE_MENU_FILE 1
#define FILE_MENU_OPEN_CONFIGURATION 2
#define FILE_MENU_ABOUT 3
#define FILE_MENU_SAVE_CONFIGURATION 4
#define FACEBOOK_LOGIN 5
#define INSTAGRAM_LOGIN 6
#define TWITTER_LOGIN 7
#define SET_TIMER_VALUE 8
#define FILE_HELP_SCAN 9
#define FILE_HELP_LOGIN 10
#define FILE_HELP_WORDLIST 11
 
//#define WM_SETFONT                      0x0200


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND MainWindow, timerWnd;                     // Global variable for main window and timer window
HWND facebookSection, instagramSection, twitterSection; // TODO make these seperate processes with their own procedures
int COUNT = 0;                                  // Counts down                    
int RESET_TIMER = 20;                           // Reset timer on finish (in seconds)
int seconds, minutes, hours, days;              //Convert COUNT into readable format
UINT_PTR ID_TIMER;                              // Timer ID
HWND fbUser, fbPass, igUser, igPass, twUser, twPass, enteredTime; // captured values
wchar_t fbUsername[100], fbPassword[100], igUsername[100], igPassword[100], twUsername[100], twPassword[100], Freq[100]; // store captured values
char buf[] = { '00', ':', '00', ':', '00' };
HWND HOURS, MINUTES, SECONDS;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void AddMenu(HWND);
void AddControls(HWND);
void Test();
void InitializeTimer();



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

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
      200, 100, 1200, 850, nullptr, nullptr, hInstance, nullptr);
   //Main Window Coordinates (0,0) upper left, (1200, 800) lower right
   
   // Store in global variable
   MainWindow = hWnd;

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
        AddControls(hWnd);  
        break;
    case WM_TIMER:
        
        if (COUNT > 0) 
        {
            hours = COUNT / 3600;
            minutes = (COUNT / 60) % 60;
            seconds = COUNT % 60;
            buf[0] = (char)hours;
            buf[2] = (char)minutes;
            buf[4] = (char)seconds;
            size_t size = strlen(buf) + 1;
            size_t outSize;
            wchar_t wbuf[6];
            mbstowcs_s(&outSize, wbuf, size, buf, size - 1);
            SetWindowTextW(timerWnd, wbuf);
            //Test();
        }
        COUNT--;
        if (COUNT <= 0) 
        {
            //Reset timer automatically
            COUNT = RESET_TIMER;
        }
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections
        switch (wmId)
        {
            //File Tab
        case FILE_MENU_FILE:
            break;
            //Configuration Tab -> open panel
        case FILE_MENU_OPEN_CONFIGURATION:
            break;
            //Configuration Tab -> save config
        case FILE_MENU_SAVE_CONFIGURATION:
            break;
            //About Tab
        case FILE_MENU_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
           //Help Tab -> Login Help
        case FILE_HELP_LOGIN:
            break;
            //Help Tab -> Wordlist Help
        case FILE_HELP_WORDLIST:
            break;
            //Help Tab -> Scanner Help Help
        case FILE_HELP_SCAN:
            break;
        case FACEBOOK_LOGIN:
            //GetWindowTextW(fbUser, fbUsername, 100);
            //GetWindowTextW(fbPass, fbPassword, 100);
            //Post login request here
            break;
        case INSTAGRAM_LOGIN:
            //GetWindowTextW(igUser, igUsername, 100);
            //GetWindowTextW(igPass, igPassword, 100);
            //Post login request here
            break;
        case TWITTER_LOGIN:
            //GetWindowTextW(twUser, twUsername, 100);
            //GetWindowTextW(twPass, twPassword, 100);
            //Post login request here
            break;
        case SET_TIMER_VALUE:
            GetWindowTextW(enteredTime, Freq, 100);
            COUNT = (int)Freq;
            std::cout << COUNT << std::endl;
            RESET_TIMER = COUNT;
            InitializeTimer();
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
        //Doesn't display?
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            /*
            if (COUNT > 0) {
                hours = COUNT / 3600;
                minutes = (COUNT / 60) % 60;
                seconds = COUNT % 60;
                buf[0] = (char)hours;
                buf[1] = (char)minutes;
                buf[2] = (char)seconds;
                DrawText(hdc, (LPCWSTR) buf, -1, &r, DT_LEFT);
            }
            */
            EndPaint(hWnd, &ps);
        }
        break;
    //Manages the Red 'X'
    case WM_DESTROY:
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

// Called in WndProc WM_CREATE case, adds menu to the main window
void AddMenu(HWND hWnd)
{
    HMENU menuMain = CreateMenu();
    
    // Submenus for Main
    HMENU menuMainSubFile = CreateMenu();
    HMENU menuMainSubHelp = CreateMenu();
    SetWindowTextW(MainWindow, L"Testing Testing");
   

    // Populate drop-down menus
    AppendMenu(menuMainSubFile, MF_STRING, IDM_EXIT, L"Exit");
    AppendMenu(menuMainSubHelp, MF_STRING, FILE_HELP_SCAN, L"Scan Help");
    AppendMenu(menuMainSubHelp, MF_STRING, FILE_HELP_LOGIN, L"Login Help");
    AppendMenu(menuMainSubHelp, MF_STRING, FILE_HELP_WORDLIST, L"Wordlist Help");
    //Populate primary menu items
    AppendMenu(menuMain, MF_POPUP, (UINT_PTR)menuMainSubFile, L"File");
    AppendMenu(menuMain, MF_POPUP, (UINT_PTR)menuMainSubHelp, L"Help");
    AppendMenu(menuMain, MF_STRING, FILE_MENU_ABOUT, L"About");
    SetMenu(hWnd, menuMain);
}

//Add static and edit controls to windows
void AddControls(HWND hWnd)
{

    //Create boxes to fill with Timer values, store h,m, and s in global variables for re-painting on WM_TIMER
    HWND timeTillNextScanBox = CreateWindowW(L"Static", L"Time Until Next Scan:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 406, 50, 140, 20, hWnd, NULL, NULL, NULL);
    HWND timerBox = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 549, 49, 97, 22, hWnd, NULL, NULL, NULL);
    HOURS = CreateWindowW(L"Edit", L"00", WS_VISIBLE | WS_CHILD | SS_CENTER, 550, 50, 25, 20, hWnd, NULL, NULL, NULL);
    HWND space_1 = CreateWindowW(L"Static", L":", WS_VISIBLE | WS_CHILD | SS_CENTER, 575, 50, 10, 20, hWnd, NULL, NULL, NULL);
    MINUTES = CreateWindowW(L"Edit", L"00", WS_VISIBLE | WS_CHILD | SS_CENTER, 585, 50, 25, 20, hWnd, NULL, NULL, NULL);
    HWND space_2 = CreateWindowW(L"Static", L":", WS_VISIBLE | WS_CHILD | SS_CENTER, 610, 50, 10, 20, hWnd, NULL, NULL, NULL);
    SECONDS = CreateWindowW(L"Edit", L"00", WS_VISIBLE | WS_CHILD | SS_CENTER, 620, 50, 25, 20, hWnd, NULL, NULL, NULL);


    HWND openConfigButton = CreateWindowW(L"Button", L"Configuration", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON |BS_CENTER, 670, 45, 110, 30, hWnd, (HMENU)SET_TIMER_VALUE, NULL, NULL);
    HWND stopScanningButton = CreateWindow(L"Button", L"Stop Scanning", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_CENTER, 670, 80, 100, 30, hWnd, NULL, NULL, NULL);

    //Add Scan count and message
    HWND scanCountText = CreateWindowW(L"Static", L"Scan Count:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 460, 75, 85, 20, hWnd, NULL, NULL, NULL);
    HWND scanCount = CreateWindowW(L"Edit", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 550, 75, 25, 20, hWnd, NULL, NULL, NULL);


    //Main three blocks
    facebookSection = CreateWindowW(L"Static", L"Facebook", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 130, 300, 500, hWnd, NULL, NULL, NULL);
    instagramSection = CreateWindowW(L"Static", L"Instagram", WS_VISIBLE | WS_CHILD | WS_BORDER, 450, 130, 300, 500, hWnd, NULL, NULL, NULL);
    twitterSection = CreateWindowW(L"Static", L"Twitter", WS_VISIBLE | WS_CHILD | WS_BORDER, 800, 130, 300, 500, hWnd, NULL, NULL, NULL, NULL);

    //Initialize username and password windows
    /*
    fbUser = CreateWindowW(L"Edit", L"Username", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | SS_CENTER, 215, 270, 100, 30, hWnd, NULL, NULL, NULL);
    fbPass = CreateWindowW(L"Edit", L"Password", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | SS_CENTER, 215, 305, 100, 30, hWnd, NULL, NULL, NULL);
    igUser = CreateWindowW(L"Edit", L"Username", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | SS_CENTER, 565, 270, 100, 30, hWnd, NULL, NULL, NULL);
    igPass = CreateWindowW(L"Edit", L"Password", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | SS_CENTER, 565, 305, 100, 30, hWnd, NULL, NULL, NULL);
    twUser = CreateWindowW(L"Edit", L"Username", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | SS_CENTER, 915, 270, 100, 30, hWnd, NULL, NULL, NULL);
    twPass = CreateWindowW(L"Edit", L"Password", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | SS_CENTER, 915, 305, 100, 30, hWnd, NULL, NULL, NULL);
    */
    //Populate login buttons
    HWND facebookLoginButton = CreateWindowW(L"Button", L"LOGIN", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 225, 205, 55, 30, hWnd, (HMENU)FACEBOOK_LOGIN, NULL, NULL);
    HWND instagramLoginButton = CreateWindowW(L"Button", L"LOGIN", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 575, 205, 55, 30, hWnd, (HMENU)INSTAGRAM_LOGIN, NULL, NULL);
    HWND twitterLoginButton = CreateWindowW(L"Button", L"LOGIN", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 925, 205, 55, 30, hWnd, (HMENU)TWITTER_LOGIN, NULL, NULL);


    //Populate flagged users buttons
    HWND facebookFlaggedUserButton = CreateWindowW(L"Button", L"Flagged Users", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 290, 130, 100, 30, hWnd, NULL, NULL, NULL);
    HWND instagramFlaggedUserButton = CreateWindowW(L"Button", L"Flagged Users", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 640, 130, 100, 30, hWnd, NULL, NULL, NULL);
    HWND twitterFlaggedUserButton = CreateWindowW(L"Button", L"Flagged Users", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 990, 130, 100, 30, hWnd, NULL, NULL, NULL);

    //Add Export Results Buttons
    HWND facebookExportButton = CreateWindowW(L"Button", L"Export Full Scan Results", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE, 290, 560, 100, 60, hWnd, NULL, NULL, NULL);
    HWND instagramExportButton = CreateWindowW(L"Button", L"Export Full Scan Results", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE, 640, 560, 100, 60, hWnd, NULL, NULL, NULL);
    HWND twitterExportButton = CreateWindowW(L"Button", L"Export Full Scan Results", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE, 990, 560, 100, 60, hWnd, NULL, NULL, NULL);

    HWND launchButton = CreateWindowW(L"Button", L"LAUCH SCAN", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON| BS_CENTER, 550, 655, 100, 60, hWnd, NULL, NULL, NULL);
   
}


//Sets up the timer and starts it going, will send WM_TIMER messages every second 
void InitializeTimer()
{   
    //One second intervals
    SetTimer(MainWindow, NULL, 500, NULL);

}
//Should make the window header the facebook username if value is captured correctly
void Test() 
{
    SetWindowTextW(MainWindow, L"Testing Testing");

}