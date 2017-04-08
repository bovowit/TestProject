using UnityEngine;

using System.Collections;
using System.Collections.Generic;
using System.IO;

using SimpleJSON;

/// <summary>
/// Holds information about each category in the game
/// </summary>
[System.Serializable]
public class CategoryInfo
{
	public string			name;			// Name of the category, should be unique
	public string			description;	// Short description, can be anything
	public Sprite			icon;			// An icon that goes with the category
	public List<LevelInfo>	levelInfos;		// The list of levels in this category
}

/// <summary>
/// Holds information about each level in the game.
/// </summary>
[System.Serializable]
public class LevelInfo
{
	public string[]	words = null;
}

public class GameManager : SingletonComponent<GameManager>
{
	#region Data Classes

	/// <summary>
	/// Holds infomation about the state of a board that is being play.
	/// </summary>
	public class BoardState
	{
		public enum TileState
		{
			NotUsed,
			Found,
			UsedButNotFound
		}

		public string		wordBoardId;		// Unique id for this board
		public int			wordBoardSize;		// The size of the board
		public string[]		words;				// The words in the board
		public bool[]		foundWords;			// The words that have been found (index in foundWords corresponds to index in words)
		public TileState[]	tileStates;			// The state of each tile on the board
		public char[]		tileLetters;		// The letter that goes in each tile on the board
		public int			nextHintIndex;		// The index into words that indicates which word will have a letter shown when the hint button it clicked
		public List<int[]>	hintLettersShown;	// The letters that have been shown by hints. (List of int[2] where the first element is the word index and the second element is the letter index)
	}

	#endregion

	#region Inspector Variables

	[Tooltip("If this is checked then ads will play between levels.")]
	[SerializeField] private bool				enableAds;
	
	[Tooltip("The zone Id for the ad.")]
	[SerializeField] private string				zoneId;

	[Tooltip("This is the number of levels that must be completed before an ad is shown.")]
	[SerializeField] private int				adLevelCompleteAmount;

	[Tooltip("The number of hints that a player gets when they first start the game.")]
	[SerializeField] private int				startingHints;

	[Tooltip("The GameObject from the Hierarchy that has the LetterBoard component attached to it.")]
	[SerializeField] private LetterBoard		letterBoard;
	
	[Tooltip("The GameObject from the Hierarchy that has the WordGrid component attached to it.")]
	[SerializeField] private WordGrid			wordGrid;
	
	[Tooltip("The prefab from the Project folder that has the LetterTile component attached to it.")]
	[SerializeField] private LetterTile			letterTilePrefab;
	
	[Tooltip("All the categories that are in the game. Levels are assigned to categories.")]
	[SerializeField] private List<CategoryInfo>	categoryInfos;

	[Tooltip("A list of levels which will be randomly choosen from when a daily puzzle starts.")]
	[SerializeField] private List<LevelInfo>	dailyPuzzles;

	[Tooltip("The sprite that appears in the top bar when a daily puzzle is being played.")]
	[SerializeField] private Sprite				dailyPuzzleIcon;

    #endregion

    #region Member Variables

    public SimpleDic gSimpleDic;


    public static string dailyPuzzleId = "Daily Puzzle";

	private CategoryInfo dailyPuzzleInfo;
    public int[,] arrExamScore;
    public int g_iExamCount = 0;

    #endregion

    #region Properties

    public static string					SaveDataPath				{ get { return Application.persistentDataPath + "/save.dat"; } }
	public ObjectPool						LetterTilePool				{ get; private set; }
	public int								CurrentHints				{ get; private set; }
	public string							ActiveCategory				{ get; private set; }
    public bool     g_bExam;// { get; private set; }
	public int								ActiveLevelIndex			{ get; private set; }
	public int								ActiveDailyPuzzleIndex		{ get; private set; }
	public BoardState						ActiveBoardState			{ get; private set; }
	public Dictionary<string, BoardState>	SavedBoardStates			{ get; private set; }
	public Dictionary<string, bool>			CompletedLevels				{ get; private set; }
	public bool								AnimatingWord				{ get; private set; }
	public System.DateTime					NextDailyPuzzleAt			{ get; private set; }
	public int								LevelsToCompleteBeforeAd	{ get; private set; }

