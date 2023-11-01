#include "native_texture_loader_dx11.h"
#include "d3d11.h"
#include "IUnityGraphics.h"

namespace UNT {

NativeTextureLoader *s_loader = nullptr;

// Unity plugin load event
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginLoad(IUnityInterfaces *unity_interfaces) {
    IUnityGraphics *graphics = unity_interfaces->Get<IUnityGraphics>();
    auto renderer = graphics->GetRenderer();
    switch (renderer) {
        case kUnityGfxRendererD3D11:
            s_loader = new NativeTextureLoaderDx11();
            break;
        default: break;
    }

    if (s_loader != nullptr) {
        s_loader->on_unity_plugin_load(unity_interfaces);
    }
}

// Unity似乎是在初次调用插件函数时触发“UnityPluginLoad”回调，此时若是异步调用“load_texture_from_path”会导致崩溃。
// 因此需要先同步调用一个插件函数，这里提供一个“无用”的“init”函数
extern "C" __declspec(dllexport) int init() { return 0; }

extern "C" __declspec(dllexport) void *load_texture_from_path(
    const char *file_path, int graphics_format, bool gen_mip, int &width,
    int &height, int &ret_code) {
    ret_code = 0;
    if (s_loader == nullptr) {
        ret_code = 1;
        return nullptr;
    }
    auto result = s_loader->load_texture_from_path(file_path, graphics_format,
                                                   gen_mip, width, height);
    ret_code = s_loader->get_err_code();
    return result;
}

extern "C" __declspec(dllexport) void unload(void *native_tex) {
    if (s_loader != nullptr) {
        s_loader->unload(native_tex);
    }
}

extern "C" __declspec(dllexport) void release() {
    if (s_loader != nullptr) {
        s_loader->release();
        delete s_loader;
        s_loader = nullptr;
    }
}

}  // namespace UNT
