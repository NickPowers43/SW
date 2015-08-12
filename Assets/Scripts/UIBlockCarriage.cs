using UnityEngine;
using System.Collections;
using System.Runtime.Serialization;
using UnityEngine.UI;

[System.Serializable]
public struct SpriteBlockPair
{
	public Sprite sprite;
	public BlockType type;
}

public class UIBlockCarriage : MonoBehaviour {

	public GameObject blockButtonPrefab;
	public SpriteBlockPair[] blocks;

	// Use this for initialization
	void Start () {

		Vector3 buttonPos = Vector3.zero;

		for (int i = 0; i < blocks.Length; i++) {

			GameObject bGO = GameObject.Instantiate(
				blockButtonPrefab, 
				buttonPos, 
                Quaternion.identity) as GameObject;

			Button button = bGO.GetComponent<Button>();
			int arg = i;
			UnityEngine.Events.UnityAction action = () => {GameManager.Instance.SelectBlockType(arg);};
			button.onClick.AddListener(action);
			Image image = bGO.GetComponent<Image>();
			image.sprite = blocks[i].sprite;

			bGO.transform.SetParent(this.transform, false);

			buttonPos += new Vector3(((RectTransform)this.transform).rect.width, 0.0f, 0.0f);
		}

	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
