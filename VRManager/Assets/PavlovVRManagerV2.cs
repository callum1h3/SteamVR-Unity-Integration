using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Reflection;
using System.Linq;


// This is messy code for pavlov, for some reason i couldn't get reloading working but you can see how keybinding works.

public static class WeaponHelper
{
	public static int BoolToInt( bool state )
	{
		if (state)
			return 1;
		return 0;
	}
}

public class WEP_HANDS : GunBase
{
	public new string ID = "hands";
	private float scrollDist;

	public new void Update( PavlovVRManagerV2 ms )
	{
        scrollDist += Input.mouseScrollDelta.y * 0.1f;
        ms.HANDS[0].transform.localPosition = new Vector3(0,0,-scrollDist);
    	ms.HANDS[1].transform.localPosition = new Vector3(0,-1,0);

    	ms.HANDS[0].transform.localEulerAngles = new Vector3(60,-1,0);

    	PointerNetwork.SetPointer<float>( "LHANDTRIGGER", (float)WeaponHelper.BoolToInt( Input.GetMouseButton(0) ) );
    	PointerNetwork.SetPointer<int>( "LHANDGRIP", WeaponHelper.BoolToInt( Input.GetKey(KeyCode.Space) ) );
	}
}

public class WEP_AK47 : GunBase
{
	public new string ID = "ak47";
	private float scrollDist;

	public new void Update( PavlovVRManagerV2 ms )
	{
        scrollDist += Input.mouseScrollDelta.y * 0.1f;
        ms.HANDS[0].transform.localPosition = new Vector3(0,0,-scrollDist);
    	ms.HANDS[1].transform.localPosition = new Vector3(0,-1,0);

    	ms.HANDS[0].transform.localEulerAngles = new Vector3(60,-1,0);

    	PointerNetwork.SetPointer<float>( "LHANDTRIGGER", (float)WeaponHelper.BoolToInt( Input.GetMouseButton(0) ) );

    	if (Input.GetKey(KeyCode.Space))
    	{
    		PointerNetwork.SetPointer<int>( "LHANDGRIP", WeaponHelper.BoolToInt( Input.GetKey(KeyCode.Space) ) );
    		ms.gunSelect = "hands";
    	}
    	
	}
}

public class GunBase
{
	public string ID = "";

	public void Update( PavlovVRManagerV2 ms )
	{

	}
}

public class PavlovVRManagerV2 : MonoBehaviour
{
	public GameObject HMD;
	public List<GameObject> HANDS = new List<GameObject>();

	public string gunSelect = "ak47";
	public bool hasControl = true;

	private Vector3 rotation;
	private Dictionary<string, dynamic> gunTypes = new Dictionary<string, dynamic>();

    void Start()
    {
        CacheGunTypes();
    }

    private void Update()
    {
        FPSView();
        WeaponUpdate();
    }

    public void DisableControl(float length)
    {
 		StartCoroutine(DisableWait(length));
    }

    private IEnumerator DisableWait(float length)
    {
    	hasControl = true;
    	yield return new WaitForSeconds(length);
    	hasControl = false;
    }

    private void WeaponUpdate()
    {
    	if (gunTypes.ContainsKey(gunSelect))
    	{
    		gunTypes[gunSelect].Update( this );
    	}
    }

    private void FPSView()
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

        if (!Cursor.visible && hasControl)
        {
        	
        	PointerNetwork.SetJointStick( "LHAND", new Vector2(Input.GetAxisRaw("Horizontal"), Input.GetAxisRaw("Vertical")) );
        	rotation += new Vector3(Input.GetAxis("Mouse Y"), -Input.GetAxis("Mouse X"), 0);
            HMD.transform.eulerAngles = rotation;
        }  	
    }

    private void CacheGunTypes()
    {
    	foreach(dynamic gun in GetAllGunClasses())
    		gunTypes.Add(gun.ID, gun);
    }

    private List<dynamic> GetAllGunClasses()
    {
        return AppDomain.CurrentDomain.GetAssemblies()
            .SelectMany(assembly => assembly.GetTypes())
            .Where(type => type.IsSubclassOf(typeof(GunBase)))
            .Select(type => Activator.CreateInstance(type)).ToList();
    }
}
