#pragma once
#include <cstddef>

struct ID3D11Device;
struct ID3D11ShaderResourceView;

HRESULT CreateShaderResourceViewFromMemory(ID3D11Device* device, const void* data, size_t size, ID3D11ShaderResourceView** out_srv);
