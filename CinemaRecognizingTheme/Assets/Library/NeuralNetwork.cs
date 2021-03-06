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

	public unsafe float[] Compute(float[] values)
	{
		float[] resultArray = new float[GetOutputSize()];
		
		fixed (float* valuesPtr = values, resultPtr = resultArray)
		{
			NeuralNetwork_Compute(handle, valuesPtr, values.Length, resultPtr, resultArray.Length);
		}

		return resultArray;
	}

	public void Debug_SetValue(int matrixIndex, int x, int y, float value)
	{
		NeuralNetwork_Debug_SetValue(handle, matrixIndex, x, y, value);
	}

	public void Train(Dataset dataset, int iterations, float alpha, bool isClassification)
	{
		NeuralNetwork_Train(handle, dataset.DangerousGetHandle(), iterations, alpha, isClassification);
	}

	public float Evaluate(Dataset dataset, int iterations, float diffThreshold)
	{
		return NeuralNetwork_Evaluate(handle, dataset.DangerousGetHandle(), iterations, diffThreshold);
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
	private static extern unsafe void NeuralNetwork_Compute(IntPtr ptr, float* input, int inputSize, float* output, int outputSize);

	[DllImport(DLL_NAME)]
	private static extern void NeuralNetwork_Debug_SetValue(IntPtr ptr, int matrixIndex, int x, int y, float value);

	[DllImport(DLL_NAME)]
	private static extern void NeuralNetwork_Train(IntPtr ptr, IntPtr dataset, int iterations, float alpha, bool isClassification);

	[DllImport(DLL_NAME)]
	private static extern float NeuralNetwork_Evaluate(IntPtr ptr, IntPtr dataset, int iterations, float diffThreshold);
}
}

