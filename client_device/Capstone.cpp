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
#include <shellapi.h>
#include <fstream>
#include <string>
#include <conio.h>
#include <fstream>
#include <algorithm>
#include <bits.h>
#include <vector>
#include <windowsx.h>
#include <thread>
#include <dwmapi.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#define DWMWA_BORDER_COLOR DWORD(34)
#define DWMWA_CAPTION_COLOR DWORD(35)
#define DWMWA_TEXT_COLOR DWORD(36)
#define MAX_LOADSTRING 100
#define EXPORT_FB 1
#define EXPORT_IG 2
#define FILE_MENU_ABOUT 3
#define FILE_MENU_SAVE_CONFIGURATION 4
#define FACEBOOK_LOGIN 5
#define INSTAGRAM_LOGIN 6
#define TWITTER_LOGIN 7
#define LAUNCH 8
#define FILE_HELP_SCAN 9
#define FILE_HELP_LOGIN 10
#define FILE_HELP_WORDLIST 11
#define CONFIG_PANEL 12
#define FACEBOOK_LOGIN_SUBMIT 13
#define INSTAGRAM_LOGIN_SUBMIT 14
#define TWITTER_LOGIN_SUBMIT 15
#define RESULTS_FB 16
#define RESULTS_IG 17
#define RESULTS_TW 18
#define WORDLIST 19
#define FLAGGED_FB 20
#define FLAGGED_IG 21
#define FLAGGED_TW 22
#define EXPORT_TW 23
#define OPEN_FLAGGED_FB 24
#define OPEN_FLAGGED_IG 25
#define OPEN_FLAGGED_TW 26
#define OPEN_KEYWORDS 27
#define SUBMIT_CONFIG 28
#define PAUSE_SCAN 29
#define RESUME_SCAN 30
#define RUN_FB_AUTH 31
#define RUN_IG_AUTH 32
#define RUN_TW_AUTH 33

 
// Global Variables:
HINSTANCE hInst, hInstanceFBLogin, hInstanceIGLogin, hInstanceTWLogin;
HINSTANCE hInstanceConfig = NULL;
WCHAR *CWD = _wgetcwd(CWD, 100);               // Current working directory
WCHAR szTitle[MAX_LOADSTRING];                 // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND MainWindow, timerWnd;                     // Global variable for main window and timer window
HWND facebookSection, instagramSection, twitterSection; // TODO make these seperate processes with their own procedures
int COUNT = 0;                                  // Counts down                    
int RESET_COUNT = 0;                          // Reset timer on finish (in seconds)
int hours, seconds, minutes, ScanCount, ScanLimitInt;              //Convert COUNT into readable format
UINT_PTR ID_TIMER;                              // Timer ID
HWND fbUser, fbPass, igUser, igPass, twUser, twPass, ScanLimit, enteredTime, Region, outputDir, scanCountDisplay; // captured values
WCHAR fbUsername[100], fbPassword[100], igUsername[100], igPassword[100], twUsername[100], twPassword[100], Freq[100]; // store captured values from Logins
WCHAR hoursCaptured[100], minutesCaptured[20], secondsCaptured[20], regionCaptured[100], scanLimitCaptured[100], outputDirCaptured[100]; // store captured values from config panel
HWND H, M, S;
HWND HOURS, MINUTES, SECONDS;
HWND FBsubmit, IGsubmit, TWsubmit;
HWND  facebookResultsSummary, instagramResultsSummary, twitterResultsSummary, launchButton, facebookLoginButton, instagramLoginButton, twitterLoginButton;
HWND checkboxFB, checkboxIG, checkboxTW;
const WCHAR *configClassName = L"ConfigClassName";
const WCHAR* FBLoginClassName = L"FacebookLoginClassName";
const WCHAR* IGLoginClassName = L"InstagramLoginClassName";
const WCHAR* TWLoginClassName = L"TwitterLoginClassName";
const char* REGIONS[5] = { "1", "2", "3", "4", "5" };
std::string SAVED_CONFIG_ELEMENTS[18] = { "" };
const char *REGION_SELECTION;
bool STOP_SCANNING = false;
bool CONFIG_SET = false;
bool CHECKED_USE_SAVED_FB = false;
bool CHECKED_USE_SAVED_IG = false;
bool CHECKED_USE_SAVED_TW = false;
bool MIN_ONE_SITE_LOGGED_IN = false;
bool NOT_DEFAULT_OUTPUT_DIR = false;
bool IG_LOGGED_IN = false;
bool FB_LOGGED_IN = false;
bool TW_LOGGED_IN = false;
bool PREV_SCAN_COMPLETE = true;
bool IG_AUTH_FINISHED = false;
bool FB_AUTH_FINISHED = false;
bool TW_AUTH_FINISHED = false;
bool FIRST_SCAN = true;
bool FB_AUTH_SUCCESS, IG_AUTH_SUCCESS, TW_AUTH_SUCCESS;
std::fstream SAVED_CONFIG_FILE;
std::string shellOperation;
HBITMAP backgroundImg, redButtonImg, greenButtonImg, toolbarImg, logoImg, scanningButtonImg, fbSectionImg, igSectionImg, twSectionImg;
HBITMAP notLoggedInButton, loggedInButton, runningButton, reAuthButton;


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
int openFileExplorer(int FLAG);
void Test();
void InitializeTimer();
void beginListeningforScrapeResults(bool fbSet, bool igSet, bool twSet);
void lauchScanners(bool fbSet, bool igSet, bool twSet);
void readFBScrapeLog();
void readIGScrapeLog();
void readTWScrapeLog();
void loadImages();



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

   HWND hWnd = CreateWindowW(szWindowClass, L"UnderCover Recovery", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
      200, 100, 1200, 850, nullptr, nullptr, hInstance, nullptr);
   //Main Window Coordinates (0,0) upper left, (1200, 800) lower right
   //BOOL USE_DARK_MODE = true;
   //BOOL SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &USE_DARK_MODE, sizeof(USE_DARK_MODE)));
   COLORREF DARK_COLOR = 0x00505050;
  
   BOOL SET_CAPTION_COLOR = SUCCEEDED(DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR,&DARK_COLOR, sizeof(DARK_COLOR)));
   COLORREF GREEN = 0xFF0000;
   //BOOL SET_TEXT_COLOR = SUCCEEDED(DwmSetWindowAttribute(hWnd, DWMWA_TEXT_COLOR, &GREEN, sizeof(GREEN)));
   BOOL SET_BORDER_COLOR = SUCCEEDED(DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &DARK_COLOR, sizeof(DARK_COLOR)));
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
        
    }
    
    //Create window after registering class
    HWND confighWnd = CreateWindowW((LPCWSTR)configClassName, L"Configuration", WS_OVERLAPPEDWINDOW, 300, 200, 500, 400, parent, NULL, hInst_config, NULL);
    ShowWindow(confighWnd, nCmdShow);
    COLORREF DARK_COLOR = 0x00505050;
    BOOL SET_CAPTION_COLOR = SUCCEEDED(DwmSetWindowAttribute(confighWnd, DWMWA_CAPTION_COLOR, &DARK_COLOR, sizeof(DARK_COLOR)));
    //COLORREF GREEN = 0xFF0000;
    //BOOL SET_TEXT_COLOR = SUCCEEDED(DwmSetWindowAttribute(confighWnd, DWMWA_TEXT_COLOR, &GREEN, sizeof(GREEN)));

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
    }

    //Create window after registering class
    HWND fbLoginhWnd = CreateWindowW((LPCWSTR)FBLoginClassName, L"Facebook", WS_OVERLAPPEDWINDOW, 400, 200, 400, 300, parent, NULL, hInst_fb, NULL);
    ShowWindow(fbLoginhWnd, nCmdShow);
    COLORREF DARK_COLOR = 0x00505050;
    BOOL SET_CAPTION_COLOR = SUCCEEDED(DwmSetWindowAttribute(fbLoginhWnd, DWMWA_CAPTION_COLOR, &DARK_COLOR, sizeof(DARK_COLOR)));

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

    }

    //Create window after registering class
    HWND igLoginhWnd = CreateWindowW((LPCWSTR)IGLoginClassName, L"Instagram", WS_OVERLAPPEDWINDOW, 400, 200, 400, 300, parent, NULL, hInst_ig, NULL);
    ShowWindow(igLoginhWnd, nCmdShow);
    COLORREF DARK_COLOR = 0x00505050;
    BOOL SET_CAPTION_COLOR = SUCCEEDED(DwmSetWindowAttribute(igLoginhWnd, DWMWA_CAPTION_COLOR, &DARK_COLOR, sizeof(DARK_COLOR)));

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
    }

    //Create window after registering class
    HWND twLoginhWnd = CreateWindowW((LPCWSTR)TWLoginClassName, L"Twitter", WS_OVERLAPPEDWINDOW, 400, 200, 400, 300, parent, NULL, hInst_tw, NULL);
    ShowWindow(twLoginhWnd, nCmdShow);
    COLORREF DARK_COLOR = 0x00505050;
    BOOL SET_CAPTION_COLOR = SUCCEEDED(DwmSetWindowAttribute(twLoginhWnd, DWMWA_CAPTION_COLOR, &DARK_COLOR, sizeof(DARK_COLOR)));

}

