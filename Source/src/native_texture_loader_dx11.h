#pragma once

#include "native_texture_looder.h"
#include "d3d11.h"
#include "IUnityGraphicsD3D11.h"

namespace UNT {

class NativeTextureLoaderDx11 : public NativeTextureLoader {
public:
    void on_unity_plugin_load(IUnityInterfaces* unity_interfaces) override;

    void* load_texture_from_path(const char* path, int graphics_format,
                                 bool gen_mip, int& width,
                                 int& height) override;

    void unload(void* native_ptr) override;

    void release() override;

private:
    ID3D11Device* unity_device_;
    ID3D11DeviceContext* unity_context_;
};

}  // namespace UNT