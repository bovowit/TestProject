﻿using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class UIScreenHintOverlay : UIScreen
{
	#region Inspector Variables

	[SerializeField] private Image		categoryIconImage;
	[SerializeField] private Text		categoryNameText;
	[SerializeField] private Text		categoryLevelText;
    [SerializeField] private Text   hintText;
    //[SerializeField] private GameObject	plusOneHintText;

    #endregion

    #region Member Variables
    #endregion

    #region Properties
    #endregion

    #region Unity Methods
    #endregion

    #region Public Methods

    public override void OnShowing(object data)
	{
		CategoryInfo categoryInfo = GameManager.Instance.GetCategoryInfo(GameManager.Instance.ActiveCategory);

		categoryIconImage.sprite	= categoryInfo.icon;
		categoryNameText.text		= GameManager.Instance.ActiveCategory;

		categoryLevelText.gameObject.SetActive(true);
		categoryLevelText.text = "Level " + (GameManager.Instance.ActiveLevelIndex + 1).ToString();

        hintText.gameObject.SetActive(true);
        hintText.text = (string)data;
       
    }

	#endregion

	#region Protected Methods
	#endregion

	#region Private Methods
	#endregion
}