	public List<CategoryInfo> CategoryInfos
	{
		get
		{
			// If the list of Category Infos doesn't already contain a daily puzzle info then add it
			if (!categoryInfos.Contains(DailyPuzzleInfo))
			{
				categoryInfos.Add(DailyPuzzleInfo);
			}

			return categoryInfos;
		}
	}

	public CategoryInfo DailyPuzzleInfo
	{
		get
		{
			// Create a new CategoryInfo for the daily puzzles
			if (dailyPuzzleInfo == null)
			{
				dailyPuzzleInfo 			= new CategoryInfo();
				dailyPuzzleInfo.name		= dailyPuzzleId;
				dailyPuzzleInfo.icon		= dailyPuzzleIcon;
				dailyPuzzleInfo.levelInfos	= dailyPuzzles;
			}

			return dailyPuzzleInfo;
		}

		set
		{
			dailyPuzzleInfo = value;
		}
	}

	#endregion

	#region Unity Methods

	protected override void Awake()
	{
		base.Awake();

		LetterTilePool		= new ObjectPool(letterTilePrefab.gameObject, 16, transform);
		SavedBoardStates	= new Dictionary<string, BoardState>();
		CompletedLevels		= new Dictionary<string, bool>();

		// Load any save data
		if (!LoadSave())
		{
			// If there as not save data then set the current hints to the starting hints
			CurrentHints				= startingHints;
			LevelsToCompleteBeforeAd	= adLevelCompleteAmount;
			ActiveDailyPuzzleIndex		= -1;
            NextDailyPuzzleAt = new System.DateTime(System.DateTime.Now.Year, System.DateTime.Now.Month, System.DateTime.Now.Day);

        }

		// Initialize all our important things
		letterBoard.Initialize();
		wordGrid.Initialize();

		// Setup events
		letterBoard.OnWordFound += OnWordFound;

        gSimpleDic = new SimpleDic();
        gSimpleDic.LoadSimpleDic();

    }

    #endregion

    #region Public Methods

    public void StartLevel(string category, int levelIndex)
	{
        ActiveCategory = category;
        Utilities.LoadWordBoardEx(category);
  		ActiveLevelIndex	= levelIndex;
       
        string		boardId		= Utilities.FormatBoardId(ActiveCategory, ActiveLevelIndex);
        WordBoard wordBoard = Utilities.GetCurrentWordBoard(levelIndex);
 

        if (wordBoard == null)
		{
			Debug.LogError("Could not load WordBoard with the boardId: " + boardId);
			return;
		}

		// If a saved BoardState does not already exist then create one
		if (!SavedBoardStates.ContainsKey(boardId))
		{
			SavedBoardStates.Add(boardId, CreateNewBoardState(wordBoard));
		}

		// Try and get a saved board state if one exists
		ActiveBoardState = SavedBoardStates[boardId];
		
		// Save the game
		Save();
		
		// Setup the display using the assigned activeBoardState
		SetupActiveBoard();
	}

    public void RunExam()               // 
    {
        bool bExamed = false;
        int iRetry = 0;
        while (!bExamed && iRetry < 100)            
        {
            iRetry++;
            int max_complete_category = GameManager.Instance.GetMaxCategory();
            int categoryindex = Random.Range(0, max_complete_category);// GameManager.Instance.CategoryInfos.Count);
            string categoryName = GameManager.Instance.CategoryInfos[categoryindex].name;
            int levelIndex = Random.Range(0, Utilities.gWordBoard.Count);
            if (categoryName != "" && levelIndex >= 0 && levelIndex < 1000)
            {
                if (arrExamScore[categoryindex, levelIndex] < 10)
                {
                    arrExamScore[categoryindex, levelIndex]++;          // 플레이하면 1점 추가
                    StartLevel(categoryName, levelIndex);
                    bExamed = true;
                }
                else
                    continue;
            }
        }
    }

    public int GetMaxCategory() // 최대 달성 레벨의 카테고리 인덱스 리턴.
    {
        int category_index = 0;
        for(category_index = 0; category_index < CompletedLevels.Count; category_index++)
        {
            string _categorystring = CategoryInfos[category_index].name;
            int level_index = 0;
            for (level_index = 0; level_index < GameManager.Instance.CategoryInfos[category_index].levelInfos.Count; level_index++)
            {
                string _indexstring = _categorystring  + "_" + level_index.ToString();
                if (CompletedLevels.ContainsKey(_indexstring))
                    continue;
                else
                    break;
            }
            if (level_index < /*GameManager.Instance.CategoryInfos[category_index].levelInfos.Count / */2)    //특정 카테고리의 레벨이 해당 수준을 해결하면 시험으로 출제..
                break;
        }

        return category_index;
    }

