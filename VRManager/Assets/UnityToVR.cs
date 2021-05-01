using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UnityToVR : MonoBehaviour
{
    public string deviceID;

    void Update()
    {
    	PointerNetwork.SetPointer<float>(deviceID+"PositionX", transform.position.x);
    	PointerNetwork.SetPointer<float>(deviceID+"PositionY", transform.position.y);
    	PointerNetwork.SetPointer<float>(deviceID+"PositionZ", transform.position.z);

    	PointerNetwork.SetPointer<float>(deviceID+"RotationW", transform.rotation.w);
    	PointerNetwork.SetPointer<float>(deviceID+"RotationX", transform.rotation.x);
    	PointerNetwork.SetPointer<float>(deviceID+"RotationY", transform.rotation.y);
    	PointerNetwork.SetPointer<float>(deviceID+"RotationZ", transform.rotation.z);  	
    }
}
