#include"..\inc\SoundControler.h"
#include<Functiondiscoverykeys_devpkey.h>
#include<propvarutil.h>

/*
２．
常駐にしてショートカットの選択肢を与える

３．
デバイス名のラベルを付与する

４．
デバイス数に応じてウィンドウサイズを変更して開く

５．
ショートカットキーを選択できるようにする
*/

SoundControler::SoundControler(): m_ChannelCnt(0){
    CComPtr<IMMDeviceEnumerator> pMMDeviceEnu;
    CComPtr<IMMDeviceCollection> pMMDeviceCollection;
    auto result = CoInitializeEx(NULL, COINIT::COINIT_MULTITHREADED);
    result = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator),
        (void**)(&pMMDeviceEnu)
    );
    result = pMMDeviceEnu->EnumAudioEndpoints(
        EDataFlow::eRender,
        DEVICE_STATE_ACTIVE,
        &pMMDeviceCollection
    );
    
    result = pMMDeviceCollection->GetCount(&this->m_ChannelCnt);
    this->m_aepVolume.resize(this->m_ChannelCnt);
    this->m_DeviceInfo.resize(this->m_ChannelCnt);
    for(int i = 0; i < this->m_ChannelCnt; ++i){
        CComPtr<IMMDevice> pMMDevice;
        result = pMMDeviceCollection->Item(i, &pMMDevice);
        result = pMMDevice->Activate(
            __uuidof(IAudioEndpointVolume),
            CLSCTX_INPROC_SERVER,
            NULL,
            (void**)(&this->m_aepVolume[i])
        );
        CComPtr<IPropertyStore> pPropStore;
        result = pMMDevice->OpenPropertyStore(STGM_READ, &pPropStore);
        PROPVARIANT propMMDevice;
        PropVariantInit(&propMMDevice);
        result = pPropStore->GetValue(PKEY_Device_FriendlyName, &propMMDevice);
    }
};

SoundControler::~SoundControler(){
    // close COM library
    CoUninitialize();
}

// 成功すればTRUE, 失敗すればFALSEを返す
BOOL SoundControler::SetChannelVolume(float normalizedVol, UINT ch){
    if(ch >= this->ChanngelCount()) return FALSE;

    static const float Delta = 1e-10f;
    if(normalizedVol < Delta) normalizedVol = 0.0f;
    if(normalizedVol > 1.0f - Delta) normalizedVol = 1.0f;
    auto result = this->m_aepVolume[ch]->SetMasterVolumeLevelScalar(normalizedVol, NULL);
    return result == S_OK;
}


// 成功すれば正規化された音量を返す, 失敗すれば0を返す
float SoundControler::GetChannelNormalizedVolume(UINT ch) const{
    if(ch >= this->ChanngelCount()) return 0.0f;

    float normalizedVolume;
    this->m_aepVolume[ch]->GetMasterVolumeLevelScalar(&normalizedVolume);
    return normalizedVolume;
}

// デバイスの名前を返す。存在しないchの場合は空文字を返す
CString SoundControler::DeviceName(UINT ch) const{
    if(ch >= this->ChanngelCount()) return CString(L"");
    
    static const int BufSize = 256;
    WCHAR deviceName[BufSize] = {0};
    PropVariantToString(this->m_DeviceInfo[ch], deviceName, BufSize);
    return CString(deviceName);
}

UINT SoundControler::ChanngelCount() const{
    return this->m_ChannelCnt;
}