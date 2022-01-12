using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using UnityEngine;
using UnityEngine.UI;


public class CSVEditor : MonoBehaviour
{
    public Plane selectionPlane;
    private Camera camera;
    [SerializeField] private MeshRenderer pointPrefab;
    
    [SerializeField] private Button blueButton;
    [SerializeField] private Material blue;
    
    [SerializeField] private Button redButton;
    [SerializeField] private Material red;

    [Header("Import & export")] 
    [SerializeField] private InputField importField;
    [SerializeField] private InputField exportField;
    private int currentColorMode = 0;

    private List<MeshRenderer> points;
    public void setColorMode(int c)
    {
        if (c < 0.5)
        {
            blueButton.Select();
            currentColorMode =  0;
        }
        else
        {
            redButton.Select();
            currentColorMode =  1;
        }
    }

    public void ImportFromCSV()
    {
        clearPoints();
        StreamReader str = new StreamReader(Application.dataPath + "/StreamingAssets/Datasets/TestCases/" + importField.text);
        bool endFile = false;
        str.ReadLine();
        CultureInfo iv = CultureInfo.InvariantCulture;
        while (!endFile)
        {
            string data = str.ReadLine();
            if (data == null)
            {
                endFile = true;
                break;
            }
            var value = data.Split(',');
            //Debug.Log("Ligne : " + value[0] + "      " + value[1] + "     " + value[2]);
            
            Vector3 pos = new Vector3(float.Parse(value[0],iv),float.Parse(value[1],iv),0);
            MeshRenderer point = Instantiate(pointPrefab, pos, Quaternion.identity);
            point.material = float.Parse(value[2],iv) < 0.5 ? blue : red;
            points.Add(point);
        }
    }
    
    public void ExportToCSV()
    {
        /*
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
        }*/
    }

    private void clearPoints()
    {
        foreach (var point in points)
        {
            Destroy(point);
        }
    }

    private void Start()
    {
        camera = FindObjectOfType<Camera>();
        points = new List<MeshRenderer>();
    }

    private void Update()
    {
        if (Input.GetMouseButtonDown(0))
        {
            RaycastHit hit;
            Ray ray = camera.ScreenPointToRay(Input.mousePosition);
        
            if (Physics.Raycast(ray, out hit))
            {
                MeshRenderer pointFound = hit.transform.GetComponent<MeshRenderer>();
                if (pointFound.GetComponent<SphereCollider>() != null)
                {
                    pointFound.material = currentColorMode < 0.5 ? blue : red;
                }
                else
                {
                    Vector3 pos = hit.point;
                    pos.z = 0;
                    MeshRenderer point = Instantiate(pointPrefab, pos, Quaternion.identity);
                    point.material = currentColorMode < 0.5 ? blue : red;
                    points.Add(point);
                }
            }
        }
    }
}
