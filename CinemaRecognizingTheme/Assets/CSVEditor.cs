using System;
using System.Collections;
using System.Collections.Generic;
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
}
