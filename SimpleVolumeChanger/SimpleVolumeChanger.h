﻿
// SimpleVolumeChanger.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CSimpleVolumeChangerApp:
// このクラスの実装については、SimpleVolumeChanger.cpp を参照してください
//

class CSimpleVolumeChangerApp : public CWinApp
{
public:
	CSimpleVolumeChangerApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CSimpleVolumeChangerApp theApp;
