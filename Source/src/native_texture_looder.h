#pragma once

#include "IUnityInterface.h"

namespace UNT {

class NativeTextureLoader {
public:
    virtual ~NativeTextureLoader() = default;

    virtual void on_unity_plugin_load(IUnityInterfaces* unity_interfaces) = 0;

    virtual void* load_texture_from_path(const char* path, int& width,
                                         int& height) = 0;

    virtual void unload(void* native_ptr) = 0;

    virtual void release() = 0;

    int get_err_code() const { return err_code_; }

protected:
    int err_code_ = 0;
};

}  // namespace UNT