#include <string>
#include "stb_image.h"
#include "d3d11.h"

#include "IUnityInterface.h"
#include "IUnityGraphicsD3D11.h"

namespace UNT {

ID3D11Device *unity_device = nullptr;

// Unity plugin load event
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginLoad(IUnityInterfaces *unityInterfaces) {
    ID3D11DeviceContext *dxDeviceContext;
    unity_device = unityInterfaces->Get<IUnityGraphicsD3D11>()->GetDevice();
    // unity_device->GetImmediateContext(&dxDeviceContext);
}

extern "C" __declspec(dllexport) int test_call(int &val) {
    val += 123;
    return val;
}

extern "C" __declspec(dllexport)
    ID3D11ShaderResourceView *load_texture_from_path(const char *file_path,
                                                     int &width, int &height,
                                                     int &ret_code) {
    if (unity_device == nullptr) {
        ret_code = 1;
        return nullptr;
    }

    int nr_components;
    unsigned char *data =
        stbi_load(file_path, &width, &height, &nr_components, 0);

    if (data == nullptr) {
        ret_code = 2;
        return nullptr;
    }

    ID3D11Texture2D *tex = nullptr;
    ID3D11ShaderResourceView *shader_view = nullptr;

    D3D11_SUBRESOURCE_DATA init_data = {0};
    init_data.pSysMem = (const void *)data;
    init_data.SysMemPitch = width * 4;
    init_data.SysMemSlicePitch = width * height * 4;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;  // D3D11_USAGE_IMMUTABLE;
    desc.BindFlags =
        D3D11_BIND_SHADER_RESOURCE;  // |
                                     // D3D11_BIND_RENDER_TARGET;//D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = unity_device->CreateTexture2D(&desc, &init_data, &tex);

    stbi_image_free(data);

    if (hr != S_OK) {
        ret_code = 3;
        return nullptr;
    }
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
    ZeroMemory(&shader_resource_view_desc, sizeof(shader_resource_view_desc));

    shader_resource_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
    shader_resource_view_desc.Texture2D.MipLevels = 1;

    unity_device->CreateShaderResourceView(tex, &shader_resource_view_desc,
                                           &shader_view);

    if (hr != S_OK || shader_view == nullptr) {
        ret_code = 4;
        return nullptr;
    }

    tex->Release();

    return shader_view;
}

extern "C" __declspec(dllexport) void unload(void *native_tex) {
    ID3D11ShaderResourceView *srv = (ID3D11ShaderResourceView *)native_tex;
    if (srv) srv->Release();
}

}  // namespace UNT
