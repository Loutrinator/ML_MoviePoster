using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using UnityEditor;
using UnityEngine;
using UnityEngine.UI;


public class CSVEditor : MonoBehaviour
{
    public Plane selectionPlane;
    private Camera camera;
    [SerializeField] private MeshRenderer pointPrefab;
    
    [SerializeField] private Image selector;
    [SerializeField] private float selectorSpeed = 0.5f;
    
    [SerializeField] private Button blueButton;
    [SerializeField] private Material blue;
    
    [SerializeField] private Button redButton;
    [SerializeField] private Material red;

    [Header("Import & export")] 
    [SerializeField] private InputField importField;
    [SerializeField] private InputField exportField;
    private int currentColorMode = 0;

    private List<MeshRenderer> points;

    private Vector3 selectorPosition;
    private Color selectorColor;

    public void setColorMode(int c)
    {
        if (c < 0.5)
        {
            currentColorMode =  0;
            selectorColor = blue.color;
            selectorPosition = blueButton.transform.position;
        }
        else
        {
            currentColorMode =  1;
            selectorColor = red.color;
            selectorPosition = redButton.transform.position;
        }
    }

    public void ImportFromCSV()
    {
        clearPoints();
        StreamReader str = new StreamReader(Application.dataPath + "/StreamingAssets/Datasets/TestCases/" + importField.text + ".csv");
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
            MeshRenderer point = Instantiate(pointPrefab, pos, Quaternion.identity,this.transform);
            point.material = float.Parse(value[2],iv) < 0.5 ? blue : red;
            points.Add(point);
        }
    }
    
    public void ExportToCSV()
    {
        string filePath = Application.dataPath + "/StreamingAssets/Datasets/TestCases/" + exportField.text+".csv";

        StreamWriter writer = new StreamWriter(filePath);

        writer.WriteLine("x,y,color");

        CultureInfo iv = CultureInfo.InvariantCulture;
        foreach (var mr in transform.GetComponentsInChildren<MeshRenderer>())
        {
            
            int color = mr.material.color == blue.color ? 0 : 1;
            writer.WriteLine(mr.transform.position.x.ToString(iv) + "," + mr.transform.position.y.ToString(iv) + "," + color);
        }

        writer.Close();
        AssetDatabase.Refresh();
    }

    public void clearPoints()
    {
        foreach (var point in points)
        {
            Destroy(point.gameObject);
        }
        points.Clear();
    }

    private void Start()
    {
        camera = FindObjectOfType<Camera>();
        points = new List<MeshRenderer>();
        
        Image blueImage = blueButton.GetComponent<Image>();
        blueImage.color = blue.color;
        Image redImage = redButton.GetComponent<Image>();
        redImage.color = red.color;
        
        selector.transform.position = blueButton.transform.position;
        selector.color = blue.color;

    }

    private void Update()
    {
        selector.transform.position = Vector3.Lerp(selector.transform.position,selectorPosition,selectorSpeed * Time.deltaTime);
        selector.color = Color.Lerp(selector.color, selectorColor, selectorSpeed * Time.deltaTime);
        
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
                    MeshRenderer point = Instantiate(pointPrefab, pos, Quaternion.identity,this.transform);
                    point.material = currentColorMode < 0.5 ? blue : red;
                    points.Add(point);
                }
            }
        }
    }
}
