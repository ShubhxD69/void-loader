#include <vector>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <wincodec.h>
#include <d3d11.h>
#include <Shlwapi.h>

#include "wic_texture_loader.h"

#pragma comment(lib, "windowscodecs.lib")

HRESULT CreateShaderResourceViewFromMemory(ID3D11Device* device, const void* data, size_t size, ID3D11ShaderResourceView** out_srv)
{
    if (!device || !data || !size || !out_srv)
        return E_INVALIDARG;
    *out_srv = nullptr;

    HRESULT com_init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    IWICImagingFactory* factory = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
    if (FAILED(hr))
    {
        if (com_init == S_OK)
            CoUninitialize();
        return hr;
    }

    IStream* stream = SHCreateMemStream(static_cast<const BYTE*>(data), static_cast<UINT>(size));
    if (!stream)
    {
        factory->Release();
        if (com_init == S_OK)
            CoUninitialize();
        return E_OUTOFMEMORY;
    }

    IWICBitmapDecoder* decoder = nullptr;
    hr = factory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnLoad, &decoder);
    stream->Release();
    if (FAILED(hr))
    {
        factory->Release();
        if (com_init == S_OK)
            CoUninitialize();
        return hr;
    }

    IWICBitmapFrameDecode* frame = nullptr;
    hr = decoder->GetFrame(0, &frame);
    decoder->Release();
    if (FAILED(hr))
    {
        factory->Release();
        if (com_init == S_OK)
            CoUninitialize();
        return hr;
    }

    IWICFormatConverter* converter = nullptr;
    hr = factory->CreateFormatConverter(&converter);
    if (FAILED(hr))
    {
        frame->Release();
        factory->Release();
        if (com_init == S_OK)
            CoUninitialize();
        return hr;
    }

    hr = converter->Initialize(frame, GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeMedianCut);
    frame->Release();
    if (FAILED(hr))
    {
        converter->Release();
        factory->Release();
        if (com_init == S_OK)
            CoUninitialize();
        return hr;
    }

    UINT width = 0, height = 0;
    converter->GetSize(&width, &height);
    const UINT stride = width * 4;
    const UINT buf_size = stride * height;
    std::vector<BYTE> pixels(buf_size);
    hr = converter->CopyPixels(nullptr, stride, buf_size, pixels.data());
    converter->Release();
    factory->Release();
    if (FAILED(hr))
    {
        if (com_init == S_OK)
            CoUninitialize();
        return hr;
    }

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA sub = {};
    sub.pSysMem = pixels.data();
    sub.SysMemPitch = stride;

    ID3D11Texture2D* tex = nullptr;
    hr = device->CreateTexture2D(&desc, &sub, &tex);
    if (FAILED(hr))
    {
        if (com_init == S_OK)
            CoUninitialize();
        return hr;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(tex, &srv_desc, out_srv);
    tex->Release();

    if (com_init == S_OK)
        CoUninitialize();
    return hr;
}
