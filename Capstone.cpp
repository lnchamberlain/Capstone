// UnderCover Recovery 
// Windows Desktop Application for scraping and storing keywords and flagged users from social media sites 
// Developed by Tyler and Logan 
// UAA CSCE Capstone Project Fall 2022
// Main Architecture
//

#include "framework.h"
#include "Capstone.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <wchar.h>
#include <string.h>
#include <cmath>
//#include <Python.h>

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
#define CONFIG_PANEL 12
#define FACEBOOK_LOGIN_SUBMIT 13
#define INSTAGRAM_LOGIN_SUBMIT 14
#define TWITTER_LOGIN_SUBMIT 15
 
// Global Variables:
HINSTANCE hInst, hInstanceConfig, hInstanceFBLogin, hInstanceIGLogin, hInstanceTWLogin;  
WCHAR szTitle[MAX_LOADSTRING];                 // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND MainWindow, timerWnd;                     // Global variable for main window and timer window
HWND facebookSection, instagramSection, twitterSection; // TODO make these seperate processes with their own procedures
int COUNT = 0;                                  // Counts down                    
int RESET_TIMER = 20;                           // Reset timer on finish (in seconds)
int seconds, minutes, hours, days;              //Convert COUNT into readable format
UINT_PTR ID_TIMER;                              // Timer ID
HWND fbUser, fbPass, igUser, igPass, twUser, twPass, enteredTime; // captured values
WCHAR fbUsername[100], fbPassword[100], igUsername[100], igPassword[100], twUsername[100], twPassword[100], Freq[100]; // store captured value
HWND HOURS, MINUTES, SECONDS;
const WCHAR *configClassName = L"ConfigClassName";
const WCHAR* FBLoginClassName = L"FacebookLoginClassName";
const WCHAR* IGLoginClassName = L"InstagramLoginClassName";
const WCHAR* TWLoginClassName = L"TwitterLoginClassName";


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM); // WndProc for Main Window
LRESULT CALLBACK WndProcConfig(HWND, UINT, WPARAM, LPARAM); // WndProc for Configuration Window
LRESULT CALLBACK WndProcFBLogin(HWND, UINT, WPARAM, LPARAM); // WndProc for Facebook Login Window
LRESULT CALLBACK WndProcIGLogin(HWND, UINT, WPARAM, LPARAM); // WndProc for Facebook Instgram Window
LRESULT CALLBACK WndProcTWLogin(HWND, UINT, WPARAM, LPARAM); // WndProc for Facebook Twitter Window
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void createConfigurationWindow(WNDCLASSEXW& config_cl, HINSTANCE& hInst_config, int nCmdShow, HWND parent);
void createFacebookLoginWindow(WNDCLASSEXW& fb_cl, HINSTANCE& hInst_fb, int nCmdShow, HWND parent);
void createInstagramLoginWindow(WNDCLASSEXW& ig_cl, HINSTANCE& hInst_ig, int nCmdShow, HWND parent);
void createTwitterLoginWindow(WNDCLASSEXW& tw_cl, HINSTANCE& hInst_tw, int nCmdShow, HWND parent);
void AddMenu(HWND);
void AddControls(HWND);
void AddConfigControls(HWND);
void AddFBLoginControls(HWND);
void AddIGLoginControls(HWND);
void AddTWLoginControls(HWND);
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

