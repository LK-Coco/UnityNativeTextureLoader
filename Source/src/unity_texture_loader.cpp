#include "native_texture_loader_dx11.h"
#include "d3d11.h"
#include "IUnityGraphics.h"

namespace UNT {

NativeTextureLoader *loader = nullptr;

// Unity plugin load event
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginLoad(IUnityInterfaces *unity_interfaces) {
    IUnityGraphics *graphics = unity_interfaces->Get<IUnityGraphics>();
    auto renderer = graphics->GetRenderer();
    switch (renderer) {
        case kUnityGfxRendererD3D11:
            loader = new NativeTextureLoaderDx11();
            break;
        default: break;
    }

    if (loader != nullptr) {
        loader->on_unity_plugin_load(unity_interfaces);
    }
}

extern "C" __declspec(dllexport) void *load_texture_from_path(
    const char *file_path, int graphics_format, bool gen_mip, int &width,
    int &height, int &ret_code) {
    ret_code = 0;
    if (loader == nullptr) {
        ret_code = 1;
        return nullptr;
    }
    auto result = loader->load_texture_from_path(file_path, graphics_format,
                                                 gen_mip, width, height);
    ret_code = loader->get_err_code();
    return result;
}

extern "C" __declspec(dllexport) void unload(void *native_tex) {
    if (loader != nullptr) {
        loader->unload(native_tex);
    }
}

extern "C" __declspec(dllexport) void release() {
    if (loader != nullptr) {
        loader->release();
        delete loader;
        loader = nullptr;
    }
}

}  // namespace UNT
