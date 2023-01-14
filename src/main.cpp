#ifndef UNICODE
#define UNICODE
#endif

#include<windows.h>
#include<mmsystem.h>
#include<endpointvolume.h>
#include<cmath>

#include"..\inc\Slider.h"
#include"..\inc\ID.h"
#include"..\inc\SoundControler.h"
#include"..\inc\TaskTray.h"

namespace{
	const LPCWSTR ClassName = TEXT("VolumeChanger");
	const LPCWSTR WindowText = TEXT("Volume Changer");
	const int AcceptInputInterval = 100;
	const int SliderWidth = 400;
	const int SliderHeight = 40;
	const int BlankSpace = 30;
	constexpr int MainWindowWidth = BlankSpace * 2 + SliderWidth;
	int MainWindowHeight = BlankSpace * 10;
	const int SliderMinVal = 0;
	const int SliderMaxVal = 100;
	const int RadioButtonSize = 20;
	LONG exDefaultStyle;
	UINT shortcutKeyValidCh = 0;
	SoundControler sc;

	// Sliderのハンドラから該当するchを返す。該当するchが無い場合は-1を返す。
	int GetChannelFromSlider(HWND hwnd){
		for(std::size_t i = 0; i < sc.ChannelCount(); ++i){
			auto id = GetDlgCtrlID(hwnd);
			if(IDC_SLIDER + i == id) return i;
		}

		return -1;
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
TaskTray tk;

int WINAPI WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow
	){	
	WNDCLASS wndC = {};
	wndC.lpfnWndProc = WindowProc;
	wndC.hInstance = hInstance;
	wndC.lpszClassName = ClassName;
	wndC.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
	wndC.hIcon = HICON(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION)));

	if(!RegisterClass(&wndC)){
		return -1;
	}

	// Create Window
	MainWindowHeight = sc.ChannelCount() * (2 * BlankSpace + SliderHeight) + BlankSpace;
	HWND hwnd = CreateWindowEx(
		0, // optional window style
		ClassName, WindowText,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, MainWindowWidth, MainWindowHeight,
		NULL, // parent window
		NULL, // menu
		hInstance, // instance handle
		NULL  // additional application data
	);

	if(hwnd == NULL){
		MessageBox(NULL, TEXT("Something wrong"), TEXT("Error"), MB_OK);
		return -1;
	}

	tk.Initialize(hInstance, hwnd, IDI_TASK_TRAY, WM_TASK_TRAY_CLIKED, HICON(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION))));

	// run message loop
    MSG msg = {};
    while(BOOL bRet = GetMessage(&msg, NULL, 0, 0) != 0){
		if(bRet == -1){
			MessageBox(NULL, TEXT("Something wrong"), TEXT("Error"), MB_OK);
			return -1;
		}else{
        	TranslateMessage(&msg);
      	 	DispatchMessage(&msg);
		}
    }

	return 0;
}

