using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.UI;

public class TestLoadTexture : MonoBehaviour
{
    public string TexturePath;

    public RawImage TexUI;

    private Texture2D _tex2d;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private IntPtr _texPtr;

    public void Test()
    {
        int val = 123;
        Debug.Log($"{test_call(ref val)}");
    }

    public void DXNativeLoadTex()
    {
        int w=0, h=0,ret = 0;
        _texPtr = load_texture_from_path(TexturePath, ref w, ref h,ref ret);
        Debug.Log($"load tex {ret}");
        if (ret != 0 || _texPtr == IntPtr.Zero)
        {
            Debug.LogError("load tex failed!");
            return;
        }

        _tex2d = Texture2D.CreateExternalTexture(w, h, TextureFormat.RGBA32, false, false, _texPtr);
        TexUI.texture = _tex2d;
    }

    public void DXNativeUnloadTex()
    {
        if(_texPtr != IntPtr.Zero)
        {
            unload(_texPtr);
            Debug.Log("unload ok");
        }
        else
        {
            Debug.LogError("texptr is nullptr");
        }
    }


    [DllImport("unity_native_texture_loader")]
    extern static IntPtr load_texture_from_path(string path,ref int width, ref int height,ref int ret);

    [DllImport("unity_native_texture_loader")]
    extern static void unload(IntPtr native_tex);


    [DllImport("unity_native_texture_loader")]
    extern static int test_call(ref int val);
}
