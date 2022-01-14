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
    public List<int> layerSizes;
    [SerializeField] private MLPoint pointPrefab;
    [SerializeField] private Material blue;
    [SerializeField] private Material red;
    [SerializeField] private string datasetFolder;
    [SerializeField] [Range(0f, 1f)] private float percentOfTrainingData = 0.75f;
    [SerializeField] private int nbTests = 10;
    [SerializeField] private int nbIterrationsPerTest = 1000;
    [SerializeField] private LineRenderer errorGraph;
    [SerializeField] private Text errorPercent;

    [SerializeField] private RawImage solverImage;
    public Texture2D solverMap;

    [Header("Import & export")] 
    [SerializeField] private InputField importField;

    private List<MLPoint> points;

    private NeuralNetwork network;
    
    private Dataset trainDataset;
    private Dataset testDataset;
    int trainCount = 0;
    int testCount = 0;

    private List<float> errors;
    

    private void Start()
    {
        network = new NeuralNetwork(false);
        network.AddLayer(2, OutputFunction.Linear);
        foreach (var size in layerSizes)
        {
            network.AddLayer(size, OutputFunction.Sigmoid);
        }
        network.AddLayer(1, OutputFunction.Sigmoid);
        
        points = new List<MLPoint>();
        testDataset = new Dataset();
        trainDataset = new Dataset();
        errors = new List<float>();

        CleanUpMap();
    }

    public void ImportFromCSV()
    {
        clear();
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
        
        
        int i = 0;
        foreach(Vector3 data in shuffledData)
        {
            if (i < countDatas * percentOfTrainingData)
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
        
    }

    public void TrainML()
    {
        if (trainDataset.GetInputCount() > 0)
        {
            
            errorGraph.positionCount = 0;
            float zScale = nbTests;
            Debug.Log("Beggining of training");
            for (int i = 0; i < nbTests; i++)
            {
                network.Train(trainDataset, nbIterrationsPerTest, 0.01f, true);
                float accuracy = network.Evaluate(testDataset, testCount, 0.5f);
                errors.Add(1-accuracy);
                errorGraph.positionCount++;
                
                errorGraph.SetPosition(i,new Vector3(i/zScale,0,1-accuracy));
            }

            Debug.Log("Training finished. Total amount of itterations : " + nbTests * nbIterrationsPerTest);
            showDataset(trainDataset, false);
            showDataset(testDataset, true);
            ComputeMap();
            errorPercent.text = "Final error percent : " + errors[nbTests-1] *100f + "%";
        }
        else
        {
            Debug.LogError("Please import datas before training.");
        }
    }

    private void ComputeMap()
    {
        Dataset mapDataset = new Dataset();
        int countPXL = 0;
        for (int y = 0; y < solverMap.height; y++)
        {
            for (int x = 0; x < solverMap.width; x++)
            {
                float normalisedX = x / (float) solverMap.width;
                float normalisedY = y / (float) solverMap.height;
                mapDataset.AddData(new float[]{normalisedX, normalisedY}, new float[]{0});
                countPXL++;
            }
        }
        network.Evaluate(mapDataset, solverMap.height * solverMap.width, 1);
        
        int dataCount = mapDataset.GetDataCount();

        Vector3 pos;
        Color blueCol = blue.color;
        Color redCol = red.color;
        for(int i = 0; i < dataCount; i++)
        {
            int x = (int)(mapDataset.GetInput(i, 0) * solverMap.width);
            int y = (int)(mapDataset.GetInput(i, 1) * solverMap.height);
            //Debug.Log("Before : " + x + " " + y);
            float output = mapDataset.GetOutput(i, 0);
            Color col = Color.Lerp(blueCol, redCol, output);//output < 0.5f ? blueCol : redCol;
            solverMap.SetPixel(x,y,col);
        }
        
        solverMap.Apply();
        solverImage.texture = solverMap;
        //Sprite newSprite = Sprite.Create(solverMap,mapRenderer.sprite.rect,mapRenderer.sprite.pivot,mapRenderer.sprite.pixelsPerUnit);

        //mapRenderer.sprite = newSprite;
    }

    private void CleanUpMap()
    {
        for (int y = 0; y < solverMap.height; y++)
        {
            for (int x = 0; x < solverMap.width; x++)
            {
                solverMap.SetPixel(x,y,Color.white);
            }
        }
        solverMap.Apply();
    }
    public void showDataset(Dataset dataset, bool evaluate)
    {
        
        int dataCount = dataset.GetDataCount();
        int outputCount = dataset.GetOutputCount();

        Vector3 pos;
        for(int i = 0; i < dataCount; i++)
        {
            pos = new Vector3(dataset.GetInput(i, 0), dataset.GetInput(i, 1), 0);
            float expected = dataset.GetExpectedOutput(i, 0);
            float output = dataset.GetOutput(i, 0);
            
            MLPoint point = Instantiate(pointPrefab, pos, Quaternion.identity,this.transform);

            Material expectedMat = expected < 0.5 ? blue : red;
            if (evaluate)
            {
                Material outputMat = output < 0.5 ? blue : red;
                point.setMaterials(outputMat,expectedMat);
            }
            else
            {
                point.setMaterial(expectedMat);
            }
            
            points.Add(point);
        }
    }
    
    public void clear()
    {
        foreach (var point in points)
        {
            Destroy(point.gameObject);
        }
        points.Clear();
    }
}

}
