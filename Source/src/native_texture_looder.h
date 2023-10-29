#pragma once

namespace UNT {

class NativeTextureLoader {
public:
    virtual void on_unity_plugin_load() = 0;
};

}  // namespace UNT