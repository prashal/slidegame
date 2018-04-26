// slidegame.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "slidegame.h"

#define MAX_LOADSTRING 100
#define SUBBLOCKSIZE 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

enum 
{
	EASY=IDC_EASY,MEDIUM=IDC_MEDIUM,HARD=IDC_HARD
}GameDifficulty=EASY;

enum
{
	NUMBERS=IDC_NUMBERS,LETTERS=IDC_LETTERS,CUSTOM=IDC_CUSTOM
}GameMode=NUMBERS;

WCHAR szImage[260]; 

OPENFILENAME ofn;       // common dialog box structure
WCHAR szFile[260];       // temporary buffer for file name
WCHAR szFileFilter[]=_T("Bitmap Images\0*.bmp\0All Files\0*.*\0");

int iSlideBox[25];
int iNumMove;
HBITMAP hBit=0;
HBITMAP hBitTemp=0;
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	AboutProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	SettingsProc(HWND, UINT, WPARAM, LPARAM);
int InitializeBlock(int num);
int Suffle(int num);
int IsCompleted(int num);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SLIDEGAME, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SLIDEGAME));

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
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SLIDEGAME));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SLIDEGAME);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SLIDEGAME));

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
	int num=(GameDifficulty==EASY?3:(GameDifficulty==MEDIUM?4:5));
	/*InitializeBlock(num);
	Suffle(num);*/
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED| WS_CAPTION |WS_SYSMENU|WS_DLGFRAME|WS_MINIMIZEBOX,CW_USEDEFAULT, 0,SUBBLOCKSIZE*num+20, SUBBLOCKSIZE*num+110, NULL, NULL, hInstance, NULL);
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
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_HELP_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutProc);
			break;
		case ID_FILE_NEWGAME:
			{
				int num=(GameDifficulty==EASY?3:(GameDifficulty==MEDIUM?4:5));
				InitializeBlock(num);
			}
		case ID_FILE_SUFFLE:
			{
				int num=(GameDifficulty==EASY?3:(GameDifficulty==MEDIUM?4:5));
				Suffle(num);
			}
			InvalidateRect(hWnd,NULL,TRUE);
			break;
		case ID_FILE_SETTINGS:
			{
				int prevGamedifficulty=GameDifficulty;
				DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGS), hWnd, SettingsProc);
				if(GameDifficulty!=prevGamedifficulty)
				{
					int num=(GameDifficulty==EASY?3:(GameDifficulty==MEDIUM?4:5));
					RECT rect;
					GetWindowRect(hWnd,&rect);
					MoveWindow(hWnd,rect.left,rect.top,num*SUBBLOCKSIZE+20,num*SUBBLOCKSIZE+110,TRUE);
					InitializeBlock(num);
					Suffle(num);
				}
				if(hBit)
					DeleteObject(hBit);
				hBit=hBitTemp;
				InvalidateRect(hWnd,NULL,TRUE);
			}
			break;
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if(!hBitTemp)
		{
			GameMode=NUMBERS;
			GameDifficulty=EASY;
			hBitTemp=(HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_3NUMBER),IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
			hBit=hBitTemp;
			InitializeBlock(3);
			Suffle(3);
		}
		if(hBit)
		{
			HDC hMem=CreateCompatibleDC(hdc);
			HPEN hGray=CreatePen(PS_SOLID,0,RGB(150,150,150));
			SelectObject(hdc,hGray);
			SelectObject(hMem,hBit);	

			int count,value;
			int num=(GameDifficulty==EASY?3:(GameDifficulty==MEDIUM?4:5));

			BITMAP bm;
			GetObject(hBit, sizeof(bm), &bm);
			
			SetStretchBltMode(hdc,COLORONCOLOR);
			SIZE sizeSubPart={bm.bmWidth/num,bm.bmHeight/num};
			for(count=0;count<num*num;count++)
			{
				value=iSlideBox[count]-1;
				if(value<0)
				{
					RECT rt={7+SUBBLOCKSIZE*(count%num),7+SUBBLOCKSIZE*(count/num),7+SUBBLOCKSIZE*(count%num+1),7+SUBBLOCKSIZE*(count/num+1)};
					Rectangle(hdc,rt.left,rt.top,rt.right,rt.bottom);
					DrawEdge(hdc,&rt,EDGE_SUNKEN,BF_RECT);
				}
				else
					StretchBlt(hdc,7+SUBBLOCKSIZE*(count%num),7+SUBBLOCKSIZE*(count/num),SUBBLOCKSIZE,SUBBLOCKSIZE,hMem,sizeSubPart.cx*(value%num),sizeSubPart.cy*(value/num),sizeSubPart.cx,sizeSubPart.cy,SRCCOPY);
			}
			DeleteDC(hMem);

			for(count=1;count<num;count++)
			{
				MoveToEx(hdc,7+count*SUBBLOCKSIZE,7,NULL);
				LineTo(hdc,7+count*SUBBLOCKSIZE,7+num*SUBBLOCKSIZE);

				MoveToEx(hdc,7,7+count*SUBBLOCKSIZE,NULL);
				LineTo(hdc,7+num*SUBBLOCKSIZE,7+count*SUBBLOCKSIZE);
			}	
			DeleteObject(hGray);
			/*MoveToEx(hdc,7,7,NULL);
			LineTo(hdc,7+num*SUBBLOCKSIZE,7);
			LineTo(hdc,7+num*SUBBLOCKSIZE,7+num*SUBBLOCKSIZE);
			LineTo(hdc,7,7+num*SUBBLOCKSIZE);
			LineTo(hdc,7,7);*/
			RECT rt={7,7,7+num*SUBBLOCKSIZE,7+num*SUBBLOCKSIZE};
			DrawEdge(hdc,&rt,EDGE_BUMP,BF_RECT);
			
			//SetBkColor(hdc,GetSysColor(COLOR_WINDOW));
			char str[50];
			SetTextColor(hdc,RGB(100,0,0));
			LOGFONT lf={0};
			lf.lfHeight=18;
			lf.lfWidth=9;
			wcscpy(lf.lfFaceName,_T("Times New Roman"));
			HFONT hf=CreateFontIndirect(&lf);
			SelectObject(hdc,hf);
			sprintf(str,"Moves : %d" ,iNumMove);
			RoundRect(hdc,7,15+num*SUBBLOCKSIZE,7+num*SUBBLOCKSIZE,50+num*SUBBLOCKSIZE,10,10);
			TextOutA(hdc,15,25+num*SUBBLOCKSIZE,str,strlen(str));

			if(IsCompleted(num))
				sprintf(str,"Score : %d",num*200-5*iNumMove);
			else
				sprintf(str,"Score : %d",num*100-5*iNumMove);
			TextOutA(hdc,num*SUBBLOCKSIZE-100,25+num*SUBBLOCKSIZE,str,strlen(str));
			DeleteObject(hf);
			
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
		{ 
			int num=(GameDifficulty==EASY?3:(GameDifficulty==MEDIUM?4:5));
			if(GET_X_LPARAM(lParam)>7&&GET_X_LPARAM(lParam)<(7+SUBBLOCKSIZE*num)&&GET_Y_LPARAM(lParam)>7&&GET_Y_LPARAM(lParam)<(7+SUBBLOCKSIZE*num))
			{
				int xpress=(GET_X_LPARAM(lParam)-7)/SUBBLOCKSIZE,ypress=(GET_Y_LPARAM(lParam)-7)/SUBBLOCKSIZE;
				if(iSlideBox[xpress+ypress*num]==0)
					break;
				HDC hdc=GetDC(hWnd);
				RECT rt={xpress*SUBBLOCKSIZE+7,ypress*SUBBLOCKSIZE+7,xpress*SUBBLOCKSIZE+7+SUBBLOCKSIZE,ypress*SUBBLOCKSIZE+7+SUBBLOCKSIZE};
				DrawFocusRect(hdc,&rt);
				ReleaseDC(hWnd,hdc);
			}
		}
		break;
	case WM_KEYDOWN: 
		if(wParam==VK_UP||wParam==VK_UP||wParam==VK_LEFT||wParam==VK_RIGHT||wParam==VK_DOWN)
		{ 
			int num=(GameDifficulty==EASY?3:(GameDifficulty==MEDIUM?4:5));
			int gap,xgap,ygap;

			for(gap=0;gap<num*num;gap++)
			{
				if(iSlideBox[gap]==0) break;
			}

			xgap=gap%num;
			ygap=gap/num;

			if(wParam==VK_RIGHT&&xgap>0)		//Right
			{
				iSlideBox[xgap+ygap*num]=iSlideBox[xgap+ygap*num-1];
				iSlideBox[xgap+ygap*num-1]=0;
				iNumMove++;
				InvalidateRect(hWnd,NULL,FALSE);
			}
			else if(wParam==VK_LEFT&&xgap<(num-1))		//Left
			{
				iSlideBox[xgap+ygap*num]=iSlideBox[xgap+ygap*num+1];
				iSlideBox[xgap+ygap*num+1]=0;
				iNumMove++;
				InvalidateRect(hWnd,NULL,FALSE);

			}
			else if(wParam==VK_DOWN&&ygap>0) //Down
			{
				iSlideBox[xgap+ygap*num]=iSlideBox[xgap+ygap*num-num];
				iSlideBox[xgap+ygap*num-num]=0;
				iNumMove++;
				InvalidateRect(hWnd,NULL,FALSE);
			}
			else if(wParam==VK_UP&&ygap<(num-1))		//Up
			{
				iSlideBox[xgap+ygap*num]=iSlideBox[xgap+ygap*num+num];
				iSlideBox[xgap+ygap*num+num]=0;
				iNumMove++;
				InvalidateRect(hWnd,NULL,FALSE);
			}
			if(IsCompleted(num))
			{
				WCHAR str[50];
				wsprintf(str,_T("Winner : (Score-%d)"),num*200-5*iNumMove);
				if(MessageBox(hWnd,_T("Congratulations-You have successfully finished this game.\nDo you want to play another game?\nPress \"Yes\" to play again or \"No\" to Exit from this application"),str,MB_YESNO)==IDYES)
				{
					iNumMove=0;
					Suffle(num);
					InvalidateRect(hWnd,NULL,FALSE);
				}
				else
					SendMessage(hWnd,WM_DESTROY,0,0);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{ 
			int num=(GameDifficulty==EASY?3:(GameDifficulty==MEDIUM?4:5));
			if(GET_X_LPARAM(lParam)>7&&GET_X_LPARAM(lParam)<(7+SUBBLOCKSIZE*num)&&GET_Y_LPARAM(lParam)>7&&GET_Y_LPARAM(lParam)<(7+SUBBLOCKSIZE*num))
			{
				int xpress=(GET_X_LPARAM(lParam)-7)/SUBBLOCKSIZE,ypress=(GET_Y_LPARAM(lParam)-7)/SUBBLOCKSIZE;
				if(xpress<(num-1)&&iSlideBox[xpress+ypress*num+1]==0)		//Right
				{
					iSlideBox[xpress+ypress*num+1]=iSlideBox[xpress+ypress*num];
					iSlideBox[xpress+ypress*num]=0;
					iNumMove++;
					InvalidateRect(hWnd,NULL,FALSE);
				}
				else if(xpress>0&&iSlideBox[xpress+ypress*num-1]==0)		//Left
				{
					iSlideBox[xpress+ypress*num-1]=iSlideBox[xpress+ypress*num];
					iSlideBox[xpress+ypress*num]=0;
					iNumMove++;
					InvalidateRect(hWnd,NULL,FALSE);

				}
				else if(ypress<(num-1)&&iSlideBox[xpress+ypress*num+num]==0) //Down
				{
					iSlideBox[xpress+ypress*num+num]=iSlideBox[xpress+ypress*num];
					iSlideBox[xpress+ypress*num]=0;
					iNumMove++;
					InvalidateRect(hWnd,NULL,FALSE);
				}
				else if(ypress>0&&iSlideBox[xpress+ypress*num-num]==0)		//Up
				{
					iSlideBox[xpress+ypress*num-num]=iSlideBox[xpress+ypress*num];
					iSlideBox[xpress+ypress*num]=0;
					iNumMove++;
					InvalidateRect(hWnd,NULL,FALSE);
				}
				if(IsCompleted(num))
				{
					WCHAR str[50];
					wsprintf(str,_T("Winner : (Score-%d)"),num*200-5*iNumMove);
					if(MessageBox(hWnd,_T("Congratulations-You have successfully finished this game.\nDo you want to play another game?\nPress \"Yes\" to play again or \"No\" to Exit from this application"),str,MB_YESNO)==IDYES)
					{
						iNumMove=0;
						Suffle(num);
						InvalidateRect(hWnd,NULL,FALSE);
					}
					else
						SendMessage(hWnd,WM_DESTROY,0,0);
				}
			}
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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


// Message handler for about box.
INT_PTR CALLBACK SettingsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		CheckRadioButton(hDlg,IDC_EASY,IDC_HARD,GameDifficulty);
		CheckRadioButton(hDlg,IDC_NUMBERS,IDC_CUSTOM,GameMode);
		if(GameMode!=CUSTOM)
		{
			EnableWindow(GetDlgItem(hDlg,IDC_STATICBROWSE),FALSE);
			EnableWindow(GetDlgItem(hDlg,ID_BROWSE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_BROWSE),FALSE);
		}
		else
			SetDlgItemText(hDlg,IDC_BROWSE,szImage);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_BROWSE:
			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = szFile;
			//
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			//
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile)/sizeof(TCHAR);
			ofn.lpstrFilter = szFileFilter;
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST|OFN_EXPLORER;

			// Display the Open dialog box. 

			if (GetOpenFileName(&ofn)==TRUE)
			{
				wcscpy_s(szImage,ofn.lpstrFile);
				RECT rt={230,10,380,160};
				SetDlgItemText(hDlg,IDC_BROWSE,szImage);
				InvalidateRect(hDlg,&rt,TRUE);
			}
			break;
		case IDC_CUSTOM:
		case IDC_LETTERS:
		case IDC_NUMBERS:
			{
			if(IsDlgButtonChecked(hDlg,IDC_CUSTOM)==BST_UNCHECKED)
			{
				EnableWindow(GetDlgItem(hDlg,IDC_STATICBROWSE),FALSE);
				EnableWindow(GetDlgItem(hDlg,ID_BROWSE),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_BROWSE),FALSE);
			}
			else
			{
				EnableWindow(GetDlgItem(hDlg,IDC_STATICBROWSE),TRUE);
				EnableWindow(GetDlgItem(hDlg,ID_BROWSE),TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_BROWSE),TRUE);
				SetDlgItemText(hDlg,IDC_BROWSE,szImage);
			}
		case IDC_EASY:
		case IDC_MEDIUM:
		case IDC_HARD:
			{
				RECT rt={230,10,380,160};
				InvalidateRect(hDlg,&rt,TRUE);}
			}
			break;
		case ID_OK:
			if(!hBitTemp)
			{
				if(MessageBox(hDlg,_T("No custom image is loded. If you press \"Yes\" default settings wil ne selected.\nIf you want to enter new image file press \"No\"."),_T("Custom Image Error..."),MB_YESNO)==IDYES)
				{
					GameDifficulty=EASY;
					GameMode=NUMBERS;
					hBitTemp=(HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_3NUMBER),IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
				}
				else
					break;
			}
			else
			{
				if(IsDlgButtonChecked(hDlg,IDC_EASY)==BST_CHECKED)
					GameDifficulty=EASY;
				else if(IsDlgButtonChecked(hDlg,IDC_MEDIUM)==BST_CHECKED)
					GameDifficulty=MEDIUM;
				else
					GameDifficulty=HARD;

				if(IsDlgButtonChecked(hDlg,IDC_NUMBERS)==BST_CHECKED)
					GameMode=NUMBERS;
				else if(IsDlgButtonChecked(hDlg,IDC_LETTERS)==BST_CHECKED)
					GameMode=LETTERS;
				else
					GameMode=CUSTOM;
			}
		case ID_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hDlg, &ps);
			// TODO: Add any drawing code here...
			HDC hMem=CreateCompatibleDC(hdc);
			if(hBitTemp!=hBit)
				DeleteObject(hBitTemp);
			if(IsDlgButtonChecked(hDlg,IDC_CUSTOM)==BST_CHECKED)
			{
				hBitTemp=(HBITMAP)LoadImage(0,szImage,IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				/*if(!hBitTemp)
				{
					wcscpy_s(szImage,_T("test.bmp"));
					CheckRadioButton(hDlg,IDC_NUMBERS,IDC_CUSTOM,GameMode);
				}*/
			}
			else if(IsDlgButtonChecked(hDlg,IDC_LETTERS)==BST_CHECKED)
			{
				if(IsDlgButtonChecked(hDlg,IDC_EASY)==BST_CHECKED)
					hBitTemp=(HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_3LETTER),IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
				else if(IsDlgButtonChecked(hDlg,IDC_MEDIUM)==BST_CHECKED)
					hBitTemp=(HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_4LETTER),IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
				else
					hBitTemp=(HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_5LETTER),IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
				
			}
			if(IsDlgButtonChecked(hDlg,IDC_NUMBERS)==BST_CHECKED)
			{
				if(IsDlgButtonChecked(hDlg,IDC_EASY)==BST_CHECKED)
					hBitTemp=(HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_3NUMBER),IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
				else if(IsDlgButtonChecked(hDlg,IDC_MEDIUM)==BST_CHECKED)
					hBitTemp=(HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_4NUMBER),IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
				else
					hBitTemp=(HBITMAP)LoadImage(hInst,MAKEINTRESOURCE(IDB_5NUMBER),IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
			}
			BITMAP bm;
			GetObject(hBitTemp, sizeof(bm), &bm);
			SelectObject(hMem,hBitTemp);
			SetStretchBltMode(hdc,COLORONCOLOR);
			StretchBlt(hdc,230,10,150,150,hMem,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
			DeleteDC(hMem);
			
			HPEN hBlack=CreatePen(PS_SOLID,3,RGB(0,0,0));
			HPEN hGray=CreatePen(PS_SOLID,1,RGB(100,100,100));
			SelectObject(hdc,hGray);

			int num=5;
			if(IsDlgButtonChecked(hDlg,IDC_EASY)==BST_CHECKED)
				num=3;
			else if(IsDlgButtonChecked(hDlg,IDC_MEDIUM)==BST_CHECKED)
				num=4;

			for(int count=1;count<num;count++)
			{
				MoveToEx(hdc,int(230+count*(150.0/num)),10,NULL);
				LineTo(hdc,int(230+count*(150.0/num)),160);

				MoveToEx(hdc,230,int(10+count*(150.0/num)),NULL);
				LineTo(hdc,380,int(10+count*(150.0/num)));
			}
			Rectangle(hdc,int(380-150.0/num),int(160-150.0/num),380,160);	
			DeleteObject(hGray);

			SelectObject(hdc,hBlack);
			MoveToEx(hdc,230,10,NULL);
			LineTo(hdc,230,160);
			LineTo(hdc,380,160);
			LineTo(hdc,380,10);
			LineTo(hdc,230,10);			

			DeleteObject(hBlack);
			EndPaint(hDlg, &ps);
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, LOWORD(wParam));
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}


//Function for suffling the subblocks
int Suffle(int num)
{
#define UP 1			
#define DOWN 2
#define RIGHT 4
#define LEFT 8

	int lastmove=0,nextmove,ranval,temp; 
	int blank,i;
	for(blank=0;blank<num*num;blank++)
	{
		if(iSlideBox[blank]==0)
			break;
	}
	if(blank==num*num)
		return 0;
	//blankx=blank%num;
	//blanky=blank/num;
	for(i=num*num*5;i>0;i--)
	{
		nextmove=0;
		if((blank%num)>0&&lastmove!=RIGHT)	//Left
			nextmove|=LEFT;					//means the blank has to move left 
		if((blank%num)<(num-1)&&lastmove!=LEFT)	//Right
			nextmove|=RIGHT;
		if((blank/num)>0&&lastmove!=DOWN)	//Up
			nextmove|=UP;
		if((blank/num)<(num-1)&&lastmove!=UP)	//Down
			nextmove|=DOWN;
		temp=((nextmove&DOWN)!=0?1:0)+((nextmove&UP)!=0?1:0)+((nextmove&RIGHT)!=0?1:0)+((nextmove&LEFT)!=0?1:0);
		srand(time(0)+i);
		ranval=rand();
		ranval=ranval%temp;
		if(nextmove&DOWN)
		{
			if(ranval==0)
			{
				iSlideBox[blank]=iSlideBox[blank+num];
				iSlideBox[blank+num]=0;
				lastmove=DOWN;
				blank+=num;
				continue;
			}
			ranval--;
		}
		if(nextmove&UP)
		{
			if(ranval==0)
			{
				iSlideBox[blank]=iSlideBox[blank-num];
				iSlideBox[blank-num]=0;
				lastmove=UP;
				blank-=num;
				continue;
			}
			ranval--;
		}
		if(nextmove&LEFT)
		{
			if(ranval==0)
			{
				iSlideBox[blank]=iSlideBox[blank-1];
				iSlideBox[blank-1]=0;
				lastmove=LEFT;
				blank-=1;
				continue;
			}
			ranval--;
		}
		if(nextmove&RIGHT)
		{
			if(ranval==0)
			{
				iSlideBox[blank]=iSlideBox[blank+1];
				iSlideBox[blank+1]=0;
				lastmove=RIGHT;
				blank+=1;
				continue;
			}
			ranval--;
		}
	}
	return 1;
}


//Function for initializing the subblocks
int InitializeBlock(int num)
{
	iNumMove=0;
	if(num>5)
		return 0;
	for(int i=0;i<(num*num-1);i++)
		iSlideBox[i]=i+1;
	iSlideBox[num*num-1]=0;
	return 1;
}

//Function to check arrangement is over or not
int IsCompleted(int num)
{
	int i;
	for(i=0;i<(num*num-1);i++)
	{
		if(iSlideBox[i]!=(i+1))
			break;
	}
	if(i==(num*num-1)&&iSlideBox[num*num-1]==0)
		return 1;
	return 0;
}