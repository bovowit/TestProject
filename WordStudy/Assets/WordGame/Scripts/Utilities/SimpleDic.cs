using UnityEngine;
//using UnityEditor;
using System;
using System.Collections;
using System.Collections.Generic;

using System.IO;
//using Newtonsoft.Json;

class Dic
{
    public string key;
    public List<string> kor;
}

[System.Serializable]
public class SimpleDic : ScriptableObject
{
    Dictionary<string, string> gSimpleDic = new Dictionary<string, string>();

    public bool LoadSimpleDic()
    {
        //string path = Application.dataPath + /*"/WordGame" + */"/Resources/" + Utilities.BoardFilesDirectory + "/" + "SimpleDic.txt";
        TextAsset path = Resources.Load<TextAsset>(Utilities.BoardFilesDirectory + "/" + "SimpleDic");
        //StreamReader r = new StreamReader(path.text);
        //string rData = r.ReadToEnd();
        string rData = path.text;
        rData = rData.Replace("\r\n", "");
        rData = rData.ToUpper();

        string[] result = rData.Split(';');
        int iWordCount = result.Length;
        if (iWordCount % 2 == 1)
            --iWordCount;
        for (int i = 0; i < iWordCount; i++)
        {
            try
            {
                gSimpleDic.Add(result[i], result[++i]);
            }
            catch(ArgumentException)
            {
                Console.WriteLine("An element with Key = \"txt\" already exists.");
            }
        }

        return true;

    }

    public string GetWordKor(string sKey)
    {
        string rValue = "";
        if (gSimpleDic.TryGetValue(sKey, out rValue))
            return rValue;
        else
           return rValue;
    }

    public bool LoadSimpleDicJson()
    {
        return true;
    }

}