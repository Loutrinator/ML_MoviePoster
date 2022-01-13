using System;

namespace ML
{
    internal static class Program
    {
        private static void Main()
        {
            NeuralNetwork network = new NeuralNetwork();
            network.AddLayer(2, OutputFunction.Linear);
            network.AddLayer(3, OutputFunction.Sigmoid);
            network.AddLayer(1, OutputFunction.Sigmoid);
			
            //Cas de test d'un AND logique
            Dataset dataset = new Dataset();
            dataset.AddData(new float[]{0, 0}, new float[]{0});
            dataset.AddData(new float[]{1, 0}, new float[]{1});
            dataset.AddData(new float[]{0, 1}, new float[]{1});
            dataset.AddData(new float[]{1, 1}, new float[]{0});
	
            network.Train(dataset, 1000000, 0.01f, true);

            int dataCount = dataset.GetDataCount();
            int inputCount = dataset.GetInputCount();
            int outputCount = dataset.GetOutputCount();
			
            for(int i = 0; i < dataCount; i++)
            {
                Console.Write($"[{dataset.GetInput(i, 0)}");
                for(int j = 1; j < inputCount; ++j)
                {
                    Console.Write($", {dataset.GetInput(i, j)}");
                }
				
                Console.Write($"] -> [{dataset.GetOutput(i, 0)}");
                for(int j = 1; j < outputCount; ++j)
                {
                    Console.Write($", {dataset.GetOutput(i, j)}");
                }
                Console.WriteLine("]");
            }
        }
    }
}

