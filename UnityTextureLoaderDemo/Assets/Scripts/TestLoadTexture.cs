﻿using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.UI;
using System.IO;

public class TestLoadTexture : MonoBehaviour
{
    public int index = 0;
    public List<string> TexturePath;

    public Transform Cube;

    public RawImage TexUI_normal;
    public RawImage TexUI_native;

    private Texture2D _tex2d_normal;
    private Texture2D _tex2d_native;

    private string _path => Application.dataPath + "/" + TexturePath[index];

    // Start is called before the first frame update
    void Start()
    {
       
    }

    // Update is called once per frame
    void Update()
    {
        var rot = Cube.localEulerAngles;
        rot.y += 1;
        Cube.localEulerAngles = rot;    
    }

    public async void NormalLoadTex()
    {
        using FileStream fs = new FileStream(_path, FileMode.Open);
        byte[] buffer = new byte[fs.Length];
        await fs.ReadAsync(buffer, 0, buffer.Length);

        _tex2d_normal = new Texture2D(1920,1080);
        _tex2d_normal.LoadImage(buffer);
        _tex2d_normal.Apply();

        TexUI_normal.texture = _tex2d_normal;
    }

    public void NormalUnloadTex()
    {
        Destroy(_tex2d_normal);
        _tex2d_normal = null;
    }


    private IntPtr _texPtr;

    public void DXNativeLoadTex()
    {
        int w=0, h=0,ret =0;
        _texPtr = load_texture_from_path(_path, ref w, ref h,ref ret);
        Debug.Log($"load tex code:{ret}");
        if (_texPtr == IntPtr.Zero)
        {
            Debug.LogError("load tex failed!");
            return;
        }

        _tex2d_native = Texture2D.CreateExternalTexture(w, h, TextureFormat.RGBA32, false, false, _texPtr);
        TexUI_native.texture = _tex2d_native;
    }

    public void DXNativeUnloadTex()
    {
        if(_texPtr != IntPtr.Zero)
        {
            Destroy(_tex2d_native);
            _tex2d_native = null;
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


}