// window procedure of main window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

	switch(uMsg){
		case WM_DESTROY:{
			KillTimer(hwnd, IDT_ACCEPT_INPUT);
			PostQuitMessage(0);
			return 0;
		}
		case WM_UPDATE_VOLUME:{
			// 音量の更新を行うメッセージ
			// wParam：変更後の音量、lParam：音量変更先デバイスch
			float normalizedVolume = float(wParam) / float(SliderMaxVal - SliderMinVal);
			sc.SetChannelVolume(normalizedVolume, UINT(lParam));
			break;
		}
		case WM_PAINT:{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_HSCROLL:{
			int nowVolume = SendMessage(
				HWND(lParam), TBM_GETPOS, WPARAM(NULL), LPARAM(NULL)
			);
			SendMessage(
				hwnd, WM_UPDATE_VOLUME, WPARAM(nowVolume), GetChannelFromSlider(HWND(lParam))
			);
			break;
		}
		case WM_TIMER:{
			if(!GetAsyncKeyState(VK_CONTROL)) break;
			int upDown = 0;
			if(GetAsyncKeyState(VK_F8) & 0x01) upDown = -1;
			else if(GetAsyncKeyState(VK_F9) & 0x01) upDown = 1;
			
			int nowVolume = SendMessage(
				GetDlgItem(hwnd, IDC_SLIDER + shortcutKeyValidCh), TBM_GETPOS, WPARAM(NULL), LPARAM(NULL)
			);
			int newVolume = nowVolume;
			if(nowVolume + upDown >= SliderMinVal && nowVolume + upDown <= SliderMaxVal) newVolume += upDown; 
			SendMessage(
				GetDlgItem(hwnd, IDC_SLIDER + shortcutKeyValidCh), TBM_SETPOS, WPARAM(1), LPARAM(newVolume)
			);
			SendMessage(hwnd, WM_UPDATE_VOLUME, WPARAM(newVolume), LPARAM(shortcutKeyValidCh));
			break;
		}
		case WM_TASK_TRAY_CLIKED:{
			auto id = (UINT)(lParam);
			if(id == WM_LBUTTONUP){
				int a = 43;
			}
			else if(id == WM_RBUTTONUP){
				POINT pt = {0};
				GetCursorPos(&pt);

				tk.SetForegroundDummyWindow();
				HMENU hMenu = CreatePopupMenu();
				InsertMenu(
					hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_OPEN_FROM_TASK_TRAY, _T("VoiceChanger") 
				);
				InsertMenu(
					hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_CLOSE_FROM_TASK_TRAY, _T("close") 
				);
				auto clikedID = TrackPopupMenu(
					hMenu,
					TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON,
					pt.x, pt.y, 0,
					hwnd, NULL
				);

				if(clikedID == IDM_CLOSE_FROM_TASK_TRAY){
					SendMessage(hwnd, WM_CLOSE, WPARAM(NULL), LPARAM(NULL));
				}else if(clikedID == IDM_OPEN_FROM_TASK_TRAY){
					// タスクバー通知領域 -> タスクバーへ移動
					SetWindowLongPtr(hwnd, GWL_EXSTYLE, exDefaultStyle);
					ShowWindow(hwnd, SW_SHOWNORMAL);
					tk.Destroy();
				}
			}
			break;
		}
		case WM_SYSCOMMAND:{
			if(wParam == SC_MINIMIZE){
				// タスクバー -> タスクバー通知領域へ移動
				tk.Create();
				auto exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
				exStyle = (exStyle & ~WS_EX_APPWINDOW) | WS_EX_TOOLWINDOW;
				SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
			}
			break;
		}
		case WM_CREATE:{
			exDefaultStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

			// create slider control and set its position according to master sound volume
			for(std::size_t i = 0; i < sc.ChannelCount(); ++i){
				Slider slider(
					hwnd, (LPCREATESTRUCT(lParam))->hInstance,
					BlankSpace, BlankSpace * (i + 2) + SliderHeight * i, SliderWidth, SliderHeight, IDC_SLIDER + i
				);
				SendMessage(
					slider.GetHandler(), TBM_SETRANGE, WPARAM(true), MAKELPARAM(SliderMinVal, SliderMaxVal)
				);
				float normalizedVolume = sc.GetChannelNormalizedVolume(i);
				int volume = static_cast<int>(std::floor(normalizedVolume * (SliderMaxVal - SliderMinVal)));
				SendMessage(
					GetDlgItem(hwnd, IDC_SLIDER + i), TBM_SETPOS, WPARAM(1), LPARAM(volume)
				);

				// explanation of the slider
				LPCTSTR name = sc.DeviceName(i);
				CreateWindow(
					_T("STATIC"), name,
					WS_CHILD | WS_VISIBLE,
					BlankSpace * 2, BlankSpace * (i + 1) + SliderHeight * i, SliderWidth, BlankSpace,
					hwnd, HMENU(IDC_STATIC_CONTROL + i), (LPCREATESTRUCT(lParam))->hInstance, NULL
				);

				// radio button is according to master sound volume that is changed by shortcut key
				CreateWindow(
					_T("BUTTON"), _T(""),
					WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | (i == 0 ? WS_GROUP: NULL),
					BlankSpace, BlankSpace * (i + 1) + SliderHeight * i, RadioButtonSize, RadioButtonSize,
					hwnd, HMENU(IDC_RADIO_BUTTON + i), (LPCREATESTRUCT(lParam))->hInstance, NULL
				);
			}

			shortcutKeyValidCh = 0;
			SendMessage(GetDlgItem(hwnd, IDC_RADIO_BUTTON + shortcutKeyValidCh),BM_SETCHECK, BST_CHECKED, WPARAM(NULL));
			SetTimer(hwnd, IDT_ACCEPT_INPUT, AcceptInputInterval, TIMERPROC(NULL));
			break;
		}
		case WM_COMMAND:{
			for(std::size_t i = 0; i < sc.ChannelCount(); ++i){
				if(LOWORD(wParam) == IDC_RADIO_BUTTON + i){
					shortcutKeyValidCh = i;
					break;
				}
			}
			break;
		}
		case WM_CLOSE:{
			DestroyWindow(hwnd);
			break;
		}
		default:{
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
