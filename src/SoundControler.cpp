#ifndef UNICODE
#define UNICODE
#endif

#include"..\inc\SoundControler.h"
#include<Functiondiscoverykeys_devpkey.h>
#include<propvarutil.h>

/*
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
    for(std::size_t i = 0; i < this->m_ChannelCnt; ++i){
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
        PropVariantInit(&this->m_DeviceInfo[i]);
        result = pPropStore->GetValue(PKEY_Device_FriendlyName, &this->m_DeviceInfo[i]);
    }
};

SoundControler::~SoundControler(){
    // close COM library
    CoUninitialize();
}

// 成功すればTRUE, 失敗すればFALSEを返す
BOOL SoundControler::SetChannelVolume(float normalizedVol, UINT ch){
    if(ch >= this->ChannelCount()) return FALSE;

    static const float Delta = 1e-10f;
    if(normalizedVol < Delta) normalizedVol = 0.0f;
    if(normalizedVol > 1.0f - Delta) normalizedVol = 1.0f;
    auto result = this->m_aepVolume[ch]->SetMasterVolumeLevelScalar(normalizedVol, NULL);
    return result == S_OK;
}


// 成功すれば正規化された音量を返す, 失敗すれば0を返す
float SoundControler::GetChannelNormalizedVolume(UINT ch) const{
    if(ch >= this->ChannelCount()) return 0.0f;

    float normalizedVolume;
    this->m_aepVolume[ch]->GetMasterVolumeLevelScalar(&normalizedVolume);
    return normalizedVolume;
}

// デバイスの名前を返す。存在しないchの場合は空文字を返す
CString SoundControler::DeviceName(UINT ch) const{
    if(ch >= this->ChannelCount()) return CString(L"");
    
    static const int BufSize = 256;
    TCHAR deviceName[BufSize] = {0};
    auto result = PropVariantToString(this->m_DeviceInfo[ch], deviceName, BufSize);
    return CString(deviceName);
}

UINT SoundControler::ChannelCount() const{
    return this->m_ChannelCnt;
}