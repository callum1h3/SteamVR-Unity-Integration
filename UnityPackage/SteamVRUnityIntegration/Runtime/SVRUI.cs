using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System;
using Memory.Win64;
using System.Diagnostics;

namespace svrui
{
	public static class PointerNetwork
	{
		// This contains the process of the steamvr driver
	    private static Process process;
	    // This contains the memory of the steamvr driver
	    private static MemoryHelper64 memory;
	    // This contains all pointers created by the driver.
	    private static Dictionary<string, ulong> pointers = new Dictionary<string, ulong>();

	    // This loads the pointer file that was createdf by the steamvr driver
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

	    public static void SetJointStick( string hand, Vector2 pos )
	    {
	        SetPointer<float>( hand+"TrackpadX", pos.x );
	        SetPointer<float>( hand+"TrackpadY", pos.y );
	    }

	    public static void Initalize()
	    {
	    	string apppath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
        	LoadFile(apppath+@"\processInformation.txt");
	    }
	}
}