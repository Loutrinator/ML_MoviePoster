using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
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
    [SerializeField] private Text filename;

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

    private void Start()
    {
        camera = FindObjectOfType<Camera>();
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

    public void WriteCSV()
    {
        string filePath = Application.dataPath + "/StreamingAssets/Datasets/TestCases/" + filename.text+".csv";

        StreamWriter writer = new StreamWriter(filePath);

        writer.WriteLine("x;y;color");

        for(int i = 0; i < transform.childCount; i++)
        {
            if(transform.GetChild(i).name.Contains("Circle"))
            {
                Transform circle = transform.GetChild(i);
                int color = circle.GetComponent<MeshRenderer>().material.color == Color.blue ? 0 : 1;
                writer.WriteLine(circle.position.x + ";" + circle.position.y + ";" + color);
            }
        }

        writer.Close();
        AssetDatabase.Refresh();
    }
}