//Create Configuration Window 
void createConfigurationWindow(WNDCLASSEXW& config_cl, HINSTANCE& hInst_config, int nCmdShow, HWND parent)
{
    config_cl.cbSize = sizeof(WNDCLASSEX);
    config_cl.style = CS_HREDRAW | CS_VREDRAW;
    config_cl.lpfnWndProc = WndProcConfig;
    config_cl.cbClsExtra = 0;
    config_cl.cbWndExtra = 0;
    config_cl.hInstance = hInst_config;
    config_cl.hIcon = LoadIcon(hInst_config, MAKEINTRESOURCE(IDI_CAPSTONE));
    config_cl.hCursor = LoadCursor(nullptr, IDC_ARROW);
    config_cl.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    config_cl.lpszMenuName = NULL;
    config_cl.lpszClassName = configClassName;
    config_cl.hIconSm = LoadIcon(config_cl.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    if (!RegisterClassExW(&config_cl))
    {
       
        int nResult = GetLastError();
        
        /*MessageBox(NULL,
            L"Window class creation failed",
            L"Window Class Failed",
            MB_ICONERROR);
            */
    }
    
    //Create window after registering class
    HWND confighWnd = CreateWindowW((LPCWSTR)configClassName, L"Configuration", WS_OVERLAPPEDWINDOW, 300, 200, 600, 400, parent, NULL, hInst_config, NULL);
    ShowWindow(confighWnd, nCmdShow);

}

//Create Facebook Login Window 
void createFacebookLoginWindow(WNDCLASSEXW& fb_cl, HINSTANCE& hInst_fb, int nCmdShow, HWND parent)
{
    fb_cl.cbSize = sizeof(WNDCLASSEX);
    fb_cl.style = CS_HREDRAW | CS_VREDRAW;
    fb_cl.lpfnWndProc = WndProcFBLogin;
    fb_cl.cbClsExtra = 0;
    fb_cl.cbWndExtra = 0;
    fb_cl.hInstance = hInst_fb;
    fb_cl.hIcon = LoadIcon(hInst_fb, MAKEINTRESOURCE(IDI_CAPSTONE));
    fb_cl.hCursor = LoadCursor(nullptr, IDC_ARROW);
    fb_cl.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    fb_cl.lpszMenuName = NULL;
    fb_cl.lpszClassName = FBLoginClassName;
    fb_cl.hIconSm = LoadIcon(fb_cl.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    if (!RegisterClassExW(&fb_cl))
    {

        int nResult = GetLastError();

        /*MessageBox(NULL,
            L"Window class creation failed",
            L"Window Class Failed",
            MB_ICONERROR);
            */
    }

    //Create window after registering class
    HWND fbLoginhWnd = CreateWindowW((LPCWSTR)FBLoginClassName, L"Facebook", WS_OVERLAPPEDWINDOW, 400, 200, 400, 300, parent, NULL, hInst_fb, NULL);
    ShowWindow(fbLoginhWnd, nCmdShow);

}

//Create Facebook Login Window 
void createInstagramLoginWindow(WNDCLASSEXW& ig_cl, HINSTANCE& hInst_ig, int nCmdShow, HWND parent)
{
    ig_cl.cbSize = sizeof(WNDCLASSEX);
    ig_cl.style = CS_HREDRAW | CS_VREDRAW;
    ig_cl.lpfnWndProc = WndProcIGLogin;
    ig_cl.cbClsExtra = 0;
    ig_cl.cbWndExtra = 0;
    ig_cl.hInstance = hInst_ig;
    ig_cl.hIcon = LoadIcon(hInst_ig, MAKEINTRESOURCE(IDI_CAPSTONE));
    ig_cl.hCursor = LoadCursor(nullptr, IDC_ARROW);
    ig_cl.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    ig_cl.lpszMenuName = NULL; 
    ig_cl.lpszClassName = IGLoginClassName;
    ig_cl.hIconSm = LoadIcon(ig_cl.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    if (!RegisterClassExW(&ig_cl))
    {

        int nResult = GetLastError();

       /* MessageBox(NULL,
            L"Window class creation failed",
            L"Window Class Failed",
            MB_ICONERROR);
            */
    }

    //Create window after registering class
    HWND igLoginhWnd = CreateWindowW((LPCWSTR)IGLoginClassName, L"Instagram", WS_OVERLAPPEDWINDOW, 400, 200, 400, 300, parent, NULL, hInst_ig, NULL);
    ShowWindow(igLoginhWnd, nCmdShow);

}

//Create Facebook Login Window 
void createTwitterLoginWindow(WNDCLASSEXW& tw_cl, HINSTANCE& hInst_tw, int nCmdShow, HWND parent)
{
    tw_cl.cbSize = sizeof(WNDCLASSEX);
    tw_cl.style = CS_HREDRAW | CS_VREDRAW;
    tw_cl.lpfnWndProc = WndProcTWLogin;
    tw_cl.cbClsExtra = 0;
    tw_cl.cbWndExtra = 0;
    tw_cl.hInstance = hInst_tw;
    tw_cl.hIcon = LoadIcon(hInst_tw, MAKEINTRESOURCE(IDI_CAPSTONE));
    tw_cl.hCursor = LoadCursor(nullptr, IDC_ARROW);
    tw_cl.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    tw_cl.lpszMenuName = NULL;
    tw_cl.lpszClassName = TWLoginClassName;
    tw_cl.hIconSm = LoadIcon(tw_cl.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    if (!RegisterClassExW(&tw_cl))
    {

        int nResult = GetLastError();

       /* MessageBox(NULL,
            L"Window class creation failed",
            L"Window Class Failed",
            MB_ICONERROR);
            */
    }

    //Create window after registering class
    HWND twLoginhWnd = CreateWindowW((LPCWSTR)TWLoginClassName, L"Twitter", WS_OVERLAPPEDWINDOW, 400, 200, 400, 300, parent, NULL, hInst_tw, NULL);
    ShowWindow(twLoginhWnd, nCmdShow);

}


//Main Window Procedure
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
            //buf[0] = (char)hours;
            //buf[2] = (char)minutes;
            //buf[4] = (char)seconds;
            //size_t size = strlen(buf) + 1;
            //size_t outSize;
            //wchar_t wbuf[6];
            //mbstowcs_s(&outSize, wbuf, size, buf, size - 1);
            //SetWindowTextW(timerWnd, wbuf);
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
            WNDCLASSEXW fbLoginWindow;
            hInstanceFBLogin = (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE);
            createFacebookLoginWindow(fbLoginWindow, hInstanceFBLogin, SW_SHOW, hWnd);
            break;
        case INSTAGRAM_LOGIN:
            WNDCLASSEXW igLoginWindow;
            hInstanceIGLogin = (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE);
            createInstagramLoginWindow(igLoginWindow, hInstanceIGLogin, SW_SHOW, hWnd);
            break;
            //GetWindowTextW(igUser, igUsername, 100);
            //GetWindowTextW(igPass, igPassword, 100);
        case TWITTER_LOGIN:
            WNDCLASSEXW twLoginWindow;
            hInstanceTWLogin = (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE);
            createTwitterLoginWindow(twLoginWindow, hInstanceTWLogin, SW_SHOW, hWnd);
            break;
            //GetWindowTextW(twUser, twUsername, 100);
            //GetWindowTextW(twPass, twPassword, 100);
        case SET_TIMER_VALUE:
            GetWindowTextW(enteredTime, Freq, 100);
            COUNT = (int)Freq;
            std::cout << COUNT << std::endl;
            RESET_TIMER = COUNT;
            InitializeTimer();
            break;
        //Launch Configuration Panel IN PROGRESS
        case CONFIG_PANEL:
            WNDCLASSEXW configWindow;
            hInstanceConfig = (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE);
            createConfigurationWindow(configWindow, hInstanceConfig, SW_SHOW, hWnd);
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

//Window Procedure for the configuration panel
LRESULT CALLBACK WndProcConfig(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId = LOWORD(wParam);
    switch (message)
    {
    case WM_CREATE:
        AddConfigControls(hWnd);
        break;
    case WM_COMMAND:
        switch (wmId)
        {
            //Add button controls, capture text, etc here

        case WM_DESTROY:
            DestroyWindow(hWnd);
            break;
        }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//Window Procedure for the Facebook Login Panel
LRESULT CALLBACK WndProcFBLogin(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId = LOWORD(wParam);
    switch (message)
    {
    case WM_CREATE:
        AddFBLoginControls(hWnd);
        break;
    case WM_COMMAND:
        switch (wmId)
        {
        case FACEBOOK_LOGIN_SUBMIT:
            //Grab values from the username and password fields, store in global variables
            GetWindowTextW(fbUser, fbUsername, 100);
            GetWindowTextW(fbPass, fbPassword, 100);
            SetWindowTextW(fbUser, L"");
            SetWindowTextW(fbPass, L"");
            //PASS TO AUTH SCRIPT HERE
            DestroyWindow(hWnd);
            break;
        }
     case WM_DESTROY:
        //DestroyWindow(hWnd);
        break;       
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//Window Procedure for the Instagram Login Panel
LRESULT CALLBACK WndProcIGLogin(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId = LOWORD(wParam);
    switch (message)
    {
    case WM_CREATE:
        AddIGLoginControls(hWnd);
        break;
    case WM_COMMAND:
        switch (wmId)
        {
        case INSTAGRAM_LOGIN_SUBMIT:
            //Grab values from the username and password fields, store in global variables
            GetWindowTextW(igUser, igUsername, 100);
            GetWindowTextW(igPass, igPassword, 100);
            SetWindowTextW(igUser, L"");
            SetWindowTextW(igPass, L"");
            //PASS TO AUTH SCRIPT HERE
            DestroyWindow(hWnd);
            break;
            //Add button controls, capture text, etc here
        }

        case WM_DESTROY:
            //DestroyWindow(hWnd);
            break;
        
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//Window Procedure for the Twitter Login Panel
LRESULT CALLBACK WndProcTWLogin(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        AddTWLoginControls(hWnd);
        break;
    case WM_COMMAND:
        switch (wParam)
        {
        case TWITTER_LOGIN_SUBMIT:
            //Grab values from the username and password fields, store in global variables
            GetWindowTextW(twUser, twUsername, 100);
            GetWindowTextW(twPass, twPassword, 100);
            SetWindowTextW(twUser, L"");
            SetWindowTextW(twPass, L"");
            //PASS TO AUTH SCRIPT HERE
            DestroyWindow(hWnd);
            break;
        }
    case WM_DESTROY:
        //DestroyWindow(hWnd);
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

//Adds features to the main window
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


    HWND openConfigButton = CreateWindowW(L"Button", L"Configuration", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON |BS_CENTER, 670, 45, 110, 30, hWnd, (HMENU)CONFIG_PANEL, NULL, NULL);
    HWND stopScanningButton = CreateWindow(L"Button", L"Stop Scanning", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_CENTER, 670, 80, 100, 30, hWnd, NULL, NULL, NULL);

    //Add Scan count and message
    HWND scanCountText = CreateWindowW(L"Static", L"Scan Count:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 460, 75, 85, 20, hWnd, NULL, NULL, NULL);
    HWND scanCount = CreateWindowW(L"Edit", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 550, 75, 25, 20, hWnd, NULL, NULL, NULL);


    //Main three blocks
    facebookSection = CreateWindowW(L"Static", L"Facebook", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 130, 300, 500, hWnd, NULL, NULL, NULL);
    instagramSection = CreateWindowW(L"Static", L"Instagram", WS_VISIBLE | WS_CHILD | WS_BORDER, 450, 130, 300, 500, hWnd, NULL, NULL, NULL);
    twitterSection = CreateWindowW(L"Static", L"Twitter", WS_VISIBLE | WS_CHILD | WS_BORDER, 800, 130, 300, 500, hWnd, NULL, NULL, NULL, NULL);

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

//Add elements to the configuration window
void AddConfigControls(HWND hWnd)
{




}

// Add elements to FB login window
void AddFBLoginControls(HWND hWnd) 
{

    HWND titleText = CreateWindowW(L"Static", L"Facebook Login", WS_VISIBLE | WS_CHILD | SS_CENTER, 120, 10, 150, 30, hWnd, NULL, NULL, NULL);
    HWND usernameTextBox = CreateWindowW(L"Static", L"Username:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 50, 70, 30, hWnd, NULL, NULL, NULL);
    fbUser = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER| ES_MULTILINE | ES_AUTOVSCROLL | SS_LEFT, 95, 50, 100, 30, hWnd, NULL, NULL, NULL);
    HWND passwordTextBox = CreateWindowW(L"Static", L"Password:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 85, 70, 30, hWnd, NULL, NULL, NULL);
    fbPass = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | SS_LEFT, 95, 85, 100, 30, hWnd, NULL, NULL, NULL);
    HWND submit = CreateWindowW(L"Button", L"SUBMIT", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | SS_CENTER, 160, 140, 60, 40, hWnd, (HMENU)FACEBOOK_LOGIN_SUBMIT, NULL, NULL);
    HWND checkBoxText = CreateWindowW(L"Static", L"Use Saved?", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT, 250, 80, 150, 30, hWnd, NULL, NULL, NULL);
    HWND checkBox = CreateWindowW(L"Button", L"", WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BST_CHECKED | SS_CENTER, 350, 85, 20, 20, hWnd, NULL, NULL, NULL);
}

// Add elements to FB login window
void AddIGLoginControls(HWND hWnd) 
{

    HWND titleText = CreateWindowW(L"Static", L"Instagram Login", WS_VISIBLE | WS_CHILD | SS_CENTER, 120, 10, 150, 30, hWnd, NULL, NULL, NULL);
    HWND usernameTextBox = CreateWindowW(L"Static", L"Username:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 50, 70, 30, hWnd, NULL, NULL, NULL);
    igUser = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | SS_LEFT, 95, 50, 100, 30, hWnd, NULL, NULL, NULL);
    HWND passwordTextBox = CreateWindowW(L"Static", L"Password:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 85, 70, 30, hWnd, NULL, NULL, NULL);
    igPass = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | SS_LEFT, 95, 85, 100, 30, hWnd, NULL, NULL, NULL);
    HWND submit = CreateWindowW(L"Button", L"SUBMIT", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | SS_CENTER, 160, 140, 60, 40, hWnd, (HMENU)INSTAGRAM_LOGIN_SUBMIT, NULL, NULL);
    HWND checkBoxText = CreateWindowW(L"Static", L"Use Saved?", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT, 250, 80, 150, 30, hWnd, NULL, NULL, NULL);
    HWND checkBox = CreateWindowW(L"Button", L"", WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BST_CHECKED | SS_CENTER, 350, 85, 20, 20, hWnd, NULL, NULL, NULL);
}

// Add elements to FB login window
void AddTWLoginControls(HWND hWnd) 
{

    HWND titleText = CreateWindowW(L"Static", L"Twitter Login", WS_VISIBLE | WS_CHILD | SS_CENTER, 120, 10, 150, 30, hWnd, NULL, NULL, NULL);
    HWND usernameTextBox = CreateWindowW(L"Static", L"Username:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 50, 70, 30, hWnd, NULL, NULL, NULL);
    twUser = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | SS_LEFT, 95, 50, 100, 30, hWnd, NULL, NULL, NULL);
    HWND passwordTextBox = CreateWindowW(L"Static", L"Password:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 85, 70, 30, hWnd, NULL, NULL, NULL);
    twPass = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | SS_LEFT, 95, 85, 100, 30, hWnd, NULL, NULL, NULL);
    HWND submit = CreateWindowW(L"Button", L"SUBMIT", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | SS_CENTER, 160, 140, 60, 40, hWnd, (HMENU)TWITTER_LOGIN_SUBMIT, NULL, NULL);
    HWND checkBoxText = CreateWindowW(L"Static", L"Use Saved?", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT, 250, 80, 150, 30, hWnd, NULL, NULL, NULL);
    HWND checkBox = CreateWindowW(L"Button", L"", WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BST_CHECKED | SS_CENTER, 350, 85, 20, 20, hWnd, NULL, NULL, NULL);
}

//Sets up the timer and starts it going, will send WM_TIMER messages every second 
void InitializeTimer()
{   
    //One second intervals
    SetTimer(MainWindow, NULL, 1000, NULL);

}
//Should make the window header the facebook username if value is captured correctly
void Test() 
{
    SetWindowTextW(MainWindow, L"Testing Testing");

}