using System;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;

namespace ML
{
	public class Dataset : SafeHandleZeroOrMinusOneIsInvalid
	{
		public Dataset() : base(true)
		{
			SetHandle(Dataset_Create());
		}

		protected override bool ReleaseHandle()
		{
			Dataset_Destroy(handle);
			return true;
		}

		public void AddData(float[] input, float[] expectedOutput)
		{
			Dataset_AddData(handle, input, input.Length, expectedOutput, expectedOutput.Length);
		}
		
		public int GetDataCount()
		{
			return Dataset_GetDataCount(handle);
		}
		
		public int GetInputCount()
		{
			return Dataset_GetInputCount(handle);
		}
		
		public int GetOutputCount()
		{
			return Dataset_GetOutputCount(handle);
		}

		public float GetInput(int dataIndex, int inputIndex)
		{
			return Dataset_GetInput(handle, dataIndex, inputIndex);
		}

		public float GetExpectedOutput(int dataIndex, int expectecOutputIndex)
		{
			return Dataset_GetExpectedOutput(handle, dataIndex, expectecOutputIndex);
		}

		public float GetOutput(int dataIndex, int outputIndex)
		{
			return Dataset_GetOutput(handle, dataIndex, outputIndex);
		}

#if DEBUG
		private const string DLL_NAME = "MachineLearningd.dll";
#else
		private const string DLL_NAME = "MachineLearning.dll";
#endif

		[DllImport(DLL_NAME)]
		private static extern IntPtr Dataset_Create();

		[DllImport(DLL_NAME)]
		private static extern void Dataset_Destroy(IntPtr ptr);
		
		[DllImport(DLL_NAME)]
		private static extern void Dataset_AddData(IntPtr ptr, float[] input, int inputSize, float[] output, int outputSize);
		
		[DllImport(DLL_NAME)]
		private static extern int Dataset_GetDataCount(IntPtr ptr);
		
		[DllImport(DLL_NAME)]
		private static extern int Dataset_GetInputCount(IntPtr ptr);
		
		[DllImport(DLL_NAME)]
		private static extern int Dataset_GetOutputCount(IntPtr ptr);
		
		[DllImport(DLL_NAME)]
		private static extern float Dataset_GetInput(IntPtr ptr, int dataIndex, int inputIndex);
		
		[DllImport(DLL_NAME)]
		private static extern float Dataset_GetExpectedOutput(IntPtr ptr, int dataIndex, int expectecOutputIndex);
		
		[DllImport(DLL_NAME)]
		private static extern float Dataset_GetOutput(IntPtr ptr, int dataIndex, int outputIndex);
	}
}

