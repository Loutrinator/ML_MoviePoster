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
    [SerializeField] private string datasetFolder;
    [SerializeField] [Range(0f, 1f)] private float percentOfTrainingData = 0.75f;
    [SerializeField] private int nbTests = 10;
    [SerializeField] private int nbIterrationsPerTest = 1000;
    

    [Header("Import & export")] 
    [SerializeField] private InputField importField;

    private List<MeshRenderer> points;

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
        network.AddLayer(3, OutputFunction.Sigmoid);
        network.AddLayer(1, OutputFunction.Sigmoid);
        
        points = new List<MeshRenderer>();
        testDataset = new Dataset();
        trainDataset = new Dataset();
    }

    public void ImportFromCSV()
    {
        Clear();
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
            Debug.Log("Beggining of training");
            for (int i = 0; i < nbTests; i++)
            {
                network.Train(trainDataset, nbIterrationsPerTest, 0.01f, true);
                errors.Add(network.Evaluate(testDataset,testCount,0.1f));
            }

            Debug.Log("Training finished. Total amount of itterations : " + nbTests * nbIterrationsPerTest);
            Debug.Log("Final error percent : " + errors[nbTests-1] *100f + "%");
        }
        else
        {
            Debug.LogError("Please import datas before training.");
        }
    }
    public void Clear()
    {
    }
}

}
