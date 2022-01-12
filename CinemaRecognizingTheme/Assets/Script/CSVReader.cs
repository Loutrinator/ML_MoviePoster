using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

public class CSVReader : MonoBehaviour
{
    [SerializeField]
    private GameObject point;

    [SerializeField] private string filePath;

    // Start is called before the first frame update
    void Start()
    {
        StreamReader str = new StreamReader(Application.dataPath + "/StreamingAssets/Datasets/" + filePath);
        bool endFile = false;
        str.ReadLine();
        while (!endFile)
        {
            string data = str.ReadLine();
            if (data == null)
            {
                endFile = true;
                break;
            }
            var value = data.Split(';');
            Debug.Log("Ligne : " + value[0] + "      " + value[1] + "     " + value[2]);
            
            Color c = point.GetComponent<SpriteRenderer>().color;

            if (float.Parse(value[2]) > 0.5) c = Color.red;
            else c = Color.blue;

            point.GetComponent<SpriteRenderer>().color = c;

            point.transform.position = new Vector2(float.Parse(value[0]), float.Parse(value[1]));

            Instantiate(point);
        }
    }

}
