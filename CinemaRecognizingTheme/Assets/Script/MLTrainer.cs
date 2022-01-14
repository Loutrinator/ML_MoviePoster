using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using Random = System.Random;

namespace ML
{
    
public class MLTrainer : MonoBehaviour
{
    public List<int> layerSizes;
    public float alpha = 0.03f;
    [SerializeField] private int nbTests = 10;
    [SerializeField] private int nbIterrationsPerTest = 1000;
    [SerializeField] [Range(0f, 1f)] private float percentOfTrainingData = 0.75f;
    [SerializeField] private MLPoint pointPrefab;
    [SerializeField] private Material blue;
    [SerializeField] private Material red;
    [SerializeField] private string datasetFolder;
    [SerializeField] private LineRenderer errorGraph;
    [SerializeField] private Text errorPercent;

    [SerializeField] private RawImage solverImage;
    public Texture2D solverMap;

    [Header("Import & export")] 
    [SerializeField] private InputField importField;

    private List<MLPoint> trainPoints;
    private bool trainVisible = true;
    [SerializeField] private Image showTrainButton;
    private List<MLPoint> testPoints;
    private bool testVisible = true;
    [SerializeField] private Image showTestButton;
    [SerializeField] private Color defaultColor;
    [SerializeField] private Color visibleColor;
    
    [SerializeField] private RawImage mapImage;
    private bool mapVisible = false;
    private bool mapComputed = false;
    [SerializeField] private Image showMapButton;

    [SerializeField] private Text ImportedText;
    [SerializeField] private Text TrainedText;

    private NeuralNetwork network;
    
    private Dataset trainDataset;
    private Dataset testDataset;
    int trainCount = 0;
    int testCount = 0;

    private List<float> errors;
    

    private void Start()
    {
        ImportedText.enabled = false;
        TrainedText.enabled = false;
        
        network = new NeuralNetwork(false);
        network.AddLayer(2, OutputFunction.Linear);
        foreach (var size in layerSizes)
        {
            network.AddLayer(size, OutputFunction.Sigmoid);
        }
        network.AddLayer(1, OutputFunction.Sigmoid);
        
        trainPoints = new List<MLPoint>();
        testPoints = new List<MLPoint>();
        testDataset = new Dataset();
        trainDataset = new Dataset();
        errors = new List<float>();

        CleanUpMap();
    }

    public void ImportFromCSV()
    {
        clear();
        string path = "/StreamingAssets/Datasets/" + datasetFolder + importField.text + ".csv";
        List<string[]> content = CSVReader.Read(path,',');
        CultureInfo iv = CultureInfo.InvariantCulture;
        List<float[]> rawDatas = new List<float[]>();
        for(int i = 1; i < content.Count; i++)
        {
            string[] line = content[i];
            //Debug.Log("Ligne : " + value[0] + "      " + value[1] + "     " + value[2]);
            float x = float.Parse(line[0], iv);
            float y = float.Parse(line[1], iv);
            float expectedOut = float.Parse(line[2], iv);
            rawDatas.Add( new []{x, y, expectedOut});
        }
        Debug.Log("Dataset fully loaded. " + content.Count + " parsed.");
        Random rnd = new Random();
        IOrderedEnumerable<float[]> shuffledData = rawDatas.OrderBy(item => rnd.Next());
        
        
        int counter = 0;
        foreach(float[] data in shuffledData)
        {
            if (counter < content.Count * percentOfTrainingData)
            {
                trainDataset.AddData(new float[]{data[0], data[1]}, new float[]{data[2]});
                trainCount++;
            }
            else
            {
                testDataset.AddData(new float[]{data[0], data[1]}, new float[]{data[2]});
                testCount++;
            }
            counter++;
        }
        
        Debug.Log("Dataset separated into train and test. " + trainCount + "/" + testCount);
        
        ImportedText.enabled = true;
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
                network.Train(trainDataset, nbIterrationsPerTest, 0.03f, true);
                float accuracy = network.Evaluate(testDataset, testCount, 0.45f);
                errors.Add(1-accuracy);
                errorGraph.positionCount++;
                
                errorGraph.SetPosition(i,new Vector3(i/zScale,0,1-accuracy));
            }

            Debug.Log("Training finished. Total amount of itterations : " + nbTests * nbIterrationsPerTest);
            InstatiateDataset(trainDataset, false, trainPoints, false);
            InstatiateDataset(testDataset, true, testPoints, true);
            errorPercent.text = "Final error percent : " + errors[nbTests-1] *100f + "%";
            
            TrainedText.enabled = true;
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
        mapComputed = true;
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
        mapComputed = false;
    }
    public void InstatiateDataset(Dataset dataset, bool evaluate, List<MLPoint> listOfPoints, bool visible)
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
            point.SetVisible(visible);
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
            
            listOfPoints.Add(point);
        }
    }
    
    public void clear()
    {
        foreach (var point in trainPoints)
        {
            Destroy(point.gameObject);
        }
        trainPoints.Clear();
        foreach (var point in testPoints)
        {
            Destroy(point.gameObject);
        }
        testPoints.Clear();
    }
    
    public void SwitchToEditor()
    {
        SceneManager.LoadScene(1);
    }
    public void ShowTrain()
    {
        trainVisible = !trainVisible;
        foreach (var point in trainPoints)
        {
            point.SetVisible(trainVisible);
        }
        showTrainButton.color = trainVisible ? visibleColor : defaultColor;
        
    }
    public void ShowTest()
    {
        testVisible = !testVisible;
        foreach (var point in testPoints)
        {
            point.SetVisible(testVisible);
        }
        showTestButton.color = testVisible ? visibleColor : defaultColor;
    }
    public void ShowMap()
    {
        if(!mapComputed) ComputeMap();
        mapVisible = !mapVisible;
        mapImage.enabled = mapVisible;
        showMapButton.color = testVisible ? visibleColor : defaultColor;
    }
}

}
