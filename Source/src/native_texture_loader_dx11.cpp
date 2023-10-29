#include "native_texture_loader_dx11.h"
#include "stb_image.h"

namespace UNT {

void NativeTextureLoaderDx11::on_unity_plugin_load(
    IUnityInterfaces *unity_interfaces) {
    ID3D11DeviceContext *dxDeviceContext;
    unity_device_ = unity_interfaces->Get<IUnityGraphicsD3D11>()->GetDevice();
}

void *NativeTextureLoaderDx11::load_texture_from_path(const char *path,
                                                      int &width, int &height) {
    if (unity_device_ == nullptr) {
        err_code_ = 2;
        return nullptr;
    }

    stbi_set_flip_vertically_on_load(1);
    int nr_components;
    unsigned char *data = stbi_load(path, &width, &height, &nr_components, 4);

    if (data == nullptr) {
        err_code_ = 3;
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
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;  // D3D11_USAGE_IMMUTABLE;
    desc.BindFlags =
        D3D11_BIND_SHADER_RESOURCE;  // |
                                     // D3D11_BIND_RENDER_TARGET;//D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = unity_device_->CreateTexture2D(&desc, &init_data, &tex);

    stbi_image_free(data);

    if (hr != S_OK) {
        err_code_ = 4;
        return nullptr;
    }
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
    ZeroMemory(&shader_resource_view_desc, sizeof(shader_resource_view_desc));

    shader_resource_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
    shader_resource_view_desc.Texture2D.MipLevels = 1;

    unity_device_->CreateShaderResourceView(tex, &shader_resource_view_desc,
                                            &shader_view);

    if (hr != S_OK || shader_view == nullptr) {
        err_code_ = 5;
        return nullptr;
    }

    tex->Release();

    return shader_view;
}

void NativeTextureLoaderDx11::unload(void *native_ptr) {
    ID3D11ShaderResourceView *srv = (ID3D11ShaderResourceView *)native_ptr;
    if (srv) srv->Release();
}

void NativeTextureLoaderDx11::release() {}

}  // namespace UNT