    /// <summary>
    /// Starts the daily puzzle.
    /// </summary>
    public void StartDailyPuzzle()
	{
		if (dailyPuzzles.Count == 0)
		{
			return;
		}

		// Check if we need to pick a new daily puzzle
		if (ActiveDailyPuzzleIndex == -1 || System.DateTime.Now >= NextDailyPuzzleAt)
		{
			if (ActiveDailyPuzzleIndex != -1)
			{
				string boardId = Utilities.FormatBoardId(dailyPuzzleId, ActiveDailyPuzzleIndex);

				// Remove any save data for the previous daily puzzle
				SavedBoardStates.Remove(boardId);
				CompletedLevels.Remove(boardId);
			}

			// Get a new random daily puzzle level index to use
			ActiveDailyPuzzleIndex = Random.Range(0, dailyPuzzles.Count);
		}

		// Start the daily puzzle
		StartLevel(dailyPuzzleId, ActiveDailyPuzzleIndex);
	}

	/// <summary>
	/// Displays one letter in the WordGrid as a hint.
	/// </summary>
	public void DisplayNextHint()
	{
		if (ActiveBoardState != null && CurrentHints > 0)
		{
			// Call DisplayNextHint in wordGrid, giving it the last hint index that was displayed. DisplayNextHint will return the word and letter that was displayed
			int		hintWordIndex;
			int		hintLetterIndex;
			bool	hintDisplayed = wordGrid.DisplayNextHint(ref ActiveBoardState.nextHintIndex, out hintWordIndex, out hintLetterIndex);

			// Check if a hint was actually displayed
			if (hintDisplayed)
			{
				// Decrement the amount of hints
				CurrentHints--;

				// Update the board state so we know what letters where shown because of hints (so if the board is loaded from save we can lpace the hint words)
				ActiveBoardState.hintLettersShown.Add(new int[] { hintWordIndex, hintLetterIndex });

				// Save the game
				Save();
			}
		}
	}

    public void DisplayKorHint()
    {
        if (ActiveBoardState != null && CurrentHints > 0)
        {
            //CurrentHints--;

            List<string> vecHintWord = new List<string>();
            for (int i = 0; i < ActiveBoardState.words.Length; i++)
            {
                string _temp = ActiveBoardState.words[i];
                string _korword = gSimpleDic.GetWordKor(_temp);
                vecHintWord.Insert(i, _korword);
            }

            StartCoroutine(WaitThenHintScreen());

            //Save();
        }
    }

    private IEnumerator WaitThenHintScreen()
    {
        string hintWords = "";

        if (ActiveCategory == dailyPuzzleId)
        {
            string boardId = Utilities.FormatBoardId(ActiveCategory, ActiveLevelIndex);
            WordBoard wordBoard = Utilities.LoadWordBoard(boardId);
            hintWords = gSimpleDic.GetWordKor(wordBoard.words[0]);
            if (hintWords == null)
                hintWords = "? ? ?";
        }
        else
        {
            string boardId = Utilities.FormatBoardId(ActiveCategory, ActiveLevelIndex);

            // 힌트 텍스트 변경.

            for (int i = 0; i < ActiveBoardState.words.Length; i++)
            {
                string _temp = gSimpleDic.GetWordKor(ActiveBoardState.words[i]);
                if (_temp == null)
                    _temp = "? ? ?";

                hintWords += _temp;
                hintWords += "\r\n";
            }
        }

        UIScreenController.Instance.Show(UIScreenController.HintScreenId, true, true, true, Tween.TweenStyle.EaseIn, null/*OnCompleteScreenShown*/, hintWords);

        yield return new WaitForSeconds(3f);

       UIScreenController.Instance.HideOverlay(UIScreenController.HintScreenId, true, Tween.TweenStyle.EaseIn);
    }

