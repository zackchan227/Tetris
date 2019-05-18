// Tetris.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Tetris.h"
#include <Windows.h>
#include <windowsX.h>
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")
#include <commdlg.h>
#include <strsafe.h>

using namespace std;

// List of song
Music List[] =
{
	// index for the songs

	{ __T("haru.mp3") },				// 0
	{ __T("tot.mp3") },					// 1
	{ __T("awaken.mp3") },				// 2
	{ __T("sundialdreams.mp3") },		// 3
	{ __T("OP.mp3") },					// 4
	{ __T("holyknight.mp3") },			// 5
	{ __T("mabinogi.mp3") },			// 6
	{ __T("victory.mp3") },				// 7
	{ __T("spectre.mp3") },				// 8
	{ __T("matsuri.mp3") },				// 9
	{ __T("flowerdance.mp3") },			// 10
	{ __T("tetris.mp3") },				// 11
};

#define MAX_LOADSTRING 100

// Global Variables:
const int PX_PER_BLOCK = 25;    // Cell size in pixels
const int SCREEN_WIDTH = 10;    // Level width in cells
const int SCREEN_HEIGHT = 20;   // Level height in cells
const int GAME_SPEED = 33;      // Update the game every GAME_SPEED millisecs (= 1/fps)
const int TIMER_ID = 1;
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
DrawEngine *de;
Game *game;
HWND hWnd;
bool firstTime = TRUE;
HFONT hFont;
HDC hdc;
PAINTSTRUCT ps;
LPCREATESTRUCT lpCreateStruct;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	NewGame(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Control(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Score(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Setting(HWND, UINT, WPARAM, LPARAM);

BOOL				OnCreate(HWND, LPCREATESTRUCT);
void				OnCommand(HWND, int, HWND, UINT);
void				OnDestroy(HWND);
void				OnPaint(HWND);
void				OnKey(HWND, UINT, BOOL, int, UINT);
void				OnTime(HWND,UINT);
void				OnKill(HWND, HWND);
void				OnSet(HWND, HWND);

//bool				LoadAndBlitBitmap(LPCWSTR, HDC);
void				startGame(HWND, HDC);
void				changeMusic(HWND);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TETRIS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TETRIS));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
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
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRIS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TETRIS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable


   hWnd = CreateWindow(szWindowClass,
	   TEXT("Tetris"),
	   WS_MINIMIZEBOX | WS_SYSMENU,  // No window resizing
	   CW_USEDEFAULT,
	   CW_USEDEFAULT,
	   SCREEN_WIDTH * PX_PER_BLOCK + 179,
	   SCREEN_HEIGHT * PX_PER_BLOCK + 60,
	   NULL,
	   NULL,
	   hInstance,
	   NULL);
  
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
//  PURPOSE:  Processes messages for the main window.
//
//  WM_CREATE	- create game engine, level
//  WM_COMMAND	- process the application menu
//  WM_KEYDOWN  - capture the input key
//  WM_TIMER	- update the main window
//  WM_KILLFOCUS- time stop main window
//  WM_SETFOCUS - set time 
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case (WM_CREATE) : return HANDLE_WM_CREATE((hWnd), (wParam), (lParam), (OnCreate));
	case (WM_COMMAND) : return HANDLE_WM_COMMAND((hWnd), (wParam), (lParam), (OnCommand));
	case (WM_KEYDOWN) : return HANDLE_WM_KEYDOWN((hWnd), (wParam), (lParam), (OnKey));
	case (WM_TIMER) : return HANDLE_WM_TIMER((hWnd), (wParam), (lParam), (OnTime));
	case (WM_KILLFOCUS) : return HANDLE_WM_KILLFOCUS((hWnd), (wParam), (lParam), (OnKill));
	case (WM_SETFOCUS) : return HANDLE_WM_SETFOCUS((hWnd), (wParam), (lParam), (OnSet));
	case (WM_PAINT) : return HANDLE_WM_PAINT((hWnd), (wParam), (lParam), (OnPaint));
	case (WM_DESTROY) : return HANDLE_WM_DESTROY((hWnd), (wParam), (lParam), (OnDestroy));
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

// Message handler for new game box.
INT_PTR CALLBACK NewGame(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			mciSendString(L"seek mp3 to 0", NULL, 0, 0);
			//mciSendString(L"close mp3", NULL, 0, hWnd); // Close the previous song
			//mciSendString(L"open music\\tetris.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
			mciSendString(L"play mp3 repeat", NULL, 0, 0);
			game->restart();
			EndDialog(hDlg, LOWORD(wParam));
		}
		else 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
		
	}
	return (INT_PTR)FALSE;
}