//loads all the images used by the GUI from the GUI_IMAGES folder
void loadImages()
{
    
    backgroundImg = (HBITMAP)LoadImageW(NULL, L".\\GUI_IMAGES\\nodes.bmp", IMAGE_BITMAP, 1300, 900, LR_LOADFROMFILE);
    logoImg = (HBITMAP)LoadImageW(NULL, L".\\GUI_IMAGES\\logo.bmp", IMAGE_BITMAP, 125, 125, LR_LOADFROMFILE);
    redButtonImg = (HBITMAP)LoadImageW(NULL, L".\\GUI_IMAGES\\red_button.bmp", IMAGE_BITMAP, 100, 60, LR_LOADFROMFILE);
    greenButtonImg = (HBITMAP)LoadImageW(NULL, L".\\GUI_IMAGES\\green_button.bmp", IMAGE_BITMAP, 100, 60, LR_LOADFROMFILE);
    scanningButtonImg = (HBITMAP)LoadImageW(NULL, L".\\GUI_IMAGES\\running_button.bmp", IMAGE_BITMAP, 100,60,LR_LOADFROMFILE);
    fbSectionImg = (HBITMAP)LoadImageW(NULL, L".\\GUI_IMAGES\\facebook_section_img_silver.bmp", IMAGE_BITMAP, 300, 500, LR_LOADFROMFILE);
    igSectionImg = (HBITMAP)LoadImageW(NULL, L".\\GUI_IMAGES\\instagram_section_img_silver.bmp", IMAGE_BITMAP, 300, 500, LR_LOADFROMFILE);
    twSectionImg = (HBITMAP)LoadImageW(NULL, L".\\GUI_IMAGES\\twitter_section_img_silver.bmp", IMAGE_BITMAP, 300, 500, LR_LOADFROMFILE);
    notLoggedInButton = (HBITMAP)LoadImageW(NULL, L".\\GUI_IMAGES\\not_logged_in_button.bmp", IMAGE_BITMAP, 55, 30, LR_LOADFROMFILE);
    loggedInButton = (HBITMAP)LoadImageW(NULL, L".\\GUI_IMAGES\\logged_in_button.bmp", IMAGE_BITMAP, 55, 30, LR_LOADFROMFILE);
    runningButton = (HBITMAP)LoadImageW(NULL, L".\\GUI_IMAGES\\running_button_logins.bmp", IMAGE_BITMAP,55, 30, LR_LOADFROMFILE);
    reAuthButton = (HBITMAP)LoadImageW(NULL, L".\\GUI_IMAGES\\reauth_button.bmp", IMAGE_BITMAP, 100, 60, LR_LOADFROMFILE);
}
//Helper function to enumerate over child windows and set the font to 'font'
bool CALLBACK SetFont(HWND child, LPARAM font) {
    SendMessage(child, WM_SETFONT, font, true);
    return true;
}

