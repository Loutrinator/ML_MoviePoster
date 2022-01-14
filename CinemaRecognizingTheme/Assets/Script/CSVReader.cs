using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

public class CSVReader
{
    public static List<string[]> Read(string filepath, char separator)
    {
        StreamReader str = new StreamReader(Application.dataPath + filepath);
        bool endFile = false;
        str.ReadLine();
        List<string[]> datas = new List<string[]>();
        while (!endFile)
        {
            string data = str.ReadLine();
            if (data == null)
            {
                endFile = true;
                break;
            }
            datas.Add(data.Split(separator));
        }

        return datas;
    }

}
