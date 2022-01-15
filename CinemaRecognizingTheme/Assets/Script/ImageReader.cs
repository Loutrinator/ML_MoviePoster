using System;
using System.IO;
using System.Drawing;
using UnityEngine;
using UnityEngine.UI;

public class ImageReader
{
    public static int width = 64;
    public static int height = 64;

    public static Texture2D LoadImage(string imageId)
    {
        string imagePath = Application.dataPath + "/StreamingAssets/Datasets/Movies/Images/" + imageId + ".jpg";

        Texture2D tex;
        tex = new Texture2D(width, height);
        WWW ww = new WWW(imagePath);
        ww.LoadImageIntoTexture(tex);
        tex.Resize(width, height);
        tex.Apply();
        return tex;
    }
    
}
