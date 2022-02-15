using System;
using System.IO;
using System.Drawing;
using UnityEditor.Experimental.GraphView;
using UnityEngine;
using UnityEngine.UI;

public class ImageReader
{
    public static int width = 8;
    public static int height = 8;

    public static Texture2D LoadImage(string imageId)
    {
        string imagePath = Application.dataPath + "/StreamingAssets/Datasets/Movies/Images/" + imageId + ".jpg";
        
        Texture2D tex;
        tex = new Texture2D(width, height);
        WWW ww = new WWW(imagePath);
        
        ww.LoadImageIntoTexture(tex);
        Debug.Log("imagepath : " + imagePath + " -> " + tex.width + " " + tex.height);
        return Resize(tex, width, height);
    }

    public static Texture2D Resize(Texture2D source, int newWidth, int newHeight)
    {
        source.filterMode = FilterMode.Point;
        RenderTexture rt = RenderTexture.GetTemporary(newWidth, newHeight);
        rt.filterMode = FilterMode.Point;
        RenderTexture.active = rt;
        Graphics.Blit(source, rt);
        Texture2D nTex = new Texture2D(newWidth, newHeight);
        nTex.ReadPixels(new Rect(0, 0, newWidth, newHeight), 0,0);
        nTex.Apply();
        RenderTexture.active = null;
        RenderTexture.ReleaseTemporary(rt);
        return nTex;
    }
    
}
