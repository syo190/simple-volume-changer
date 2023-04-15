#include "pch.h"
#include "SoundControler.h"
#include <Functiondiscoverykeys_devpkey.h>
#include <propvarutil.h>
#pragma comment(lib, "Propsys.lib")

SoundControler::SoundControler(): m_ChannelCnt(0){
    CComPtr<IMMDeviceCollection> pMMDeviceCollection;
    auto result = CoInitializeEx(NULL, COINIT::COINIT_MULTITHREADED);
    result = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator),
        (void**)(&m_pMMDeviceEnu)
    );
    result = m_pMMDeviceEnu->EnumAudioEndpoints(
        EDataFlow::eRender,
        DEVICE_STATE_ACTIVE,
        &pMMDeviceCollection
    );

    result = pMMDeviceCollection->GetCount(&this->m_ChannelCnt);
    this->m_aepVolume.resize(this->m_ChannelCnt);
    this->m_DeviceInfo.resize(this->m_ChannelCnt);
    for(std::size_t i = 0; i < this->m_ChannelCnt; ++i){
        CComPtr<IMMDevice> pMMDevice;
        result = pMMDeviceCollection->Item(static_cast<UINT>(i), &pMMDevice);
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
BOOL SoundControler::SetChannelVolume(float normalizedVol, std::size_t ch){
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
    if(ch >= this->ChannelCount()) return _T("");
    
    return CString(this->m_DeviceInfo[ch].pwszVal);
}

// ch数を返す
UINT SoundControler::ChannelCount() const{
    return this->m_ChannelCnt;
}

// 現在の出力チャネルを返す
UINT SoundControler::GetCurrentCannek() const {
    CComPtr<IMMDevice> device;
    auto a = m_pMMDeviceEnu->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &device);
    CComPtr<IPropertyStore> propertyStore;
    device->OpenPropertyStore(STGM_READ, &propertyStore);
    PROPVARIANT p;
    PropVariantInit(&p);
    propertyStore->GetValue(PKEY_Device_FriendlyName, &p);

    for (std::size_t i = 0; i < m_DeviceInfo.size(); ++i) {
        if (_tcsicmp(m_DeviceInfo[i].pwszVal, p.pwszVal) == 0) {
            return static_cast<UINT>(i);
        }
    }

    return 0;
}