    /// <summary>
    /// Resets the board so all the letters are back on the GameBoard and the WordGrid is only showing the hints.
    /// </summary>
    public void RestartBoard()
	{
		if (ActiveBoardState != null)
		{
			// Set all the words to not found on the BoardState
			for (int i = 0; i < ActiveBoardState.foundWords.Length; i++)
			{
				ActiveBoardState.foundWords[i] = false;
			}

			// Set all Found tile states back to UsedButNotFound
			for (int i = 0; i < ActiveBoardState.tileStates.Length; i++)
			{
				if (ActiveBoardState.tileStates[i] == BoardState.TileState.Found)
				{
					ActiveBoardState.tileStates[i] = BoardState.TileState.UsedButNotFound;
				}
			}

			// Save the game
			Save();

			SetupActiveBoard();
		}
	}

	/// <summary>
	/// Adds one hint to the current number of hints
	/// </summary>
	public void AddHint()
	{
		CurrentHints++;
		Save();
	}

	/// <summary>
	/// Returns true if the given category and level is completed
	/// </summary>
	public bool IsLevelCompleted(CategoryInfo categoryInfo, int levelIndex)
	{
		string boardId = Utilities.FormatBoardId(categoryInfo.name, levelIndex);
		return CompletedLevels.ContainsKey(boardId) && CompletedLevels[boardId];
	}

	/// <summary>
	/// Returns the number of completed levels for the given category
	/// </summary>
	public int GetCompletedLevelCount(CategoryInfo categoryInfo)
	{
		int numberOfCompletedLevels = 0;

		for (int i = 0; i < categoryInfo.levelInfos.Count; i++)
		{
			if (IsLevelCompleted(categoryInfo, i))
			{
				numberOfCompletedLevels++;
			}
		}

		return numberOfCompletedLevels;
	}

	/// <summary>
	/// Returns the CategoryInfo with the given category name.
	/// </summary>
	public CategoryInfo GetCategoryInfo(string categoryName)
	{
		for (int i = 0; i < CategoryInfos.Count; i++)
		{
			if (categoryName == CategoryInfos[i].name)
			{
				return CategoryInfos[i];
			}
		}

		return null;
	}

	#endregion

	#region Private Methods

	/// <summary>
	/// Called when the player finds a word
	/// </summary>
	private void OnWordFound(string word, List<LetterTile> letterTile, bool foundAllWords)
	{
		// Set all the tileStates for the found game tiles to BoardState.TileState.Found to indicate the tile has been found
		for (int i = 0; i < letterTile.Count; i++)
		{
			ActiveBoardState.tileStates[letterTile[i].TileIndex] = BoardState.TileState.Found;
		}

		// Set the flag of the word to found
		for (int i = 0; i < ActiveBoardState.words.Length; i++)
		{
			if (word == ActiveBoardState.words[i])
			{
				ActiveBoardState.foundWords[i] = true;
			}
		}

		// Save the game
		Save();

		// Cannot transition screens while a word is animating or bad things happen
		AnimatingWord = true;

		// Trasition the LetterTiles from the board to the word grid
		if (foundAllWords)
		{
			// If we found all the words then when the tile animation is finished call BoardComplete
			wordGrid.FoundWord(word, letterTile, (GameObject obj, object[] objs) => {
				BoardComplete();
				AnimatingWord = false;
			});
		}
		else
		{
			wordGrid.FoundWord(word, letterTile, (GameObject obj, object[] objs) => {
				AnimatingWord = false;
			});
		}
	}

	/// <summary>
	/// Sets up the GameBoard and WordGrid using the current active BoardState
	/// </summary>
	private void SetupActiveBoard()
	{
		if (ActiveBoardState == null)
		{
			Debug.LogError("[GameManager] No activeBoardState when SetupActiveBoard was called.");

			return;
		}

		// Setup the GameBoard and WordGrid
		letterBoard.Setup(ActiveBoardState);
		wordGrid.Setup(ActiveBoardState);
	}