//Main Window Procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{    
    wchar_t hOut[100], mOut[100], sOut[100], limitOut[20];
    static const int points_per_inch = 72;
    int points, pixels_per_inch, pixels_height;
    HFONT font;
    HDC hDC;
    std::fstream readOutputLog;
    int i, savedUsernameLen, savedPasswordLen, char_val;
    char usernameSavedClearText[100], passwordSavedClearText[100];
    std::string usernameSaved_enc, passwordSaved_enc, usernameClearText, passwordClearText, line;
    std::ofstream ofs;
    switch (message)
    {
    case WM_CREATE:
        loadImages();
        AddMenu(hWnd);
        AddControls(hWnd);
        points = 10;
        hDC = GetDC(0);
        pixels_per_inch = GetDeviceCaps(hDC, LOGPIXELSY);
        pixels_height = -(points * pixels_per_inch / points_per_inch);
        font = CreateFontA(pixels_height, 0, 0, 0, 500, false, false, false, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH | FF_SWISS, "Century Gothic");
        SendMessage(hWnd, WM_SETFONT, (WPARAM)font, TRUE);
        EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)font);
        break;
    case WM_TIMER:
        
        if (COUNT > 0) 
        {
            hours = COUNT / 3600;
            minutes = (COUNT / 60) % 60;
            seconds = COUNT % 60;
            _itow_s(hours, hOut, 10);
            _itow_s(minutes, mOut, 10);
            _itow_s(seconds, sOut, 10);
            SetWindowTextW(HOURS, hOut);
            SetWindowTextW(MINUTES, mOut);
            SetWindowTextW(SECONDS, sOut);
        }
        if ((!STOP_SCANNING) && (PREV_SCAN_COMPLETE))
        {
            COUNT--;
        }
        if (COUNT <= 0) 
        {     
            ScanCount++;
            //Update Scan Count
            _itow_s(ScanCount, limitOut, 10);
            SetWindowTextW(scanCountDisplay, limitOut);     
            SendMessageW(launchButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)scanningButtonImg);
            lauchScanners(FB_LOGGED_IN, IG_LOGGED_IN, TW_LOGGED_IN);
            beginListeningforScrapeResults(FB_LOGGED_IN, IG_LOGGED_IN, TW_LOGGED_IN);
            COUNT = RESET_COUNT;
            
        }
       
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections
        switch (wmId)
        {
            //Open FB results in file explorer
        case EXPORT_FB:
            if (!openFileExplorer(RESULTS_FB)) {
                MessageBox(hWnd, L"Error opening results", L"Window Class Failed", MB_ICONERROR);
            }
            break;
            //Open IG results in file explorer
        case EXPORT_IG:
            if (!openFileExplorer(RESULTS_IG)) {
                MessageBox(hWnd, L"Error opening results", L"Window Class Failed", MB_ICONERROR);
            }
            break;
            //Open TW results in file explorer
        case EXPORT_TW:
            if (!openFileExplorer(RESULTS_TW)) {
                MessageBox(hWnd, L"Error opening results", L"Window Class Failed", MB_ICONERROR);
            }
            break;
        case OPEN_FLAGGED_FB:
            if (!openFileExplorer(FLAGGED_FB)) {
                MessageBox(hWnd, L"Error opening results", L"Window Class Failed", MB_ICONERROR);
            }
            break;
            //Open IG results in file explorer
        case OPEN_FLAGGED_IG:
            if (!openFileExplorer(FLAGGED_IG)) {
                MessageBox(hWnd, L"Error opening results", L"Window Class Failed", MB_ICONERROR);
            }
            break;
            //Open TW results in file explorer
        case OPEN_FLAGGED_TW:
            if (!openFileExplorer(FLAGGED_TW)) {
                MessageBox(hWnd, L"Error opening results", L"Window Class Failed", MB_ICONERROR);
            }
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
        case TWITTER_LOGIN:
            WNDCLASSEXW twLoginWindow;
            hInstanceTWLogin = (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE);
            createTwitterLoginWindow(twLoginWindow, hInstanceTWLogin, SW_SHOW, hWnd);
            break;
        case LAUNCH: 
            ScanCount = 0;
            if (CONFIG_SET && MIN_ONE_SITE_LOGGED_IN) 
            {
                InitializeTimer();
                COUNT = 0;
                SendMessageW(launchButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)scanningButtonImg);
                if (FB_LOGGED_IN) {
                    SendMessageW(facebookLoginButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)runningButton);
                }
                if (IG_LOGGED_IN) {
                    SendMessageW(instagramLoginButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)runningButton);
                }
                if (TW_LOGGED_IN) {
                    SendMessageW(twitterLoginButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)runningButton);
                }
                //lauchScanners(FB_LOGGED_IN, IG_LOGGED_IN, TW_LOGGED_IN);
                //beginListeningforScrapeResults(FB_LOGGED_IN, IG_LOGGED_IN, TW_LOGGED_IN);
                //FIRST_SCAN = false;
            }
            else
            {
                MessageBox(hWnd, L"Must:\n\nComplete Configuration\nBe logged into at least one site\n\nBefore Launching Scan", NULL, MB_ICONERROR | MB_OK);
            }
            break;
        case PAUSE_SCAN:
            STOP_SCANNING = true;
            break;
        case RESUME_SCAN:
            STOP_SCANNING = false;
            break;
        //Launch Configuration Panel
        case CONFIG_PANEL:
            //Reset the default output directory if configuration has been opened before
            if (hInstanceConfig != NULL)
            {
                SetWindowTextW(outputDir, L"Default: .\\Program Data\\FoundPosts\\");
            }
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
    int hoursVal, minutesVal, secondsVal, i, RegionItemIndex;
    wchar_t hOut[100], mOut[100], sOut[100];
    wchar_t checkDefault[] = L"Default";
    wchar_t indexItemOut[2];
    static const int points_per_inch = 72;
    int points, pixels_per_inch, pixels_height;
    HFONT font;
    HDC hDC;
    switch (message)
    {
    case WM_CREATE:
        AddConfigControls(hWnd);
        points = 10;
        hDC = GetDC(0);
        pixels_per_inch = GetDeviceCaps(hDC, LOGPIXELSY);
        pixels_height = -(points * pixels_per_inch / points_per_inch);
        font = CreateFontA(pixels_height, 0, 0, 0, 500, false, false, false, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH | FF_SWISS, "Century Gothic");
        SendMessage(hWnd, WM_SETFONT, (WPARAM)font, TRUE);
        EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)font);
        break;
    case WM_COMMAND:
        switch (wmId)
        {
        case OPEN_KEYWORDS:
            if (!openFileExplorer(WORDLIST))
            {
                MessageBox(hWnd, L"Error opening results", L"Window Class Failed", MB_ICONERROR);
            }
            break;

        case SUBMIT_CONFIG:
            //Grab values from config panel, set to default to show user something happened
            GetWindowTextW(ScanLimit, scanLimitCaptured, 100);
            SetWindowTextW(ScanLimit, L"0");
            GetWindowTextW(outputDir, outputDirCaptured, 100);
            //Check if first characters are 'Default'
            NOT_DEFAULT_OUTPUT_DIR = false;
            for (i = 0; i < 7; i++)
            {
                if (checkDefault[i] != outputDirCaptured[i]) {
                    NOT_DEFAULT_OUTPUT_DIR = true;
                }
            }
           
            SetWindowTextW(outputDir, L"");
                      
            GetWindowTextW(H, hoursCaptured, 100);
            SetWindowTextW(H, L"00");

            GetWindowTextW(M, minutesCaptured, 20);
            SetWindowTextW(M, L"00");

            GetWindowTextW(S, secondsCaptured, 20);
            SetWindowTextW(S, L"00");

            //Grab cursor index for region dropdown list, 
            RegionItemIndex = SendMessageW(Region, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0); 
            REGION_SELECTION = REGIONS[RegionItemIndex];
            //REGION_SELECTION = (char)(RegionItemIndex + 1);


            // Compute and store COUNT and RESET COUNT
            hoursVal = _wtoi(hoursCaptured);
            minutesVal = _wtoi(minutesCaptured);
            secondsVal = _wtoi(secondsCaptured);
            if ((hoursVal > 200 or hoursVal < 0) or (minutesVal > 60 or minutesVal < 0) or (secondsVal > 60 or secondsVal < 0) or (hoursVal == 0 and minutesVal == 0 and secondsVal == 0)) {
                MessageBox(NULL, L"Enter valid time interval: \n0-200 Hours 0-60 Minutes and 0-60 Seconds", L"Input Error", MB_ICONERROR);
                hoursVal = 0;
                minutesVal = 0;
                secondsVal = 0;
            }

            COUNT = ((hoursVal * 3600) + (minutesVal * 60) + secondsVal);
            RESET_COUNT = COUNT;
            //Show captured values in main window
            SetWindowTextW(HOURS, hoursCaptured);
            SetWindowTextW(MINUTES, minutesCaptured);
            SetWindowTextW(SECONDS, secondsCaptured);
            
            //Convert and display scan limit
            ScanLimitInt = _wtoi(scanLimitCaptured);
            if (ScanLimitInt == 0) {
                //No limit if no value set
                ScanLimitInt = NULL;
            }

            //IN FUTURE, CHECK VALUE INTEGRITY BEFORE SETTING CONFIG_SET TO TRUE
            //CONFIG_SET = true;
            if (COUNT > 0) {
                DestroyWindow(hWnd);
                CONFIG_SET = true;
            }
            //DestroyWindow(hWnd);
            if (CONFIG_SET && MIN_ONE_SITE_LOGGED_IN) {
                SendMessageW(launchButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)greenButtonImg);
            }
            break;
            
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
    int state, i, j, k, char_val, savedUsernameLen, savedPasswordLen;
    int wmId = LOWORD(wParam);
    std::string usernameSaved_enc, passwordSaved_enc, usernameClearText, passwordClearText;
    char c;
    char usernameSavedClearText[100], passwordSavedClearText[100];
    std::string line;
    std::fstream readOutputLog;
    static const int points_per_inch = 72;
    int points, pixels_per_inch, pixels_height;
    HFONT font;
    HDC hDC;
    switch (message)
    {
    case WM_CREATE:
        AddFBLoginControls(hWnd);
        points = 10;
        hDC = GetDC(0);
        pixels_per_inch = GetDeviceCaps(hDC, LOGPIXELSY);
        pixels_height = -(points * pixels_per_inch / points_per_inch);
        font = CreateFontA(pixels_height, 0, 0, 0, 500, false, false, false, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH | FF_SWISS, "Century Gothic");
        SendMessage(hWnd, WM_SETFONT, (WPARAM)font, TRUE);
        EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)font);
        break;
    case WM_COMMAND:
        switch (wmId)
        {
        case FACEBOOK_LOGIN_SUBMIT:
        {
            FB_AUTH_SUCCESS = false;
            FB_AUTH_FINISHED = false;
            FB_LOGGED_IN = false;
            shellOperation = "";
            shellOperation.append("python3 authenticator.py FB ");
            //Grab values from the username and password fields, store in global variables
            GetWindowTextW(fbUser, fbUsername, 100);
            GetWindowTextW(fbPass, fbPassword, 100);
            SetWindowTextW(fbUser, L"");
            SetWindowTextW(fbPass, L"");
            state = Button_GetCheck(checkboxFB);
            if (state == BST_CHECKED)
            {
                SAVED_CONFIG_FILE.open(".\\Program Data\\Configuration\\user_config.txt", std::ios::in);
                //Read in all lines, fill in string array of elements
                if (SAVED_CONFIG_FILE.is_open())
                {
                    for (i = 0; i < 9; i++)
                    {
                        std::getline(SAVED_CONFIG_FILE, line);
                        SAVED_CONFIG_ELEMENTS[i] = line;
                    }
                }
                //Decrpyt the usernames and passwords (-10 to each character value)
                usernameSaved_enc = SAVED_CONFIG_ELEMENTS[1];
                passwordSaved_enc = SAVED_CONFIG_ELEMENTS[2];

                savedUsernameLen = strlen(usernameSaved_enc.c_str());
                savedPasswordLen = strlen(passwordSaved_enc.c_str());

                for (i = 0; i < savedUsernameLen; i++) {
                    char_val = int(usernameSaved_enc[i]) - 10;
                    usernameSavedClearText[i] = (char)char_val;
                }
                usernameSavedClearText[i] = '\0';
                for (i = 0; i < savedPasswordLen; i++) {
                    char_val = int(passwordSaved_enc[i]) - 10;
                    passwordSavedClearText[i] = (char)char_val;
                }
                passwordSavedClearText[i] = '\0';

                //Use saved credentials to post login request
                usernameClearText = usernameSavedClearText;
                passwordClearText = passwordSavedClearText;
                shellOperation.append(usernameClearText);
                shellOperation.append(" ");
                shellOperation.append(passwordClearText);
                shellOperation.append(" ");

                //Lauch program
                WinExec((LPCSTR)shellOperation.c_str(), SW_SHOW);
                SetWindowTextW(FBsubmit, L"Authenticating...");
            }
            //NOTE: if user puts stuff in the login but also checks used saved, will override and use saved info
            if (state != BST_CHECKED)
            {
                //USE igUsername and igPassword
                std::wstring wideUsername(fbUsername);
                std::wstring widePassword(fbPassword);
                std::string correctedUsername(wideUsername.begin(), wideUsername.end());
                std::string correctedPassword(widePassword.begin(), widePassword.end());
                shellOperation = "";
                shellOperation.append("python3 authenticator.py FB ");
                shellOperation.append(correctedUsername);
                shellOperation.append(" ");
                shellOperation.append(correctedPassword);
                shellOperation.append(" ");

                //Lauch program
                WinExec((LPCSTR)shellOperation.c_str(), SW_SHOW);
                SetWindowTextW(FBsubmit, L"Authenticating...");
            }

            FB_AUTH_SUCCESS = false;
            //Wait three seconds then see if the logs show a success or a fail
            while (!FB_AUTH_FINISHED)
            {
                Sleep(3000);
                readOutputLog.open(".\\Program Data\\Logs\\FB_AUTH_LOGS\\log.txt", std::ios::in);
                //Read in all lines, check for SUCCESS or FAIL
                if (readOutputLog.is_open())
                {
                    while (std::getline(readOutputLog, line))
                    {
                        if (strcmp(line.c_str(), "SUCCESS") == 0) {
                            FB_AUTH_SUCCESS = true;
                            FB_AUTH_FINISHED = true;
                        }
                        if (strcmp(line.c_str(), "FAIL") == 0) {
                            MessageBox(hWnd, L"Login Attempt Failed", L"Error on Login", MB_ICONERROR);
                            SetWindowTextW(FBsubmit, L"SUBMIT");
                            FB_AUTH_FINISHED = true;
                        }
                    }

                    if (FB_AUTH_SUCCESS)
                    {
                        SendMessageW(facebookLoginButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)loggedInButton);
                        MIN_ONE_SITE_LOGGED_IN = true;
                        FB_LOGGED_IN = true;
                        DestroyWindow(hWnd);
                    }
                    readOutputLog.close();
                }
            }
            if (CONFIG_SET && MIN_ONE_SITE_LOGGED_IN) {
                SendMessageW(launchButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)greenButtonImg);
            }
            break;
        }
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
    int state, i,j,k, char_val, savedUsernameLen, savedPasswordLen;
    int wmId = LOWORD(wParam);
    std::string usernameSaved_enc, passwordSaved_enc, usernameClearText, passwordClearText;
    char c;
    char usernameSavedClearText[50], passwordSavedClearText[50];
    std::string line;
    std::fstream readOutputLog;
    static const int points_per_inch = 72;
    int points, pixels_per_inch, pixels_height;
    HFONT font;
    HDC hDC;
    switch (message)
    {
    case WM_CREATE:
        AddIGLoginControls(hWnd);
        points = 10;
        hDC = GetDC(0);
        pixels_per_inch = GetDeviceCaps(hDC, LOGPIXELSY);
        pixels_height = -(points * pixels_per_inch / points_per_inch);
        font = CreateFontA(pixels_height, 0, 0, 0, 500, false, false, false, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH | FF_SWISS, "Century Gothic");
        SendMessage(hWnd, WM_SETFONT, (WPARAM)font, TRUE);
        EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)font);
        break;
    case WM_COMMAND:
        switch (wmId)
        {
        case INSTAGRAM_LOGIN_SUBMIT:
        {
            IG_AUTH_SUCCESS = false;
            IG_AUTH_FINISHED = false;
            IG_LOGGED_IN = false;
            shellOperation = "";
            shellOperation.append("python3 authenticator.py IG ");
            //Grab values from the username and password fields, store in global variables
            GetWindowTextW(igUser, igUsername, 100);
            GetWindowTextW(igPass, igPassword, 100);
            SetWindowTextW(igUser, L"");
            SetWindowTextW(igPass, L"");
            state = Button_GetCheck(checkboxIG);
            if (state == BST_CHECKED)
            {
                SAVED_CONFIG_FILE.open(".\\Program Data\\Configuration\\user_config.txt", std::ios::in);
                //Read in all lines, fill in string array of elements
                if (SAVED_CONFIG_FILE.is_open())
                {
                    for (i = 0; i < 9; i++)
                    {
                        std::getline(SAVED_CONFIG_FILE, line);
                        SAVED_CONFIG_ELEMENTS[i] = line;
                    }
                }
                //Decrpyt the usernames and passwords (-10 to each character value)
                usernameSaved_enc = SAVED_CONFIG_ELEMENTS[4];
                passwordSaved_enc = SAVED_CONFIG_ELEMENTS[5];

                savedUsernameLen = strlen(usernameSaved_enc.c_str());
                savedPasswordLen = strlen(passwordSaved_enc.c_str());

                for (i = 0; i < savedUsernameLen; i++) {
                    char_val = int(usernameSaved_enc[i]) - 10;
                    usernameSavedClearText[i] = (char)char_val;
                }
                usernameSavedClearText[i] = '\0';
                for (i = 0; i < savedPasswordLen; i++) {
                    char_val = int(passwordSaved_enc[i]) - 10;
                    passwordSavedClearText[i] = (char)char_val;
                }
                passwordSavedClearText[i] = '\0';

                //Use saved credentials to post login request
                usernameClearText = usernameSavedClearText;
                passwordClearText = passwordSavedClearText;
                shellOperation.append(usernameClearText);
                shellOperation.append(" ");
                shellOperation.append(passwordClearText);
                shellOperation.append(" ");

                //Lauch program
                WinExec((LPCSTR)shellOperation.c_str(), SW_HIDE);
                SetWindowTextW(IGsubmit, L"Authenticating...");
            }
            //NOTE: if user puts stuff in the login but also checks used saved, will override and use saved info
            if (state != BST_CHECKED)
            {
                //USE igUsername and igPassword
                std::wstring wideUsername(igUsername);
                std::wstring widePassword(igPassword);
                std::string correctedUsername(wideUsername.begin(), wideUsername.end());
                std::string correctedPassword(widePassword.begin(), widePassword.end());
                shellOperation = "";
                shellOperation.append("python3 authenticator.py IG ");
                shellOperation.append(correctedUsername);
                shellOperation.append(" ");
                shellOperation.append(correctedPassword);
                shellOperation.append(" ");

                //Lauch program
                WinExec((LPCSTR)shellOperation.c_str(), SW_HIDE);
                SetWindowTextW(IGsubmit, L"Authenticating...");
            }

            //Wait three seconds then see if the logs show a success or a fail
            while (!IG_AUTH_FINISHED)
            {
                Sleep(3000);
                readOutputLog.open(".\\Program Data\\Logs\\IG_AUTH_LOGS\\log.txt", std::ios::in);
                //Read in all lines, check for SUCCESS or FAIL
                if (readOutputLog.is_open())
                {
                    while (std::getline(readOutputLog, line))
                    {
                        if (strcmp(line.c_str(), "SUCCESS") == 0) {
                            IG_AUTH_SUCCESS = true;
                            IG_AUTH_FINISHED = true;
                        }
                        if (strcmp(line.c_str(), "FAIL") == 0) {
                            MessageBox(hWnd, L"Login Attempt Failed", L"Error on Login", MB_ICONERROR);
                            SetWindowTextW(IGsubmit, L"SUBMIT");
                            IG_AUTH_FINISHED = true;
                        }
                    }

                    if (IG_AUTH_SUCCESS)
                    {
                        SendMessageW(instagramLoginButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)loggedInButton);
                        MIN_ONE_SITE_LOGGED_IN = true;
                        IG_LOGGED_IN = true;
                        DestroyWindow(hWnd);
                    }
                    readOutputLog.close();
                }
            }
            if (CONFIG_SET && MIN_ONE_SITE_LOGGED_IN) {
                SendMessageW(launchButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)greenButtonImg);
            }
            break;
        }
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
    int state, i, j, k, char_val, savedUsernameLen, savedPasswordLen;
    int wmId = LOWORD(wParam);
    std::string usernameSaved_enc, passwordSaved_enc, usernameClearText, passwordClearText;
    char c;
    char usernameSavedClearText[50], passwordSavedClearText[50];
    std::string line;
    std::fstream readOutputLog;
    static const int points_per_inch = 72;
    int points, pixels_per_inch, pixels_height;
    HFONT font;
    HDC hDC;
    switch (message)
    {
    case WM_CREATE:
        AddTWLoginControls(hWnd);
        points = 10;
        hDC = GetDC(0);
        pixels_per_inch = GetDeviceCaps(hDC, LOGPIXELSY);
        pixels_height = -(points * pixels_per_inch / points_per_inch);
        font = CreateFontA(pixels_height, 0, 0, 0, 500, false, false, false, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH | FF_SWISS, "Century Gothic");
        SendMessage(hWnd, WM_SETFONT, (WPARAM)font, TRUE);
        EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)font);
        break;
    case WM_COMMAND:
        switch (wmId)
        {
        case TWITTER_LOGIN_SUBMIT:
        {
            TW_AUTH_SUCCESS = false;
            TW_AUTH_FINISHED = false;
            TW_LOGGED_IN = false;
            shellOperation = "";
            shellOperation.append("python3 authenticator.py TW ");
            //Grab values from the username and password fields, store in global variables
            GetWindowTextW(twUser, twUsername, 100);
            GetWindowTextW(twPass, twPassword, 100);
            SetWindowTextW(twUser, L"");
            SetWindowTextW(twPass, L"");
            state = Button_GetCheck(checkboxTW);
            if (state == BST_CHECKED)
            {
                SAVED_CONFIG_FILE.open(".\\Program Data\\Configuration\\user_config.txt", std::ios::in);
                //Read in all lines, fill in string array of elements
                if (SAVED_CONFIG_FILE.is_open())
                {
                    for (i = 0; i < 9; i++)
                    {
                        std::getline(SAVED_CONFIG_FILE, line);
                        SAVED_CONFIG_ELEMENTS[i] = line;
                    }
                }
                //Decrpyt the usernames and passwords (-10 to each character value)
                usernameSaved_enc = SAVED_CONFIG_ELEMENTS[7];
                passwordSaved_enc = SAVED_CONFIG_ELEMENTS[8];

                savedUsernameLen = strlen(usernameSaved_enc.c_str());
                savedPasswordLen = strlen(passwordSaved_enc.c_str());

                for (i = 0; i < savedUsernameLen; i++) {
                    char_val = int(usernameSaved_enc[i]) - 10;
                    usernameSavedClearText[i] = (char)char_val;
                }
                usernameSavedClearText[i] = '\0';
                for (i = 0; i < savedPasswordLen; i++) {
                    char_val = int(passwordSaved_enc[i]) - 10;
                    passwordSavedClearText[i] = (char)char_val;
                }
                passwordSavedClearText[i] = '\0';

                //Use saved credentials to post login request
                usernameClearText = usernameSavedClearText;
                passwordClearText = passwordSavedClearText;
                shellOperation.append(usernameClearText);
                shellOperation.append(" ");
                shellOperation.append(passwordClearText);
                shellOperation.append(" ");

                //Lauch program
                WinExec((LPCSTR)shellOperation.c_str(), SW_HIDE);
                SetWindowTextW(TWsubmit, L"Authenticating...");
            }
            //NOTE: if user puts stuff in the login but also checks used saved, will override and use saved info
            if (state != BST_CHECKED)
            {
                //USE igUsername and igPassword
                std::wstring wideUsername(twUsername);
                std::wstring widePassword(twPassword);
                std::string correctedUsername(wideUsername.begin(), wideUsername.end());
                std::string correctedPassword(widePassword.begin(), widePassword.end());
                shellOperation = "";
                shellOperation.append("python3 authenticator.py TW ");
                shellOperation.append(correctedUsername);
                shellOperation.append(" ");
                shellOperation.append(correctedPassword);
                shellOperation.append(" ");

                //Lauch program
                WinExec((LPCSTR)shellOperation.c_str(), SW_HIDE);
                SetWindowTextW(TWsubmit, L"Authenticating...");
            }

            //Wait three seconds then see if the logs show a success or a fail
            while (!TW_AUTH_FINISHED)
            {
                Sleep(3000);
                readOutputLog.open(".\\Program Data\\Logs\\TW_AUTH_LOGS\\log.txt", std::ios::in);
                //Read in all lines, check for SUCCESS or FAIL
                if (readOutputLog.is_open())
                {
                    while (std::getline(readOutputLog, line))
                    {
                        if (strcmp(line.c_str(), "SUCCESS") == 0) {
                            TW_AUTH_SUCCESS = true;
                            TW_AUTH_FINISHED = true;
                        }
                        if (strcmp(line.c_str(), "FAIL") == 0) {
                            MessageBox(hWnd, L"Login Attempt Failed", L"Error on Login", MB_ICONERROR);
                            SetWindowTextW(TWsubmit, L"SUBMIT");
                            TW_AUTH_FINISHED = true;
                        }
                    }

                    if (TW_AUTH_SUCCESS)
                    {
                        SendMessageW(twitterLoginButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)loggedInButton);
                        MIN_ONE_SITE_LOGGED_IN = true;
                        TW_LOGGED_IN = true;
                        DestroyWindow(hWnd);
                    }
                    readOutputLog.close();
                }
            }
            if (CONFIG_SET && MIN_ONE_SITE_LOGGED_IN) {
                SendMessageW(launchButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)greenButtonImg);
            }
            break;
        }
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

    HMENU menuMainSubFile = CreateMenu();
    HMENU menuMainSubHelp = CreateMenu();
   

    // Populate drop-down menus
    AppendMenu(menuMainSubFile, MF_STRING, IDM_EXIT, L"Exit");
    //Populate primary menu items
    AppendMenu(menuMain, MF_POPUP, (UINT_PTR)menuMainSubFile, L"File");
    AppendMenu(menuMain, MF_STRING, FILE_MENU_ABOUT, L"About");
    SetMenu(hWnd, menuMain);
}

