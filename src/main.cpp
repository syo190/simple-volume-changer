#ifndef UNICODE
#define UNICODE
#endif

#include<windows.h>
#include<mmsystem.h>
#include<endpointvolume.h>
#include<cmath>
#include<vector>

#include"..\inc\Slider.h"
#include"..\inc\ID.h"
#include"..\inc\SoundControler.h"
#include"..\inc\TaskTray.h"

namespace{
	const LPCWSTR ClassName = _T("VolumeChanger");
	const LPCWSTR WindowText = _T("Volume Changer");
	const LPCWSTR SettingClassName = _T("Shortcut Key Setting");
	const LPCWSTR SWindowText = _T("Shortcut Key Setting");
	const int AcceptInputInterval = 100;
	const int SliderWidth = 400;
	const int SliderHeight = 40;
	const int BlankSpace = 30;
	constexpr int MainWindowWidth = BlankSpace * 2 + SliderWidth;
	int MainWindowHeight = BlankSpace * 10;
	const int SliderMinVal = 0;
	const int SliderMaxVal = 100;
	const int RadioButtonSize = 20;
	const int ButtonHeight = 20;
	const int ButtonWidth = 100;
	const int LabelHeight = ButtonHeight;
	const int LabelWidth = SliderWidth;
	const int SWindowWidth = 300;
	const int SWindowHeight = 300;
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
BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
TaskTray tk;

int WINAPI WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow
	){	
	WNDCLASS wndC = {0};
	wndC.lpfnWndProc = WindowProc;
	wndC.hInstance = hInstance;
	wndC.lpszClassName = ClassName;
	wndC.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
	wndC.hIcon = HICON(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION)));

	if(!RegisterClass(&wndC)){
		return -1;
	}

	// Create Window
	MainWindowHeight = sc.ChannelCount() * (2 * BlankSpace + SliderHeight) + BlankSpace + ButtonHeight;
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
			HMENU hSysMenu = GetSystemMenu(hwnd, FALSE);
			RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);

			exDefaultStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

			// create button to set shortcut key
			CreateWindow(
				_T("BUTTON"), _T("shortcut key"),
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				BlankSpace, BlankSpace / 2, ButtonWidth, ButtonHeight,
				hwnd, HMENU(IDC_BUTTON), (LPCREATESTRUCT(lParam))->hInstance, NULL
			);

			// create slider control and set its position according to master sound volume
			for(std::size_t i = 0; i < sc.ChannelCount(); ++i){
				Slider slider(
					hwnd, (LPCREATESTRUCT(lParam))->hInstance,
					BlankSpace, BlankSpace * (i + 2) + ButtonHeight + SliderHeight * i, SliderWidth, SliderHeight, IDC_SLIDER + i
				);
				SendMessage(
					slider.GetHandler(), TBM_SETRANGE, WPARAM(true), MAKELPARAM(SliderMinVal, SliderMaxVal)
				);
				float normalizedVolume = sc.GetChannelNormalizedVolume(i);
				int volume = static_cast<int>(std::floor(normalizedVolume * (SliderMaxVal - SliderMinVal)));
				SendMessage(
					GetDlgItem(hwnd, IDC_SLIDER + i), TBM_SETPOS, WPARAM(1), LPARAM(volume)
				);

				// label explaining the slider
				LPCTSTR name = sc.DeviceName(i);
				CreateWindow(
					_T("STATIC"), name,
					WS_CHILD | WS_VISIBLE,
					BlankSpace * 2, BlankSpace * (i + 1) + ButtonHeight + SliderHeight * i, LabelWidth, LabelHeight,
					hwnd, HMENU(IDC_STATIC_CONTROL + i), (LPCREATESTRUCT(lParam))->hInstance, NULL
				);

				// radio button is according to master sound volume that is changed by shortcut key
				CreateWindow(
					_T("BUTTON"), _T(""),
					WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | (i == 0 ? WS_GROUP: NULL),
					BlankSpace, BlankSpace * (i + 1) + ButtonHeight + SliderHeight * i, RadioButtonSize, RadioButtonSize,
					hwnd, HMENU(IDC_RADIO_BUTTON + i), (LPCREATESTRUCT(lParam))->hInstance, NULL
				);
			}

			shortcutKeyValidCh = 0;
			SendMessage(GetDlgItem(hwnd, IDC_RADIO_BUTTON + shortcutKeyValidCh),BM_SETCHECK, BST_CHECKED, WPARAM(NULL));
			SetTimer(hwnd, IDT_ACCEPT_INPUT, AcceptInputInterval, TIMERPROC(NULL));
			break;
		}
		case WM_COMMAND:{
			if(LOWORD(wParam) == IDC_BUTTON){
				// do modal window
				auto result = DialogBox(
					HINSTANCE(GetWindowLongPtr(hwnd, GWL_HINSTANCE)), LPCTSTR(IDI_SHORTCUTKEY_DLG),
					hwnd, DialogProc
				);
			}else{
				for(std::size_t i = 0; i < sc.ChannelCount(); ++i){
					if(LOWORD(wParam) == IDC_RADIO_BUTTON + i){
						shortcutKeyValidCh = i;
						break;
					}
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


BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){

	static const int detectedKeyList[] = {
		VK_SHIFT, VK_CONTROL, VK_MENU, VK_PAUSE, VK_CAPITAL, VK_ESCAPE, VK_SPACE, VK_PRIOR, VK_NEXT,
		VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, // 0 ~ 9
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, // A ~ J
		0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, // K ~ T
		0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, // U ~ Z
		VK_LWIN, VK_RWIN, VK_APPS,
		VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, // numpad 0 ~ 9
		VK_MULTIPLY, VK_ADD, VK_SEPARATOR, VK_SUBTRACT,
		VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12,
		VK_NUMLOCK, VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL, VK_LMENU, VK_RMENU 
	};
	static const LPCTSTR detectedKeyNameList[] = {
		_T("Shift"), _T("Control"), _T("Alt"), _T("Pause"), _T("CapsLock"), _T("Esc"), _T("Space"), _T("PageUp"), _T("PageDown"),
		_T("Left"), _T("Up"), _T("Right"), _T("Down"),
		_T("0"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9"),
		_T("A"), _T("B"), _T("C"), _T("D"), _T("E"), _T("F"), _T("G"), _T("H"), _T("I"), _T("J"),
		_T("K"), _T("L"), _T("M"), _T("N"), _T("O"), _T("P"), _T("Q"), _T("R"), _T("S"), _T("T"),
		_T("U"), _T("V"), _T("W"), _T("X"), _T("Y"), _T("Z"),
		_T("LeftWin"), _T("RightWin"), _T("App"),
		_T("Pad0"), _T("Pad1"), _T("Pad2"), _T("Pad3"), _T("Pad4"), _T("Pad5"), _T("Pad6"), _T("Pad7"), _T("Pad8"), _T("Pad9"),
		_T("Multiply"), _T("Add"), _T("Divide"), _T("Subtract"),
		_T("F1"), _T("F2"), _T("F3"), _T("F4"), _T("F5"), _T("F6"), _T("F7"), _T("F8"), _T("F9"), _T("F10"), _T("F11"), _T("F12"),
		_T("NumLock"), _T("LeftShift"), _T("RightShift"), _T("LeftControl"), _T("RightControl"), _T("LeftAlt"), _T("RightAlt")   
	};
	static const std::size_t keyCnt = std::size(detectedKeyList);

	switch(uMsg){
		case WM_CLOSE:{
			EndDialog(hwndDlg, IDCANCEL);
			return TRUE;
		}
		case WM_COMMAND:{
			if(LOWORD(wParam) == IDC_SHORTCUTKEY_DLG_OK){
				EndDialog(hwndDlg, IDOK);
				return TRUE;
			}else if(LOWORD(wParam) == IDC_SHORTCUTKEY_DLG_CANCEL){
				EndDialog(hwndDlg, IDCANCEL);
				return TRUE;
			}else if(IDC_SHORTCUTKEY_DLG_EDIT_UP1 <= LOWORD(wParam)
			&& LOWORD(wParam) <= IDC_SHORTCUTKEY_DLG_EDIT_DOWN3){
				if(HIWORD(wParam) != EN_SETFOCUS) return FALSE;

				// フォーカスが当たっていることの確認
				int focusedEditID = -1;
				HWND focusedWnd = GetFocus();
				for(int id = IDC_SHORTCUTKEY_DLG_EDIT_UP1; id <= IDC_SHORTCUTKEY_DLG_EDIT_DOWN3; ++id){
					if(focusedWnd == GetDlgItem(hwndDlg, id)){
						focusedEditID = id;
						break;
					}
				}
				if(focusedEditID == -1) ATLASSERT(_T("ERROR"));

				HDC hdc = GetDC(focusedWnd);
				SetBkMode(hdc, TRANSPARENT);
				ReleaseDC(focusedWnd, hdc);
			}

			break;
		}
		case WM_CTLCOLOREDIT:{
			SetBkMode(HDC(wParam), TRANSPARENT);
			SetTextColor(HDC(wParam), RGB(0, 0, 256));
			return BOOL(GetStockObject(BLACK_BRUSH));
		}
		default:{
			break;
		}
	}

	// mean that this message is ignored
	return FALSE;
}