	/// <summary>
	/// Creates a new BoardState object using the values defined in the given WordBoard
	/// </summary>
	private BoardState CreateNewBoardState(WordBoard wordBoard)
	{
		BoardState boardState = new BoardState();

		boardState.wordBoardId		= wordBoard.id;
		boardState.wordBoardSize	= wordBoard.size;
		boardState.words			= wordBoard.words;
		boardState.nextHintIndex	= 0;

		boardState.foundWords		= new bool[wordBoard.words.Length];
		boardState.tileLetters		= new char[wordBoard.wordTiles.Length];
		boardState.tileStates		= new BoardState.TileState[wordBoard.wordTiles.Length];
		boardState.hintLettersShown = new List<int[]>();

		for (int i = 0; i < boardState.tileStates.Length; i++)
		{
			boardState.tileLetters[i]	= wordBoard.wordTiles[i].hasLetter ? wordBoard.wordTiles[i].letter : (char)0;
			boardState.tileStates[i]	= wordBoard.wordTiles[i].hasLetter ? BoardState.TileState.UsedButNotFound : BoardState.TileState.NotUsed;
		}

		return boardState;
	}

	/// <summary>
	/// Called when the current active board is completed by the player (ie. they found the last word)
	/// </summary>
	private void BoardComplete()
	{
        StartCoroutine(WaitThenCompleteScreen());

    }

	private void OnCompleteScreenShown()
	{
        if (g_bExam)    // for exam
        {
            RunExam();
            if (g_iExamCount >= 10)
            {

                g_bExam = false;

            }
            else
            {
                g_iExamCount++;
             }
        }
        else            // for normal mode
        {
            CategoryInfo categoryInfo = GetCategoryInfo(ActiveCategory);
            int nextLevelIndex = ActiveLevelIndex + 1;

            // Check if the category has been completed or it was the daily puzzle
            if (ActiveCategory == dailyPuzzleId || nextLevelIndex >= categoryInfo.levelInfos.Count)
            {

                // If we completed the daily puzzle then move back to the main screen else move to the categories screen
                string screenToShow = (ActiveCategory == dailyPuzzleId) ? UIScreenController.MainScreenId : UIScreenController.CategoriesScreenId;

                // Set the active category to nothing
                ActiveCategory = "";
                ActiveLevelIndex = -1;

                Save();

                // Force the category screen to show right away (behind the now showing overlay)
                UIScreenController.Instance.Show(screenToShow, true, false);
            }
            else
            {
                // Start the next level
                StartLevel(ActiveCategory, nextLevelIndex);
            }
        }
		StartCoroutine(WaitThenHideCompleteScreen());
	}

    private IEnumerator WaitThenCompleteScreen()
    {
        yield return new WaitForSeconds(2f);

        string boardId = Utilities.FormatBoardId(ActiveCategory, ActiveLevelIndex);
        bool awardHint = true;

        // Check if the completed category was a daily puzzle, if so check if we want to award a hint
        if (ActiveCategory != dailyPuzzleId)
        {
            // Get the CategoryInfo for the active category
            CategoryInfo categoryInfo = GetCategoryInfo(ActiveCategory);

            bool categoryCompleted = (GetCompletedLevelCount(categoryInfo) == categoryInfo.levelInfos.Count);

            // Award a hint if the category was just completed
            awardHint = ((!CompletedLevels.ContainsKey(boardId) || !CompletedLevels[boardId]) && categoryCompleted);
        }
        else
        {
            // Set the next daily puzzle to start at the start of the next day
            NextDailyPuzzleAt = new System.DateTime(System.DateTime.Now.Year, System.DateTime.Now.Month, System.DateTime.Now.Day, System.DateTime.Now.Hour, System.DateTime.Now.Minute, System.DateTime.Now.Second).AddHours(1);//.AddDays(1);
        }

        // Award a hint for completing the category or daily puzzle
        if (awardHint)
        {
            AddHint();
        }

        // Set the completed flag on the level
        CompletedLevels[boardId] = true;

        // The board has been completed, we no longer need to save it
        ActiveBoardState = null;

        // Remove the BoardState from the list of saved BoardStates
        SavedBoardStates.Remove(boardId);

        Save();

        LevelsToCompleteBeforeAd -= 1;

        UIScreenController.Instance.Show(UIScreenController.CompleteScreenId, false, true, true, Tween.TweenStyle.EaseOut, OnCompleteScreenShown, awardHint);

        AddHint();

    }

