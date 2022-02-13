using System;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;

namespace ML
{
	public class NeuralNetwork : SafeHandleZeroOrMinusOneIsInvalid
	{
		public NeuralNetwork(bool debug = false) : base(true)
		{
			SetHandle(NeuralNetwork_Create(debug));
		}

		protected override bool ReleaseHandle()
		{
			NeuralNetwork_Destroy(handle);
			return true;
		}

		public void AddLayer(int neuronCount, OutputFunction outputFunction)
		{
			NeuralNetwork_AddLayer(handle, neuronCount, outputFunction);
		}

		private int GetOutputSize()
		{
			return NeuralNetwork_GetOutputSize(handle);
		}

		public float[] Compute(float[] values)
		{
			float[] result = new float[GetOutputSize()];
			
			NeuralNetwork_Compute(handle, values, values.Length, result, result.Length);

			return result;
		}

		public void Debug_SetValue(int matrixIndex, int x, int y, float value)
		{
			NeuralNetwork_Debug_SetValue(handle, matrixIndex, x, y, value);
		}

		public void Train(Dataset dataset, int iterations, float alpha, bool isClassification)
		{
			NeuralNetwork_Train(handle, dataset.DangerousGetHandle(), iterations, alpha, isClassification);
		}

		public float Evaluate(Dataset dataset, float diffThreshold, LossFunction lossFunction)
		{
			return NeuralNetwork_Evaluate(handle, dataset.DangerousGetHandle(), diffThreshold, lossFunction);
		}
		
		public void Save(string path, bool beautify = false)
		{
			NeuralNetwork_Save(handle, path, beautify);
		}

		public void Load(string path)
		{
			NeuralNetwork_Load(handle, path);
		}

#if DEBUG
		private const string DLL_NAME = "MachineLearningd.dll";
#else
		private const string DLL_NAME = "MachineLearning.dll";
#endif
		
		[DllImport(DLL_NAME)]
		private static extern IntPtr NeuralNetwork_Create(bool debug);

		[DllImport(DLL_NAME)]
		private static extern void NeuralNetwork_Destroy(IntPtr ptr);

		[DllImport(DLL_NAME)]
		private static extern void NeuralNetwork_AddLayer(IntPtr ptr, int neuronCount, OutputFunction outputFunction);

		[DllImport(DLL_NAME)]
		private static extern int NeuralNetwork_GetOutputSize(IntPtr ptr);

		[DllImport(DLL_NAME)]
		private static extern void NeuralNetwork_Compute(IntPtr ptr, float[] input, int inputSize, float[] output, int outputSize);

		[DllImport(DLL_NAME)]
		private static extern void NeuralNetwork_Debug_SetValue(IntPtr ptr, int matrixIndex, int x, int y, float value);

		[DllImport(DLL_NAME)]
		private static extern void NeuralNetwork_Train(IntPtr ptr, IntPtr dataset, int iterations, float alpha, bool isClassification);

		[DllImport(DLL_NAME)]
		private static extern float NeuralNetwork_Evaluate(IntPtr ptr, IntPtr dataset, float diffThreshold, LossFunction lossFunction);

		[DllImport(DLL_NAME)]
		private static extern void NeuralNetwork_Save(IntPtr ptr, string path, bool beautify);

		[DllImport(DLL_NAME)]
		private static extern void NeuralNetwork_Load(IntPtr ptr, string path);
	}
}

