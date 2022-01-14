using System;
using System.IO;
using System.Drawing;
using UnityEngine;
using UnityEngine.UI;

public class ImageLoader : MonoBehaviour
{
    public int width = 0;
    public int height = 0;

    public Texture2D LoadImage(string imageId)
    {
        string imagePath = Application.dataPath + "/StreamingAssets/Datasets/Movies/Images/" + imageId + ".jpg";

        Texture2D tex;
        tex = new Texture2D(width, height);
        WWW ww = new WWW(imagePath);
        ww.LoadImageIntoTexture(tex);
        return tex;
    }

}