    private IEnumerator WaitThenHideCompleteScreen()
	{
		yield return new WaitForSeconds(1f);

#if UNITY_ADS
        // If Unity Ads is on and the player completed the proper amount of levels then show an ad
        if (enableAds && LevelsToCompleteBeforeAd <= 0)
        {
            LevelsToCompleteBeforeAd = adLevelCompleteAmount;

            UnityEngine.Advertisements.ShowOptions adShowOptions = new UnityEngine.Advertisements.ShowOptions();

            // Show the ad first and when its done then hide the complete overlay
            adShowOptions.resultCallback = (UnityEngine.Advertisements.ShowResult adShowResult) =>
            {
                UIScreenController.Instance.HideOverlay(UIScreenController.CompleteScreenId, true, Tween.TweenStyle.EaseIn);
            };

            UnityEngine.Advertisements.Advertisement.Show(zoneId, adShowOptions);
        }
        else
        {
            UIScreenController.Instance.HideOverlay(UIScreenController.CompleteScreenId, true, Tween.TweenStyle.EaseIn);
        }
#else
		UIScreenController.Instance.HideOverlay(UIScreenController.CompleteScreenId, true, Tween.TweenStyle.EaseIn);
#endif
    }

    /// <summary>
    /// Saves the game
    /// </summary>
    private void Save()
	{
		Dictionary<string, object> jsonObj = new Dictionary<string, object>();

		jsonObj.Add("currentHints", CurrentHints);
		jsonObj.Add("activeCategory", ActiveCategory);
		jsonObj.Add("activeLevelIndex", ActiveLevelIndex);
		jsonObj.Add("ActiveDailyPuzzleIndex", ActiveDailyPuzzleIndex);
		jsonObj.Add("NextDailyPuzzleAt", NextDailyPuzzleAt.ToString("yyyyMMdd"));
		jsonObj.Add("LevelsToCompleteBeforeAd", LevelsToCompleteBeforeAd);

		// Get all the saved board states
		List<object> savedBoardStatesObj = new List<object>();

		foreach (KeyValuePair<string, BoardState> pair in SavedBoardStates)
		{
			savedBoardStatesObj.Add(ConvertBoardStateToJson(pair.Value));
		}

		jsonObj.Add("savedBoardStates", savedBoardStatesObj);

		// Get all the completed levels
		List<object> completedLevelsObj = new List<object>();

		foreach (KeyValuePair<string, bool> pair in CompletedLevels)
		{
			if (pair.Value)
			{
				completedLevelsObj.Add(pair.Key);
			}
		}

		jsonObj.Add("completedLevels", completedLevelsObj);

		// Now convert the jsonObj to a json string and save it to the save file
		System.IO.File.WriteAllText(SaveDataPath, Utilities.ConvertToJsonString(jsonObj));
	}

	/// <summary>
	/// Loads the save game
	/// </summary>
	private bool LoadSave()
	{
		if (File.Exists(SaveDataPath))
		{
			string		jsonStr	= System.IO.File.ReadAllText(SaveDataPath);
			JSONNode	json	= JSON.Parse(jsonStr);

			// Load the number of current hints
			CurrentHints = json["currentHints"].AsInt;

			// Parse the saved board states
			JSONArray savedBoardStatesJson = json["savedBoardStates"].AsArray;
			SavedBoardStates = new Dictionary<string, BoardState>(savedBoardStatesJson.Count);

			for (int i = 0; i < savedBoardStatesJson.Count; i++)
			{
				BoardState boardState = CreateBoardStateFromJson(savedBoardStatesJson[i]);
				SavedBoardStates.Add(boardState.wordBoardId, boardState);
			}

			// Get the active category and level index
			ActiveCategory				= json["activeCategory"].Value;
			ActiveLevelIndex			= json["activeLevelIndex"].AsInt;
			ActiveDailyPuzzleIndex		= json["ActiveDailyPuzzleIndex"].AsInt;
			LevelsToCompleteBeforeAd	= json["LevelsToCompleteBeforeAd"].AsInt;

			// Get the next daily puzzle at time
			string time = json["NextDailyPuzzleAt"].Value;

			if (string.IsNullOrEmpty(time))
			{
				NextDailyPuzzleAt = System.DateTime.Now;
			}
			else
			{
				NextDailyPuzzleAt = System.DateTime.ParseExact(time, "yyyyMMdd", System.Globalization.CultureInfo.InvariantCulture);
			}

			// Parse the completed levels
			JSONArray completedLevelsJson = json["completedLevels"].AsArray;

			for (int i = 0; i < completedLevelsJson.Count; i++)
			{
				CompletedLevels[completedLevelsJson[i].Value] = true;
			}

			return true;
		}

		return false;
	}

