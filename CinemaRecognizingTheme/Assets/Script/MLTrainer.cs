using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using UnityEditor;
using UnityEngine;
using UnityEngine.UI;
using Random = System.Random;

namespace ML
{
    
public class MLTrainer : MonoBehaviour
{
    public string datasetFolder;
    [SerializeField] [Range(0f, 1f)] private float percentOfTraining;
    public Plane selectionPlane;
    private Camera camera;
    [SerializeField] private MeshRenderer pointPrefab;
    
    [SerializeField] private Material blue;
    [SerializeField] private Material red;

    [Header("Import & export")] 
    [SerializeField] private InputField importField;
    [SerializeField] private InputField exportField;

    private List<MeshRenderer> points;

    private NeuralNetwork network;
    
    
    public void ImportFromCSV()
    {
        clearPoints();
        StreamReader str = new StreamReader(Application.dataPath + "/StreamingAssets/Datasets/" + datasetFolder + importField.text + ".csv");
        bool endFile = false;
        str.ReadLine();
        CultureInfo iv = CultureInfo.InvariantCulture;
        
        int countDatas = 0;
        List<Vector3> rawDatas = new List<Vector3>();
        
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
            float x = float.Parse(value[0], iv);
            float y = float.Parse(value[1], iv);
            float expectedOut = float.Parse(value[2], iv);
            rawDatas.Add(new Vector3(x,y,expectedOut));
            countDatas++;
        }
        Debug.Log("Dataset fully loaded. " + countDatas + " parsed.");
        Random rnd = new Random();
        IOrderedEnumerable<Vector3> shuffledData = rawDatas.OrderBy(item => rnd.Next());
        
        
        Dataset trainDataset = new Dataset();
        Dataset testDataset = new Dataset();
        int i = 0;
        int trainCount = 0;
        int testCount = 0;
        foreach(Vector3 data in shuffledData)
        {
            if (i < countDatas * percentOfTraining)
            {
                trainDataset.AddData(new float[]{data.x, data.y}, new float[]{data.z});
                trainCount++;
            }
            else
            {
                testDataset.AddData(new float[]{data.x, data.y}, new float[]{data.z});
                testCount++;
            }

            i++;
        }
        
        Debug.Log("Dataset separated into train and test. " + trainCount + "/" + testCount);
        Debug.Log("Beggining of training");
        
        network.Train(trainDataset, 100000, 0.01f, true);
        Debug.Log("Training finished.");
        Debug.Log("Starting evaluation.");
        float error = network.Evaluate(testDataset,testCount,0.1f);
        Debug.Log("Evaluation finished. ");
        Debug.Log("Error : " + error*100f + "%");
        
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
        
        network = new NeuralNetwork();
        network.AddLayer(2, OutputFunction.Linear);
        network.AddLayer(3, OutputFunction.Sigmoid);
        network.AddLayer(1, OutputFunction.Sigmoid);
        
        camera = FindObjectOfType<Camera>();
        points = new List<MeshRenderer>();
    }

    private void Update()
    {
       
    }
}

}
