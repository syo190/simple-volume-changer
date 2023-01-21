#ifndef UNICODE
#define UNICODE
#endif

#include<windows.h>
#include<mmsystem.h>
#include<endpointvolume.h>
#include<cmath>
#include<vector>
#include<array>
#include<fstream>

#include"..\inc\Slider.h"
#include"..\inc\ID.h"
#include"..\inc\SoundControler.h"
#include"..\inc\TaskTray.h"
#include"..\inc\EditBox.h"

namespace{
	const LPCWSTR ClassName = _T("VolumeChanger");
	const LPCWSTR WindowText = _T("Volume Changer");
	const LPCWSTR SettingClassName = _T("Shortcut Key Setting");
	const LPCWSTR SWindowText = _T("Shortcut Key Setting");
	const int AcceptInputInterval = 100;
	const int AcceptKeySetInputInterval = 100;
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
	const char ConfigFilePath[] = "./shorcutkey.bin";
	std::int32_t vkArray[IDC_SHORTCUTKEY_DLG_EDIT_DOWN3 - IDC_SHORTCUTKEY_DLG_EDIT_UP1 + 1] = {NULL};
	SoundControler sc;

	// Sliderのハンドラから該当するchを返す。該当するchが無い場合は-1を返す。
	int GetChannelFromSlider(HWND hwnd){
		for(std::size_t i = 0; i < sc.ChannelCount(); ++i){
			auto id = GetDlgCtrlID(hwnd);
			if(IDC_SLIDER + i == id) return i;
		}

		return -1;
	}

	// 成功すれば戻り値true
	bool WriteShortcutKey(const std::int32_t shortcutKey[], std::size_t length){
		std::ofstream ofs;
		ofs.open(ConfigFilePath, std::ios::out | std::ios::binary);
		if(!ofs) return false;

		ofs.write(reinterpret_cast<const char*>(shortcutKey), sizeof(std::int32_t) * length);
		if(ofs.fail()) return false;
		ofs.close();
		return true;
	}

	// 成功すれば戻り値true
	bool ReadShortcutKey(std::int32_t shortcutkey[], std::size_t length){
		std::ifstream ifs;
		ifs.open(ConfigFilePath, std::ios::in | std::ios::binary);
		if(!ifs) return false;

		ifs.read(reinterpret_cast<char*>(shortcutkey), sizeof(std::int32_t) * length);
		if(ifs.fail()) return false;
		ifs.close();
		return  true;
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
			int upDown = 0;
			int sum = 0;
			bool changeFlag = true;
			for(std::size_t i = 0; i < std::size(vkArray) / 2; ++i){
				sum += vkArray[i];
				if(vkArray[i] == NULL) continue;
				if(!GetAsyncKeyState(vkArray[i])){
					changeFlag = false;
					break;
				}
			}
			if(sum != 0 && changeFlag) upDown = 1;

			sum = 0;
			changeFlag = true;
			for(std::size_t i = std::size(vkArray) / 2; i < std::size(vkArray); ++i){
				sum += vkArray[i];
				if(vkArray[i] == NULL) continue;
				if(!GetAsyncKeyState(vkArray[i])){
					changeFlag = false;
					break;
				}
			}
			if(sum != 0 && changeFlag) upDown = -1;
			
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
				DestroyMenu(hMenu);

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

				// load preset shortcut key
				ReadShortcutKey(vkArray, std::size(vkArray));
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
				if(result == IDOK){
					// キー更新
					ReadShortcutKey(vkArray, std::size(vkArray));
				}
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
	static int init_x = 95;
	static int init_y = 12;
	static int offset_x = 40;
	static int offset_y = 55;
	static int edit_width = 84;
	static int edit_height = 25;
	static EditBox edt[IDC_SHORTCUTKEY_DLG_EDIT_DOWN3 - IDC_SHORTCUTKEY_DLG_EDIT_UP1 + 1];

	switch(uMsg){
		case WM_INITDIALOG:{
			HINSTANCE hInstance = HINSTANCE(GetWindowLongPtr(hwndDlg, GWLP_HINSTANCE));
			for(int id = IDC_SHORTCUTKEY_DLG_EDIT_UP1; id <= IDC_SHORTCUTKEY_DLG_EDIT_UP3; ++id){
				edt[id - IDC_SHORTCUTKEY_DLG_EDIT_UP1].Initialize(
					hwndDlg, hInstance,
					init_x + (edit_width + offset_x) * (id - IDC_SHORTCUTKEY_DLG_EDIT_UP1), init_y, edit_width, edit_height, id
				);
				edt[id - IDC_SHORTCUTKEY_DLG_EDIT_UP1].Create();
				edt[id - IDC_SHORTCUTKEY_DLG_EDIT_UP1].SetVK(vkArray[id - IDC_SHORTCUTKEY_DLG_EDIT_UP1]);
			}
			for(int id = IDC_SHORTCUTKEY_DLG_EDIT_DOWN1; id <= IDC_SHORTCUTKEY_DLG_EDIT_DOWN3; ++id){
				edt[id - IDC_SHORTCUTKEY_DLG_EDIT_UP1].Initialize(
					hwndDlg, hInstance,
					init_x + (edit_width + offset_x) * (id - IDC_SHORTCUTKEY_DLG_EDIT_DOWN1), init_y + offset_y, edit_width, edit_height, id
				);
				edt[id - IDC_SHORTCUTKEY_DLG_EDIT_UP1].Create();
				edt[id - IDC_SHORTCUTKEY_DLG_EDIT_UP1].SetVK(vkArray[id - IDC_SHORTCUTKEY_DLG_EDIT_UP1]);
			}
			return TRUE;
		}
		case WM_CLOSE:{
			EndDialog(hwndDlg, IDCANCEL);
			return TRUE;
		}
		case WM_COMMAND:{
			if(LOWORD(wParam) == IDC_SHORTCUTKEY_DLG_OK){
				EndDialog(hwndDlg, IDOK);
				std::int32_t shortcutKey[IDC_SHORTCUTKEY_DLG_EDIT_DOWN3 - IDC_SHORTCUTKEY_DLG_EDIT_UP1 + 1];
				for(std::size_t i = 0; i < std::size(shortcutKey); ++i){
					int vk;
					if(edt[i].GetVK(vk)) shortcutKey[i] = vk;
					else shortcutKey[i] = NULL;
				}
				WriteShortcutKey(shortcutKey, std::size(shortcutKey));
				return TRUE;
			}else if(LOWORD(wParam) == IDC_SHORTCUTKEY_DLG_CANCEL){
				EndDialog(hwndDlg, IDCANCEL);
				return TRUE;
			}
		}
		default:{
			break;
		}
	}

	// mean that this message is ignored
	return FALSE;
}

