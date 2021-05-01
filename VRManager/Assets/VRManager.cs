using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System;
using Memory.Win64;
using System.Diagnostics;

public static class PointerNetwork
{
    private static Process process;
    private static MemoryHelper64 memory;
    private static Dictionary<string, ulong> pointers = new Dictionary<string, ulong>();

    public static Dictionary<string, int> buttonStates = new Dictionary<string, int>();
    public static Dictionary<string, float> buttonDelay = new Dictionary<string, float>();

    public static void LoadFile(string filename)
    {
        UnityEngine.Debug.Log("VR: Loaded Pointer File!");

        string text = File.ReadAllText(filename);
        string[] lines = text.Replace("\r", "").Split('\n');

        process = Process.GetProcessById(Int32.Parse(lines[0]));
        memory = new MemoryHelper64(process);

        for (int i = 1; i < lines.Length-1; i++)
        {
            string[] words = lines[i].Split(' ');
            pointers.Add(words[0], (ulong)Convert.ToInt64(words[1], 16));
        }
    }

    public static void SetPointer<T>( string name, T val )
    {
        memory.WriteMemory<T>(pointers[name], val);
    }
    
    private static bool StateToBool(int state)
    {
        if (state > 1)
            return true;
        return false;
    }

    public static void SetButton(string button, int state)
    {
        SetPointer<bool>( button, StateToBool(state) );
        buttonStates[button] = state; 
        buttonDelay[button] = Time.fixedTime + 0.1f; 
    }

    public static void SetJointStick( string hand, Vector2 pos )
    {
        SetPointer<float>( hand+"TrackpadX", pos.x );
        SetPointer<float>( hand+"TrackpadY", pos.y );
    }
}

public class VRManager : MonoBehaviour
{
	void Awake()
	{
        string apppath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
        PointerNetwork.LoadFile(apppath+@"\processInformation.txt");
	}

    void Update()
    {
        List<string> marked = new List<string>();
        foreach(KeyValuePair<string, int> entry in PointerNetwork.buttonStates)
        {
            if (entry.Value == 1)
            {
                PointerNetwork.SetPointer<bool>( entry.Key, false );
                marked.Add(entry.Key);                  
            }
        }

        foreach(string item in marked)
            if (PointerNetwork.buttonDelay[item] < Time.fixedTime)
                PointerNetwork.buttonStates[item] = 0;    
    }
}