	private Dictionary<string, object> ConvertBoardStateToJson(BoardState boardState)
	{
		Dictionary<string, object> jsonObj = new Dictionary<string, object>();

		jsonObj.Add("wordBoardId", boardState.wordBoardId);
		jsonObj.Add("wordBoardSize", boardState.wordBoardSize);
		jsonObj.Add("NextDailyPuzzleAt", boardState.nextHintIndex);

		// Get all the words
		List<object> wordsObj = new List<object>();

		for (int i = 0; i < boardState.words.Length; i++)
		{
			wordsObj.Add(boardState.words[i]);
		}

		jsonObj.Add("words", wordsObj);

		// Get all the found words
		List<object> foundWordsObj = new List<object>();

		for (int i = 0; i < boardState.foundWords.Length; i++)
		{
			foundWordsObj.Add(boardState.foundWords[i]);
		}

		jsonObj.Add("foundWords", foundWordsObj);

		// Get all the tiel states
		List<object> tileStatesObj = new List<object>();
		
		for (int i = 0; i < boardState.tileStates.Length; i++)
		{
			tileStatesObj.Add((int)boardState.tileStates[i]);
		}
		
		jsonObj.Add("tileStates", tileStatesObj);

		// Get all the tile letters
		List<object> tileLettersObj = new List<object>();
		
		for (int i = 0; i < boardState.tileLetters.Length; i++)
		{
			tileLettersObj.Add(boardState.tileLetters[i].ToString());
		}
		
		jsonObj.Add("tileLetters", tileLettersObj);

		// Get all the hint letters shown
		List<object> hintLettersObj = new List<object>();

		for (int i = 0; i < boardState.hintLettersShown.Count; i++)
		{
			hintLettersObj.Add(string.Format("{0},{1}", boardState.hintLettersShown[i][0], boardState.hintLettersShown[i][1]));
		}

		jsonObj.Add("hintLetters", hintLettersObj);

		return jsonObj;
	}

	private BoardState CreateBoardStateFromJson(JSONNode json)
	{
		BoardState boardState = new BoardState();

		boardState.wordBoardId		= json["wordBoardId"].Value;
		boardState.wordBoardSize	= json["wordBoardSize"].AsInt;
		boardState.nextHintIndex	= json["nextHintIndex"].AsInt;

		// Parse the words
		JSONArray wordsJson	= json["words"].AsArray;
		boardState.words	= new string[wordsJson.Count];

		for (int i = 0; i < wordsJson.Count; i++)
		{
			boardState.words[i] = wordsJson[i].Value;
		}

		// Parse the found words
		JSONArray foundWordsJson	= json["foundWords"].AsArray;
		boardState.foundWords		= new bool[foundWordsJson.Count];
		
		for (int i = 0; i < foundWordsJson.Count; i++)
		{
			boardState.foundWords[i] = foundWordsJson[i].AsBool;
		}

		// Parse the tile states
		JSONArray tileStatesJson	= json["tileStates"].AsArray;
		boardState.tileStates		= new BoardState.TileState[tileStatesJson.Count];
		
		for (int i = 0; i < tileStatesJson.Count; i++)
		{
			boardState.tileStates[i] = (BoardState.TileState)tileStatesJson[i].AsInt;
		}

		// Parse the tile lettes
		JSONArray tileLettersJson	= json["tileLetters"].AsArray;
		boardState.tileLetters		= new char[tileLettersJson.Count];
		
		for (int i = 0; i < tileLettersJson.Count; i++)
		{
			boardState.tileLetters[i] = System.Convert.ToChar(tileLettersJson[i].Value);
		}

		// Parse the hint letters
		JSONArray hintLettersJson	= json["hintLetters"].AsArray;
		boardState.hintLettersShown	= new List<int[]>(hintLettersJson.Count);
		
		for (int i = 0; i < hintLettersJson.Count; i++)
		{
			string[] hintLetter = hintLettersJson[i].Value.Split(',');
			boardState.hintLettersShown.Add(new int[2] { System.Convert.ToInt32(hintLetter[0]), System.Convert.ToInt32(hintLetter[1])} );
		}

		return boardState;
	}

	#endregion
}
