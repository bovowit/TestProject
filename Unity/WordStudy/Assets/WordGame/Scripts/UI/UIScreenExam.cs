using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class UIScreenExam : UIScreen
{
	#region Inspector Variables

	[SerializeField] private Text 			categoryText;
	[SerializeField] private Text 			levelText;
	[SerializeField] private Image			iconImage;
	[SerializeField] private Text 			hintBtnText;
	[SerializeField] private Text 			selectedWordText;
	[SerializeField] private LetterBoard	letterBoard;
	
	#endregion

	#region Unity Methods

	private void Update()
	{
		hintBtnText.text = string.Format("HINT ({0})", GameManager.Instance.CurrentHints);
	}

	#endregion

	#region Public Methods

	public override void Initialize()
	{
		selectedWordText.text = "";

		letterBoard.OnSelectedWordChanged += (string word) => 
		{
			selectedWordText.text = word;
		};
	}

	public override void OnShowing(object data)
	{
		CategoryInfo categoryInfo = GameManager.Instance.GetCategoryInfo(GameManager.Instance.ActiveCategory);

		categoryText.text	= GameManager.Instance.ActiveCategory.ToUpper();
		hintBtnText.text	= string.Format("HINT ({0})", GameManager.Instance.CurrentHints);
		iconImage.sprite	= categoryInfo.icon;

		//if (GameManager.Instance.ActiveCategory == GameManager.dailyPuzzleId)
		//{
		//	levelText.text = string.Format("COMPLETE TO GAIN 1 HINT");
		//}
		//else
		{
			levelText.text = string.Format("LEVEL {0}", GameManager.Instance.ActiveLevelIndex + 1);
		}

	}
	
	public void OnBackClicked()
	{
		if (!GameManager.Instance.AnimatingWord)
		{
			UIScreenController.Instance.Show(UIScreenController.MainScreenId, true, true, false, Tween.TweenStyle.EaseOut, null, GameManager.Instance.ActiveCategory);
		}

        GameManager.Instance.g_bExam = false;

    }

	#endregion
}
