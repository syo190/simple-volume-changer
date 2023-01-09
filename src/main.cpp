#ifndef UNICODE
#define UNICODE
#endif

#include<windows.h>
#include<mmsystem.h>
#include<endpointvolume.h>

#include"..\inc\Slider.h"
#include"..\inc\ID.h"
#include"..\inc\SoundControler.h"

namespace{
	const LPCWSTR ClassName = TEXT("VolumeChanger");
	const LPCWSTR WindowText = TEXT("Volume Changer");
	const int AcceptInputInterval = 100;
	const int SliderWidth = 300;
	const int SliderHeight = 40;
	const int BlankSpace = 30;
	constexpr int MainWindowWidth = BlankSpace * 2 + SliderWidth;
	constexpr int MainWindowHeight = BlankSpace * 5;
	const int SliderMinVal = 0;
	const int SliderMaxVal = 100;
	SoundControler sc;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow
	){	
	WNDCLASS wndC = {};
	wndC.lpfnWndProc = WindowProc;
	wndC.hInstance = hInstance;
	wndC.lpszClassName = ClassName;
	wndC.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);

	if(!RegisterClass(&wndC)){
		return -1;
	}

	// Create Window
	HWND hwnd = CreateWindowEx(
		0, // optional window style
		ClassName, WindowText,
// 		WS_OVERLAPPEDWINDOW | WS_VISIBLE, //window style
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
			// 音量の変更を行う
			// wparamには変更後の音量が格納されている
			float normalizedVolume = float(wParam) / float(SliderMaxVal - SliderMinVal);
			sc.SetChannelVolume(normalizedVolume, 1);
			break;
		}
		case WM_PAINT:{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_HSCROLL:{
			int volume = HIWORD(wParam);
			SendMessage(hwnd, WM_UPDATE_VOLUME, WPARAM(volume), LPARAM(NULL));
			break;
		}
		case WM_TIMER:{
			int nowVolume = SendMessage(
				GetDlgItem(hwnd, IDC_SLIDER), TBM_GETPOS, WPARAM(NULL), LPARAM(NULL)
			);
			int upDown = 0;
			if(GetAsyncKeyState(VK_F8) & 0x01) upDown = -1;
			else if(GetAsyncKeyState(VK_F9) & 0x01) upDown = 1;
			SendMessage(
				GetDlgItem(hwnd, IDC_SLIDER), TBM_SETPOS, WPARAM(1), LPARAM(nowVolume + upDown)
			);
			if(upDown != 0) SendMessage(hwnd, WM_UPDATE_VOLUME, WPARAM(nowVolume + upDown), LPARAM(NULL));
			break;
		}
		case WM_CREATE:{
			Slider slider(
				hwnd, (LPCREATESTRUCT(lParam))->hInstance,
				BlankSpace, BlankSpace, SliderWidth, SliderHeight, IDC_SLIDER
			);
			SendMessage(
				slider.GetHandler(), TBM_SETRANGE, WPARAM(true), MAKELPARAM(SliderMinVal, SliderMaxVal)
			);	

			SetTimer(hwnd, IDT_ACCEPT_INPUT, AcceptInputInterval, TIMERPROC(NULL));
			break;
		}
		default:{
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
