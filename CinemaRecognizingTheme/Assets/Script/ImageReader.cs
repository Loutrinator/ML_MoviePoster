using System;
using System.IO;
using System.Drawing;
using UnityEngine;
using UnityEngine.UI;

public class ImageReader
{
    public static int width = 0;
    public static int height = 0;

    public static Texture2D LoadImage(string imageId)
    {
        string imagePath = Application.dataPath + "/StreamingAssets/Datasets/Movies/Images/" + imageId + ".jpg";

        Texture2D tex;
        tex = new Texture2D(width, height);
        WWW ww = new WWW(imagePath);
        ww.LoadImageIntoTexture(tex);
        return tex;
    }

}
