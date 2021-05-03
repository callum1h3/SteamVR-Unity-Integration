using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using svrui;

namespace svrui
{
	public class SVRUIManager : MonoBehaviour
	{
	    private void Start()
	    {
	        PointerNetwork.Initalize();
	    }
	}
}