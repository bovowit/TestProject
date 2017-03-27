using UnityEngine;
using UnityEditor;
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
        string path = Application.dataPath + "/WordGame" + "/Resources/" + Utilities.BoardFilesDirectory + "/" + "SimpleDic.txt";
        StreamReader r = new StreamReader(path);
        string rData = r.ReadToEnd();
        rData = rData.Replace("\r\n", "");
        rData = rData.ToUpper();

        string[] result = rData.Split(';');
        int iWordCount = result.Length;
        if (iWordCount % 2 == 1)
            --iWordCount;
        for (int i = 0; i < iWordCount; i++)
        {
            gSimpleDic.Add(result[i], result[++i]);
        }

        return true;

    }

    public string GetWordKor(string sKey)
    {
        string rValue = gSimpleDic[sKey];
        return rValue;
    }

    public bool LoadSimpleDicJson()
    {
        return true;
    }

}