//Adds features to the main window
void AddControls(HWND hWnd)
{
    HWND backgroundTile = CreateWindowW(L"Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, 0, 0, 1200, 800, hWnd, NULL, NULL, NULL);
    SendMessageW(backgroundTile, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)backgroundImg);
    HWND logo = CreateWindowW(L"Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, 1035,650, 100, 100, hWnd, NULL, NULL, NULL);
    SendMessageW(logo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)logoImg);

    //Create boxes to fill with Timer values, store h,m, and s in global variables for re-painting on WM_TIMER
    HWND timeTillNextScanBox = CreateWindowW(L"Static", L"Time Until Next Scan:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 406, 50, 140, 20, hWnd, NULL, NULL, NULL);
    HWND timerBox = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 549, 49, 97, 22, hWnd, NULL, NULL, NULL);
    HOURS = CreateWindowW(L"Edit", L"00", WS_VISIBLE | WS_CHILD | SS_CENTER, 550, 50, 25, 20, hWnd, NULL, NULL, NULL);
    HWND space_1 = CreateWindowW(L"Static", L":", WS_VISIBLE | WS_CHILD | SS_CENTER, 575, 50, 10, 20, hWnd, NULL, NULL, NULL);
    MINUTES = CreateWindowW(L"Edit", L"00", WS_VISIBLE | WS_CHILD | SS_CENTER, 585, 50, 25, 20, hWnd, NULL, NULL, NULL);
    HWND space_2 = CreateWindowW(L"Static", L":", WS_VISIBLE | WS_CHILD | SS_CENTER, 610, 50, 10, 20, hWnd, NULL, NULL, NULL);
    SECONDS = CreateWindowW(L"Edit", L"00", WS_VISIBLE | WS_CHILD | SS_CENTER, 620, 50, 25, 20, hWnd, NULL, NULL, NULL);


    HWND openConfigButton = CreateWindowW(L"Button", L"Configuration", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON |BS_CENTER, 670, 45, 110, 30, hWnd, (HMENU)CONFIG_PANEL, NULL, NULL);
    HWND stopScanningButton = CreateWindow(L"Button", L"Pause Scan", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_CENTER, 670, 80, 100, 30, hWnd, (HMENU)PAUSE_SCAN, NULL, NULL);
    HWND resumeScanningButton = CreateWindowW(L"Button", L"Resume Scan", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_CENTER, 780, 80, 100, 30, hWnd, (HMENU)RESUME_SCAN, NULL, NULL);
    //Add Scan count and message
    HWND scanCountText = CreateWindowW(L"Static", L"Scan Count:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 460, 75, 85, 20, hWnd, NULL, NULL, NULL);
    scanCountDisplay = CreateWindowW(L"Edit", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 550, 75, 25, 20, hWnd, NULL, NULL, NULL);


    //Main three blocks
    facebookSection = CreateWindowW(L"Static", L"Facebook", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_BITMAP, 100, 130, 300, 500, hWnd, NULL, NULL, NULL);
    instagramSection = CreateWindowW(L"Static", L"Instagram", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_BITMAP, 450, 130, 300, 500, hWnd, NULL, NULL, NULL);
    twitterSection = CreateWindowW(L"Static", L"Twitter", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_BITMAP, 800, 130, 300, 500, hWnd, NULL, NULL, NULL, NULL);
    SendMessageW(facebookSection, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)fbSectionImg);
    SendMessageW(instagramSection, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)igSectionImg);
    SendMessageW(twitterSection, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)twSectionImg);

    
    

    //Populate login buttons
    facebookLoginButton = CreateWindowW(L"Button", L"LOGIN", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON |BS_BITMAP, 225, 205, 55, 30, hWnd, (HMENU)FACEBOOK_LOGIN, NULL, NULL);
    instagramLoginButton = CreateWindowW(L"Button", L"LOGIN", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON |BS_BITMAP, 575, 205, 55, 30, hWnd, (HMENU)INSTAGRAM_LOGIN, NULL, NULL);
    twitterLoginButton = CreateWindowW(L"Button", L"LOGIN", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON |BS_BITMAP, 925, 205, 55, 30, hWnd, (HMENU)TWITTER_LOGIN, NULL, NULL);
    SendMessageW(facebookLoginButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)notLoggedInButton);
    SendMessageW(instagramLoginButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)notLoggedInButton);
    SendMessageW(twitterLoginButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)notLoggedInButton);


    facebookResultsSummary = CreateWindowW(L"Edit", L"Scan Summary:", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 150, 250, 200, 250, hWnd, NULL, NULL, NULL);
    instagramResultsSummary = CreateWindowW(L"Edit", L"Scan Summary:", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 500, 250, 200, 250, hWnd, NULL, NULL, NULL);
    twitterResultsSummary = CreateWindowW(L"Edit", L"Scan Summary:", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 850, 250, 200, 250, hWnd, NULL, NULL, NULL);

    //Populate flagged users buttons
    HWND facebookFlaggedUserButton = CreateWindowW(L"Button", L"Flagged Users", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 290, 130, 100, 30, hWnd, (HMENU)OPEN_FLAGGED_FB, NULL, NULL);
    HWND instagramFlaggedUserButton = CreateWindowW(L"Button", L"Flagged Users", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 640, 130, 100, 30, hWnd, (HMENU)OPEN_FLAGGED_IG, NULL, NULL);
    HWND twitterFlaggedUserButton = CreateWindowW(L"Button", L"Flagged Users", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 990, 130, 100, 30, hWnd, (HMENU)OPEN_FLAGGED_TW, NULL, NULL);

    //Add Export Results Buttons
    HWND facebookExportButton = CreateWindowW(L"Button", L"Export Full Scan Results", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE, 290, 560, 100, 60, hWnd, (HMENU)EXPORT_FB, NULL, NULL);
    HWND instagramExportButton = CreateWindowW(L"Button", L"Export Full Scan Results", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE, 640, 560, 100, 60, hWnd, (HMENU)EXPORT_IG, NULL, NULL);
    HWND twitterExportButton = CreateWindowW(L"Button", L"Export Full Scan Results", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE, 990, 560, 100, 60, hWnd, (HMENU)EXPORT_TW, NULL, NULL);

    launchButton = CreateWindowW(L"Button", L"LAUNCH SCAN", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON |BS_CENTER |BS_BITMAP, 550, 655, 100, 60, hWnd, (HMENU)LAUNCH, NULL, NULL);
    SendMessageW(launchButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)redButtonImg);
}

