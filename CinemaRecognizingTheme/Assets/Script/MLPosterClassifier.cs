using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;
using Random = System.Random;

namespace ML
{
    enum DataColorFormat
    {
        rgb,
        grayscale
    }
    
    public class MLPosterClassifier : MonoBehaviour
    {

        public List<int> layerSizes;
        public int nbOfMovieTypes = 2;
        public int nbIterrationsPerTest = 100;
        [SerializeField] private float alpha = 0.03f;
        [SerializeField] [Range(0f,1f)]
        private float percentOfTrain = 0.5f;
        [SerializeField] 
        private string datasetFileName;

        [SerializeField] private Text epochsLabel;
        [SerializeField] private Text errorLabel;
        [SerializeField] private LineRenderer errorGraph;

        private DataColorFormat imageDataFormat = DataColorFormat.grayscale;
        private NeuralNetwork network;

        Dataset train;
        Dataset test;
        private bool training = false;
        private int epochs = 0;
        private List<float> errors = new List<float>();
        
        public void CreateNeuralNetwork()
        {
            int nbInputs = ImageReader.width * ImageReader.height;
            nbInputs *= imageDataFormat == DataColorFormat.rgb ? 3 : 1;
            
            network = new NeuralNetwork(false);
            network.AddLayer(nbInputs, OutputFunction.Linear);
            string desc = nbInputs + " ";
            foreach (var size in layerSizes)
            {
                network.AddLayer(size, OutputFunction.Sigmoid);
                desc += size + " ";
            }
            network.AddLayer(nbOfMovieTypes, OutputFunction.Sigmoid);
            desc += nbOfMovieTypes;
            Debug.Log("Neural network created : (" + desc + ")");
        }
        
        public void ImportDataset()
        {

            ReadDataset(datasetFileName, nbOfMovieTypes, out train, out test);
            Debug.Log("Datasets created ! ");
            Debug.Log("Training datasets size : " + train.GetDataCount());
            Debug.Log("Testing datasets size : " + test.GetDataCount());
        }
        
        private void Update()
        {
            if (training)
            {
                if (train.GetInputCount() > 0)
                {
            
                    network.Train(train, nbIterrationsPerTest, alpha, true);
                    float accuracy = network.Evaluate(test, 0.499f, LossFunction.MeanSquareError);
                    errors.Add(1-accuracy);
                    UpdateErrorGraph();
                    epochs += nbIterrationsPerTest;
                    epochsLabel.text = epochs + " epochs";
                    errorLabel.text = "Error percent : " + (errors[errors.Count - 1] * 100) + "%";
                }
                else
                {
                    Debug.LogError("Please import datas before training.");
                }
            }
        }

        private void ReadDataset(string datasetName, int typeCount, out Dataset train, out Dataset test)
        {
            train = new Dataset();
            test = new Dataset();
            string filepath = "/StreamingAssets/Datasets/Movies/" + datasetName + ".csv";
            List<string[]> csvContent = CSVReader.Read(filepath,';');
            List<float[]> inputs = new List<float[]>();
            List<float[]> outputs = new List<float[]>();
            Debug.Log("File read: " + csvContent.Count + " lines found.");
            CultureInfo iv = CultureInfo.InvariantCulture;
            for (int i = 1; i < csvContent.Count; i++)
            {
                string[] line = csvContent[i];
                string imageId = line[0];

                Texture2D texture = ImageReader.LoadImage(imageId);
                float[] dataInputs = extractImageData(texture,imageDataFormat);
                float[] dataOutputs = new float[typeCount];
                
                //Debug.Log("Image converted : " + dataInputs.Length + " inputs extracted.");
                for (int j = 0; j < line.Length-2; j++)
                {
                    dataOutputs[j] = float.Parse(line[j + 2], iv);
                }
                //Debug.Log("Expected outputs : " + dataOutputs.Length + " outputs.");
                inputs.Add(dataInputs);
                outputs.Add(dataOutputs);
            }
            
            //randomisation du dataset
            shuffleDataset(inputs, outputs, out inputs, out outputs);
            
            //separation du dataset randomisé en deux datasets séparés
            int separation = (int)(inputs.Count * percentOfTrain);
            for (int i = 0; i < inputs.Count; i++)
            {
                float[] input = inputs[i];
                float[] output = outputs[i];
                if (i < separation)
                {
                    train.AddData(input,output);
                    test.AddData(input,output);
                }
            }
        }

        private void shuffleDataset(List<float[]> inputs, List<float[]> outputs, out List<float[]>shuffledInputs, out List<float[]> shuffledOutputs)
        {
            Random rng = new Random();
            var rawDataset = inputs.Zip(outputs, (a, b) => new { a, b } ).ToList();
            
            int n = rawDataset.Count; 
            while (n > 1) {  
                n--;  
                int k = rng.Next(n + 1);  
                var tmp = rawDataset[k];  
                rawDataset[k] = rawDataset[n];  
                rawDataset[n] = tmp;  
            }  
            shuffledInputs = rawDataset.Select(pair => pair.a).ToList();
            shuffledOutputs = rawDataset.Select(pair => pair.b).ToList();
        }

        private float[] extractImageData(Texture2D tex,DataColorFormat format)
        {
            int dataSize = tex.width * tex.height;
            if(format == DataColorFormat.rgb) dataSize *= 3; 
            float[] data = new float[dataSize];
            for (int y = 0; y < tex.width; y++)
            {
                for (int x = 0; x < tex.height; x++)
                {
                    int i = x + y * tex.height;
                    Color col = tex.GetPixel(x, y);
                    if (format == DataColorFormat.rgb)
                    {
                        data[i + 0] = col.r;
                        data[i + 1] = col.g;
                        data[i + 2] = col.b;
                    }
                    else
                    {
                        data[i] = col.grayscale;
                    }
                }
            }

            return data;
        }

        public void StartTraining()
        {
            training = true;
            epochs = 0;
            Debug.Log("Started training");
        }

        public void StopTraining()
        {
            training = false;
            Debug.Log("Stopped training");
        }

        public void UpdateErrorGraph()
        {
            errorGraph.positionCount = errors.Count;
            float zScale = errors.Count;
            for (int i = 0; i < errors.Count; i++)
            {
                float error = errors[i];
                errorGraph.SetPosition(i,new Vector3(i/zScale,0,error));
            }
        }
    }
}