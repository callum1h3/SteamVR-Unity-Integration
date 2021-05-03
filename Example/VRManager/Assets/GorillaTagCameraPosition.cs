using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using svrui;

#if ENABLE_INPUT_SYSTEM && ENABLE_INPUT_SYSTEM_PACKAGE
#define USE_INPUT_SYSTEM
    using UnityEngine.InputSystem;
    using UnityEngine.InputSystem.Controls;
#endif


public class GorillaTagCameraPosition : MonoBehaviour
{
	public GameObject camera;
	public GameObject hand;

	public float scrollMult = 1;
	private float scrollDist = 1;
	private Vector3 rotation;

    void Update()
    {
		if (Input.GetKey(KeyCode.Escape))
        {
            Cursor.lockState = CursorLockMode.None;
            Cursor.visible = true;
        }

        if (Input.GetMouseButtonDown(1))
        {
            Cursor.lockState = CursorLockMode.Locked;
            Cursor.visible = false;
        }

        if (!Cursor.visible)
        {
            
        	scrollDist += Input.mouseScrollDelta.y * scrollMult;
        	hand.transform.localPosition = new Vector3(0,0,scrollDist);

        	rotation += new Vector3(Input.GetAxis("Mouse Y"), -Input.GetAxis("Mouse X"), 0);
            camera.transform.eulerAngles = rotation;
        }
    }
}