// Message handler for control box.
INT_PTR CALLBACK Control(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for score box.
INT_PTR CALLBACK Score(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for setting box.
INT_PTR CALLBACK Setting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hwndList;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		// Add items to list. 
		hwndList = GetDlgItem(hDlg, IDC_MUSICLIST);
		for (int i = 0; i < ARRAYSIZE(List); i++)
		{
			int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0,(LPARAM)List[i].achName);
			// Set the array index of the song as item data.
			// This enables us to retrieve the item from the array
			// even after the items are sorted by the list box.
			SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM)i);
		}
		// Set input focus to the list box.
		SetFocus(hwndList);
		return TRUE;
	}
	
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:

			//change before on-off volume
			changeMusic(hDlg);

			if (IsDlgButtonChecked(hDlg, IDC_ON) == BST_CHECKED)
			{
				mciSendString(L"set mp3 audio all on", NULL, 0, hWnd);
				
			}
			else if (IsDlgButtonChecked(hDlg, IDC_OFF) == BST_CHECKED)
			{
				mciSendString(L"set mp3 audio all off", NULL, 0, hWnd);
			}
			
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)FALSE;
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		return TRUE;
		}
	}
	return (INT_PTR)FALSE;
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	hdc = GetDC(hwnd);
	MessageBox(hwnd, L"Use headphones for the best experience", L"TIP", MB_OK);
	startGame(hwnd, hdc);
	return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	// Parse the menu selections:
	switch (id)
	{
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
		break;
	case IDM_NEWGAME:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_NEWGAMEBOX), hwnd, NewGame);
		break;
	case IDM_CONTROL:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_CONTROLBOX), hwnd, Control);
		break;
	case IDM_SCORE:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_SCOREBOX), hwnd, Score);
		break;
	case IDM_SETTING:
		if (IDOK == DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGBOX), hwnd, Setting))
			UpdateWindow(hwnd);
		break;
	case IDM_EXIT:
		DestroyWindow(hwnd);
		break;
	}
}

void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	game->keyPress(vk);
}

void OnTime(HWND hwnd, UINT id)
{
	game->timerUpdate();
}

void OnKill(HWND hwnd, HWND newhwndfocus)
{
	KillTimer(hWnd, TIMER_ID);
	game->pause(true);
}

void OnSet(HWND hwnd, HWND oldhwndfocus)
{
	SetTimer(hWnd, TIMER_ID, GAME_SPEED, NULL);
}

void OnPaint(HWND hwnd)
{
	
	hdc = BeginPaint(hwnd, &ps);
	// TODO: Add any drawing code here...
	//LoadAndBlitBitmap(__T("main.bmp"), hdc);
	game->repaint();
	EndPaint(hwnd, &ps);
}

void OnDestroy(HWND hwnd)
{
	//PlaySound(L"shutdown.WAV", NULL, SND_SYNC);
	delete game, de;
	PostQuitMessage(0);
}


// load the image
//bool LoadAndBlitBitmap(LPCWSTR szFileName, HDC hWinDC)
//{
//	// Load the bitmap image file
//	HBITMAP hBitmap;
//	hBitmap = (HBITMAP)::LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0,
//		LR_LOADFROMFILE);
//	// Verify that the image was loaded
//	if (hBitmap == NULL) {
//		::MessageBox(NULL, __T("LoadImage Failed"), __T("Error"), MB_OK);
//		return false;
//	}
//
//	// Create a device context that is compatible with the window
//	HDC hLocalDC;
//	hLocalDC = ::CreateCompatibleDC(hWinDC);
//	// Verify that the device context was created
//	if (hLocalDC == NULL) {
//		::MessageBox(NULL, __T("CreateCompatibleDC Failed"), __T("Error"), MB_OK);
//		return false;
//	}
//
//	// Get the bitmap's parameters and verify the get
//	BITMAP qBitmap;
//	int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP),
//		reinterpret_cast<LPVOID>(&qBitmap));
//	if (!iReturn) {
//		::MessageBox(NULL, __T("GetObject Failed"), __T("Error"), MB_OK);
//		return false;
//	}
//
//	// Select the loaded bitmap into the device context
//	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hLocalDC, hBitmap);
//	if (hOldBmp == NULL) {
//		::MessageBox(NULL, __T("SelectObject Failed"), __T("Error"), MB_OK);
//		return false;
//	}
//
//	// Blit the dc which holds the bitmap onto the window's dc
//	BOOL qRetBlit = ::BitBlt(hWinDC, 0, 0, qBitmap.bmWidth, qBitmap.bmHeight,
//		hLocalDC, 0, 0, SRCCOPY);
//	if (!qRetBlit) {
//		::MessageBox(NULL, __T("Blit Failed"), __T("Error"), MB_OK);
//		return false;
//	}
//
//	// Unitialize and deallocate resources
//	::SelectObject(hLocalDC, hOldBmp);
//	::DeleteDC(hLocalDC);
//	::DeleteObject(hBitmap);
//	return true;
//}

