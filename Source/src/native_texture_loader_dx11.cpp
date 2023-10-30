#include "native_texture_loader_dx11.h"
#include <d3d11.h>
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

    UINT stride = width * 4;
    UINT buf_size = stride * height;

    // D3D11_SUBRESOURCE_DATA init_data = {0};
    // init_data.pSysMem = (const void *)data;
    // init_data.SysMemPitch = stride;
    // init_data.SysMemSlicePitch = buf_size;

    D3D11_TEXTURE2D_DESC tex_desc = {};
    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.MipLevels = 0;
    tex_desc.ArraySize = 1;
    tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage = D3D11_USAGE_DEFAULT;  // D3D11_USAGE_IMMUTABLE;
    tex_desc.BindFlags =
        D3D11_BIND_SHADER_RESOURCE |
        D3D11_BIND_RENDER_TARGET;  // D3D11_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags = 0;
    tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    ID3D11Texture2D *tex = nullptr;
    HRESULT hr = unity_device_->CreateTexture2D(&tex_desc, nullptr, &tex);

    if (FAILED(hr)) {
        err_code_ = 4;
        stbi_image_free(data);
        return nullptr;
    }
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    // shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = -1;

    ID3D11ShaderResourceView *tex_view = nullptr;
    hr = unity_device_->CreateShaderResourceView(tex, &srv_desc, &tex_view);
    if (FAILED(hr)) {
        err_code_ = 5;
        stbi_image_free(data);
        return nullptr;
    }

    ID3D11DeviceContext *content;
    unity_device_->CreateDeferredContext(0, &content);

    content->UpdateSubresource(tex, 0, nullptr, data, stride, buf_size);
    content->GenerateMips(tex_view);

    stbi_image_free(data);

    // https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-textures-how-to

    // tex->Release();

    return tex_view;
}

void NativeTextureLoaderDx11::unload(void *native_ptr) {
    ID3D11ShaderResourceView *srv = (ID3D11ShaderResourceView *)native_ptr;
    if (srv) srv->Release();
}

void NativeTextureLoaderDx11::release() {}

}  // namespace UNT