//Add elements to the configuration window
void AddConfigControls(HWND hWnd)
{
   //Scan frequency boxes
    HWND setFrequencyText = CreateWindowW(L"Static", L"Scan Frequency:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 20, 120, 20, hWnd, NULL, NULL, NULL);
    H = CreateWindowW(L"Edit", L"00", WS_VISIBLE | WS_CHILD | SS_CENTER |WS_BORDER |ES_AUTOHSCROLL, 145, 20, 25, 20, hWnd, NULL, NULL, NULL);
    HWND space_1 = CreateWindowW(L"Static", L"h", WS_VISIBLE | WS_CHILD | SS_CENTER, 170, 20, 10, 20, hWnd, NULL, NULL, NULL);
    M = CreateWindowW(L"Edit", L"00", WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER, 185, 20, 25, 20, hWnd, NULL, NULL, NULL);
    HWND space_2 = CreateWindowW(L"Static", L"m", WS_VISIBLE | WS_CHILD | SS_CENTER, 215, 20, 10, 20, hWnd, NULL, NULL, NULL);
    S = CreateWindowW(L"Edit", L"00", WS_VISIBLE | WS_CHILD | SS_CENTER |WS_BORDER, 230, 20, 25, 20, hWnd, NULL, NULL, NULL);
    HWND space_3 = CreateWindowW(L"Static", L"s", WS_VISIBLE | WS_CHILD | SS_CENTER, 260, 20, 10, 20, hWnd, NULL, NULL, NULL);
    
    //Scan limiter
    HWND setScanLimitText = CreateWindowW(L"Static", L"Set Scan Limit:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 50, 120, 20, hWnd, NULL, NULL, NULL);
    ScanLimit = CreateWindowW(L"Edit", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 145, 50, 30, 20, hWnd, NULL, NULL, NULL);

    //Open keywords folder button
    HWND openKeyWordsDir = CreateWindowW(L"Button", L"Open Keywords", WS_VISIBLE | BS_DEFPUSHBUTTON | WS_CHILD | SS_LEFT, 20, 80, 120, 60, hWnd, (HMENU)OPEN_KEYWORDS, NULL, NULL);
    
    //Select region from dropdown menu WORK IN PROGRESS
   
    HWND setRegionText = CreateWindowW(L"Static", L"Select Region:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 150, 120, 20, hWnd, NULL, NULL, NULL);
    
    Region = CreateWindowW(L"combobox", L"Region", WS_VISIBLE | WS_BORDER | WS_CHILD |CBS_DROPDOWN, 145, 150, 200, 200, hWnd, NULL, NULL, NULL);
    
    //Populate the dropdown list
    SendMessageW(Region, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"Alaska");
    SendMessageW(Region, CB_ADDSTRING, (WPARAM)1, (LPARAM)L"Anchorage");
    SendMessageW(Region, CB_ADDSTRING, (WPARAM)2, (LPARAM)L"Juneau");
    SendMessageW(Region, CB_ADDSTRING, (WPARAM)3, (LPARAM)L"Fairbanks");
    SendMessageW(Region, CB_ADDSTRING, (WPARAM)4, (LPARAM)L"Bethel");
    //Default option
    SendMessageW(Region, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);


    //Output folder 
    HWND outputFolderText = CreateWindowW(L"Static", L"Output Folder:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 180, 120, 20, hWnd, NULL, NULL, NULL);
    outputDir = CreateWindowW(L"Edit", L"Default: .\\Program Data\\FoundPosts\\", WS_VISIBLE | WS_CHILD | SS_LEFT | WS_BORDER | ES_AUTOHSCROLL,145, 180, 250, 20, hWnd, NULL, NULL, NULL);
    
    //SUBMIT
    HWND submit = CreateWindowW(L"Button", L"SUBMIT", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 190, 230, 120, 60, hWnd, (HMENU)SUBMIT_CONFIG, NULL, NULL);

    

}

// Add elements to FB login window
void AddFBLoginControls(HWND hWnd) 
{

    HWND titleText = CreateWindowW(L"Static", L"Facebook Login", WS_VISIBLE | WS_CHILD | SS_CENTER, 120, 10, 150, 30, hWnd, NULL, NULL, NULL);
    HWND usernameTextBox = CreateWindowW(L"Static", L"Email:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 50, 70, 30, hWnd, NULL, NULL, NULL);
    fbUser = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER| ES_MULTILINE | ES_AUTOVSCROLL | SS_LEFT, 95, 50, 100, 30, hWnd, NULL, NULL, NULL);
    HWND passwordTextBox = CreateWindowW(L"Static", L"Password:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 85, 70, 30, hWnd, NULL, NULL, NULL);
    fbPass = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | SS_LEFT, 95, 85, 100, 30, hWnd, NULL, NULL, NULL);
    FBsubmit = CreateWindowW(L"Button", L"SUBMIT", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | SS_CENTER, 110, 140, 120, 40, hWnd, (HMENU)FACEBOOK_LOGIN_SUBMIT, NULL, NULL);
    HWND checkBoxText = CreateWindowW(L"Static", L"Use Saved?", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT, 250, 80, 150, 30, hWnd, NULL, NULL, NULL);
    checkboxFB = CreateWindowW(L"Button", L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | SS_CENTER, 350, 85, 20, 20, hWnd, NULL, NULL, NULL);
}

// Add elements to FB login window
void AddIGLoginControls(HWND hWnd) 
{

    HWND titleText = CreateWindowW(L"Static", L"Instagram Login", WS_VISIBLE | WS_CHILD | SS_CENTER, 120, 10, 150, 30, hWnd, NULL, NULL, NULL);
    HWND usernameTextBox = CreateWindowW(L"Static", L"Username:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 50, 70, 30, hWnd, NULL, NULL, NULL);
    igUser = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | SS_LEFT, 95, 50, 100, 30, hWnd, NULL, NULL, NULL);
    HWND passwordTextBox = CreateWindowW(L"Static", L"Password:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 85, 70, 30, hWnd, NULL, NULL, NULL);
    igPass = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | SS_LEFT, 95, 85, 100, 30, hWnd, NULL, NULL, NULL);
    IGsubmit = CreateWindowW(L"Button", L"SUBMIT", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | SS_CENTER, 110, 140, 120, 40, hWnd, (HMENU)INSTAGRAM_LOGIN_SUBMIT, NULL, NULL);
    HWND checkBoxText = CreateWindowW(L"Static", L"Use Saved?", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT, 250, 80, 150, 30, hWnd, NULL, NULL, NULL);
    checkboxIG = CreateWindowW(L"Button", L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX| SS_CENTER, 350, 85, 20, 20, hWnd, NULL, NULL, NULL);
}

// Add elements to FB login window
void AddTWLoginControls(HWND hWnd) 
{

    HWND titleText = CreateWindowW(L"Static", L"Twitter Login", WS_VISIBLE | WS_CHILD | SS_CENTER, 120, 10, 150, 30, hWnd, NULL, NULL, NULL);
    HWND usernameTextBox = CreateWindowW(L"Static", L"Username:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 50, 70, 30, hWnd, NULL, NULL, NULL);
    twUser = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | SS_LEFT, 95, 50, 100, 30, hWnd, NULL, NULL, NULL);
    HWND passwordTextBox = CreateWindowW(L"Static", L"Password:", WS_VISIBLE | WS_CHILD | SS_RIGHT, 20, 85, 70, 30, hWnd, NULL, NULL, NULL);
    twPass = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | SS_LEFT, 95, 85, 100, 30, hWnd, NULL, NULL, NULL);
    TWsubmit = CreateWindowW(L"Button", L"SUBMIT", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | SS_CENTER, 110, 140, 120, 40, hWnd, (HMENU)TWITTER_LOGIN_SUBMIT, NULL, NULL);
    HWND checkBoxText = CreateWindowW(L"Static", L"Use Saved?", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT, 250, 80, 150, 30, hWnd, NULL, NULL, NULL);
    checkboxTW = CreateWindowW(L"Button", L"", WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BST_CHECKED | SS_CENTER, 350, 85, 20, 20, hWnd, NULL, NULL, NULL);
}

//Sets up the timer and starts it going, will send WM_TIMER messages every second 
void InitializeTimer()
{   
    //One second intervals
    SetTimer(MainWindow, NULL, 1000, NULL);

}

//Takes in a Flag denoted where to open file explorer and executes a shell api call
//Returns 1 on success, 0 on failure
int openFileExplorer(int FLAG)
{
    int i;
    char nonDefaultPathStr[100];
    if (NOT_DEFAULT_OUTPUT_DIR)
    {
        size_t wcharSize = wcslen(outputDirCaptured) + 1;
        size_t convertedChars = 0;
        const size_t newSize = wcharSize * 2;
        wcstombs_s(&convertedChars, nonDefaultPathStr, newSize, outputDirCaptured, _TRUNCATE);
    }
    HINSTANCE caught;
    switch (FLAG)
    {
    case RESULTS_FB:
        if (NOT_DEFAULT_OUTPUT_DIR)
        {              
            caught = ShellExecuteA(NULL, "open", (LPCSTR)nonDefaultPathStr, NULL, NULL, SW_SHOWDEFAULT);
            if (caught < (HINSTANCE)32) //Per windows documentation, error messages will be less than 32
            {
                return 0;
            }
        }
        else
        {
            caught = ShellExecuteA(NULL, "open", ".\\Program Data\\FoundPosts\\FoundPostsFB", NULL, NULL, SW_SHOWDEFAULT);
            if (caught < (HINSTANCE)32) //Per windows documentation, error messages will be less than 32
            {
                return 0;
            }
        }
        break;
    case RESULTS_IG:
        if (NOT_DEFAULT_OUTPUT_DIR)
        {
            caught = ShellExecuteA(NULL, "open", (LPCSTR)nonDefaultPathStr, NULL, NULL, SW_SHOWDEFAULT);
            if (caught < (HINSTANCE)32) //Per windows documentation, error messages will be less than 32
            {
                return 0;
            }
        }
        else 
        {
            caught = ShellExecuteA(NULL, "open", ".\\Program Data\\FoundPosts\\FoundPostsIG", NULL, NULL, SW_SHOWDEFAULT);
            if (caught < (HINSTANCE)32) //Per windows documentation, error messages will be less than 32
            {
                return 0;
            }
        }
        break;
    case RESULTS_TW:
        if (NOT_DEFAULT_OUTPUT_DIR)
        {
            caught = ShellExecuteA(NULL, "open", (LPCSTR)nonDefaultPathStr, NULL, NULL, SW_SHOWDEFAULT);
            if (caught < (HINSTANCE)32) //Per windows documentation, error messages will be less than 32
            {
                return 0;
            }
        }
        else
        {
            caught = ShellExecuteA(NULL, "open", ".\\Program Data\\FoundPosts\\FoundPostsTW", NULL, NULL, SW_SHOWDEFAULT);
            if (caught < (HINSTANCE)32) //Per windows documentation, error messages will be less than 32
            {
                return 0;
            }
        }
        break;
    case WORDLIST:
        caught = ShellExecuteA(NULL, "open", ".\\Program Data\\Wordlists", NULL, NULL, SW_SHOWDEFAULT);
        if (caught < (HINSTANCE)32) //Per windows documentation, error messages will be less than 32
        {
            return 0;
        }
        break;
    case FLAGGED_FB:
        caught = ShellExecuteA(NULL, "open", ".\\Program Data\\FlaggedUsers\\FBFlaggedUsers", NULL, NULL, SW_SHOWDEFAULT);
        if (caught < (HINSTANCE)32) //Per windows documentation, error messages will be less than 32
        {
            return 0;
        }
        break;
    case FLAGGED_IG:
        caught = ShellExecuteA(NULL, "open", ".\\Program Data\\FlaggedUsers\\IGFlaggedUsers", NULL, NULL, SW_SHOWDEFAULT);
        if (caught < (HINSTANCE)32) //Per windows documentation, error messages will be less than 32
        {
            return 0;
        }
        break;
    case FLAGGED_TW:
        caught = ShellExecuteA(NULL, "open", ".\\Program Data\\FlaggedUsers\\TWFlaggedUsers", NULL, NULL, SW_SHOWDEFAULT);
        if (caught < (HINSTANCE)32) //Per windows documentation, error messages will be less than 32
        {
            return 0;
        }
        break;

    }
    return 1;
}

//Execute shell commands to begin scraping for logged in sites
void lauchScanners(bool fbSet, bool igSet, bool twSet)
{
 
    std::string shellOperation;
    std::string outputDir;
    char outputDirShell[100];
    std::string regionSelection(REGION_SELECTION);
    std::string defaultPath = "DEFAULT";
    shellOperation.append("python3 ");
    if (NOT_DEFAULT_OUTPUT_DIR)
    {
        size_t wcharSize = wcslen(outputDirCaptured) + 1;
        size_t convertedChars = 0;
        const size_t newSize = wcharSize * 2;
        wcstombs_s(&convertedChars, outputDirShell, newSize, outputDirCaptured, _TRUNCATE);
    }

    if (fbSet)
    {
        shellOperation.append("scrape_fb.py ");
        shellOperation.append(regionSelection);
        shellOperation.append(" ");
        if (!NOT_DEFAULT_OUTPUT_DIR)
        {
            shellOperation.append(defaultPath);
        }
        else
        {
            shellOperation.append(outputDirShell);
        }        
        WinExec((LPCSTR)shellOperation.c_str(), SW_HIDE);
        SetWindowTextW(facebookResultsSummary, L"Scanning....");
    }
    if (igSet)
    {
        shellOperation = "python3 ";
        shellOperation.append("scrape_ig.py ");
        shellOperation.append(regionSelection);
        shellOperation.append(" ");
        if (!NOT_DEFAULT_OUTPUT_DIR)
        {
            shellOperation.append(defaultPath);
        }
        else
        {
            shellOperation.append(outputDirShell);
        }

        std::wstring widestr = std::wstring(shellOperation.begin(), shellOperation.end());
        const wchar_t* widecstr = widestr.c_str();

        WinExec((LPCSTR)shellOperation.c_str(), SW_HIDE);
        SetWindowTextW(instagramResultsSummary, L"Scanning....");
    }
    if (twSet)
    {
        shellOperation = "python3 ";
        shellOperation.append("scrape_tw.py ");
        shellOperation.append(regionSelection);
        shellOperation.append(" ");
        if (!NOT_DEFAULT_OUTPUT_DIR)
        {
            shellOperation.append(defaultPath);
        }
        else
        {
            shellOperation.append(outputDirShell);
        }
        WinExec((LPCSTR)shellOperation.c_str(), SW_HIDE);
        SetWindowTextW(twitterResultsSummary, L"Scanning....");
    }

}
//On "Launch", start a thread for each site where a login has been set, read from the incoming file 
void beginListeningforScrapeResults(bool fbSet, bool igSet, bool twSet)
{
    if (fbSet)
    {
        std::thread fbThread(readFBScrapeLog);
        fbThread.detach();
    }
    if (igSet)
    {
        std::thread igThread(readIGScrapeLog);
        igThread.detach();
    }
    if (twSet)
    {
        std::thread twThread(readTWScrapeLog);
        twThread.detach();
    }
}


void readFBScrapeLog()
{
    Sleep(2000);
    std::fstream readOutputLog;
    std::string line;
    std::string full_log_text = "";
    bool END_THREAD = false;
    while (!END_THREAD)
    {
        Sleep(2000);
        readOutputLog.open(".\\Program Data\\Logs\\FB_SCRAPE_LOGS\\log.txt", std::ios::in);
        if (readOutputLog.is_open())
        {
            while (std::getline(readOutputLog, line))
            {
                full_log_text.append("\r\n");
                line += "\r\n";
                full_log_text.append(line);
                if (strcmp(line.c_str(), "SCAN COMPELTE") == 0) {
                    END_THREAD = true;
                }
            }
            std::wstring wide_string = std::wstring(full_log_text.begin(), full_log_text.end());
            const wchar_t* scanUpdate = wide_string.c_str();
            SetWindowTextW(facebookResultsSummary, scanUpdate);
            full_log_text = "";
        }
        readOutputLog.close();
    }
    //Kill this thread
    std::terminate();
}

void readIGScrapeLog()
{
    Sleep(2000);
    std::fstream readOutputLog;
    std::string line;
    std::string full_log_text = "";
    bool END_THREAD = false;
    while (!END_THREAD)
    {
        Sleep(2000);
        readOutputLog.open(".\\Program Data\\Logs\\IG_SCRAPE_LOGS\\log.txt", std::ios::in);
        if (readOutputLog.is_open())
        {
            while (std::getline(readOutputLog, line))
            {
                full_log_text.append("\r\n");
                line += "\r\n";
                full_log_text.append(line);
                if (strcmp(line.c_str(), "SCAN COMPELTE") == 0) {
                    END_THREAD = true;
                }
            }
            std::wstring wide_string = std::wstring(full_log_text.begin(), full_log_text.end());
            const wchar_t* scanUpdate = wide_string.c_str();
            SetWindowTextW(instagramResultsSummary, scanUpdate);
            full_log_text = "";
        }
        readOutputLog.close();
    }
    //Kill this thread
    std::terminate();
}


void readTWScrapeLog()
{
    Sleep(2000);
    std::fstream readOutputLog;
    std::string line;
    std::string full_log_text = "";
    bool END_THREAD = false;
    while (!END_THREAD)
    {
        Sleep(2000);
        readOutputLog.open(".\\Program Data\\Logs\\TW_SCRAPE_LOGS\\log.txt", std::ios::in);
        if (readOutputLog.is_open())
        {
            while (std::getline(readOutputLog, line))
            {
                full_log_text.append("\r\n");
                line += "\r\n";
                full_log_text.append(line);
                if (strcmp(line.c_str(), "SCAN COMPELTE") == 0) {
                    END_THREAD = true;
                }
            }
            std::wstring wide_string = std::wstring(full_log_text.begin(), full_log_text.end());
            const wchar_t* scanUpdate = wide_string.c_str();
            SetWindowTextW(twitterResultsSummary, scanUpdate);
            full_log_text = "";
        }
        readOutputLog.close();
    }
    //Kill this thread
    std::terminate();

}

//Should make the window header CWD if working right
void Test() 
{
    SetWindowTextW(MainWindow, CWD);

}