// draw and create the game
void startGame(HWND hWnd, HDC hdc)
{
	de = new DrawEngine(hdc, hWnd, PX_PER_BLOCK);
	game = new Game(*de);
	mciSendString(L"open ..\\Source\\Tetris\\music\\tetris.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
	mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
	SetTimer(hWnd, TIMER_ID, GAME_SPEED, NULL);

	ReleaseDC(hWnd, hdc);
}

// change the music
void changeMusic(HWND hDlg)
{
	HWND hwndList;
	int lbItem;
	int i;
	hwndList = GetDlgItem(hDlg, IDC_MUSICLIST);
	// Get selected index.
	lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
	// Get item data.
	i = (int)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);

	switch (i)
	{
	case 0:
		// Close the previous song
		mciSendString(L"close mp3", NULL, 0, 0);
		// Play song
		mciSendString(L"open ..\\Source\\Tetris\\music\\haru.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
		break;
	case 1:
		// Close the previous song
		mciSendString(L"close mp3", NULL, 0, 0);
		// Play song
		mciSendString(L"open ..\\Source\\Tetris\\music\\tot.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
		break;
	case 2:
		// Close the previous song
		mciSendString(L"close mp3", NULL, 0, 0);
		// Play song
		mciSendString(L"open ..\\Source\\Tetris\\music\\awaken.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
		break;
	case 3:
		// Close the previous song
		mciSendString(L"close mp3", NULL, 0, 0);
		// Play song
		mciSendString(L"open ..\\Source\\Tetris\\music\\sundialdreams.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
		break;
	case 4:
		// Close the previous song
		mciSendString(L"close mp3", NULL, 0, 0);
		// Play song
		mciSendString(L"open ..\\Source\\Tetris\\music\\OP.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
		break;
	case 5:
		// Close the previous song
		mciSendString(L"close mp3", NULL, 0, 0);
		// Play song
		mciSendString(L"open ..\\Source\\Tetris\\music\\holyknight.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
		break;
	case 6:
		// Close the previous song
		mciSendString(L"close mp3", NULL, 0, 0);
		// Play song
		mciSendString(L"open ..\\Source\\Tetris\\music\\mabinogi.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
		break;
	case 7:
		// Close the previous song
		mciSendString(L"close mp3", NULL, 0, 0);
		// Play song
		mciSendString(L"open ..\\Source\\Tetris\\music\\victory.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
		break;
	case 8:
		// Close the previous song
		mciSendString(L"close mp3", NULL, 0, 0);
		// Play song
		mciSendString(L"open ..\\Source\\Tetris\\music\\spectre.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
		break;
	case 9:
		// Close the previous song
		mciSendString(L"close mp3", NULL, 0, 0);
		// Play song
		mciSendString(L"open ..\\Source\\Tetris\\music\\matsuri.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
		break;
	case 10:
		// Close the previous song
		mciSendString(L"close mp3", NULL, 0, 0);
		// Play song
		mciSendString(L"open ..\\Source\\Tetris\\music\\flowerdance.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
		break;
	case 11:
		// Close the previous song
		mciSendString(L"close mp3", NULL, 0, 0);
		// Play song
		mciSendString(L"open ..\\Source\\Tetris\\music\\tetris.mp3 type mpegvideo alias mp3", NULL, 0, NULL);
		mciSendString(L"play mp3 repeat", NULL, 0, hWnd);
		break;
	}
}