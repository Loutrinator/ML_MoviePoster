using System;
using System.IO;
using System.Drawing;
using UnityEngine;
using UnityEngine.UI;

public class ImageLoader : MonoBehaviour
{
    [SerializeField] Text imageName;
    public int width = 0;
    public int height = 0;

    public void LoadImage()
    {
        string imagePath = Application.dataPath + "/StreamingAssets/Datasets/Movies/Images/" + imageName.text + ".jpg";

        Texture2D tex;
        tex = new Texture2D(width, height);
        WWW ww = new WWW(imagePath);

        GameObject ri = new GameObject();
        ri.AddComponent<Image>();
        ww.LoadImageIntoTexture(tex);

        ri.GetComponent<Image>().sprite = Sprite.Create(tex, new Rect(0, 0, tex.width, tex.height), Camera.current.transform.position);

    }

}
