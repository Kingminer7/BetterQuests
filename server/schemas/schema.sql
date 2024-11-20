-- DROP TABLE IF EXISTS Quests;
-- DROP TABLE IF EXISTS Roles;
-- DROP TABLE IF EXISTS Users;
-- DROP TABLE IF EXISTS GeneralData;
-- DROP TABLE IF EXISTS Levels;

CREATE TABLE
	IF NOT EXISTS Quests (
		Id INTEGER PRIMARY KEY AUTOINCREMENT,
		Name TEXT DEFAULT 'Unnamed Quest',
		Description TEXT DEFAULT 'Unknown',
		Reward INTEGER DEFAULT 0,
		Difficulty TEXT DEFAULT 'None',
		Type TEXT DEFAULT 'None',
		Quantity INTEGER DEFAULT 0,
		Specifications JSON DEFAULT '{}'
	);

CREATE TABLE
	IF NOT EXISTS QueuedQuests (
		Id INTEGER PRIMARY KEY AUTOINCREMENT,
		Name TEXT DEFAULT 'Unnamed Quest',
		Description TEXT DEFAULT 'Unknown',
		Reward INTEGER DEFAULT 0,
		Difficulty TEXT DEFAULT 'None',
		Type TEXT DEFAULT 'None',
		Quantity INTEGER DEFAULT 0,
		Specifications JSON DEFAULT '{}'
	);

CREATE TABLE
	IF NOT EXISTS Levels (
		Id INTEGER PRIMARY KEY AUTOINCREMENT,
		Name TEXT DEFAULT 'Unknown',
		QuestName TEXT DEFAULT '',
		LevelId INTEGER DEFAULT 0,
		Difficulty TEXT DEFAULT 'None'
	);

CREATE TABLE
	IF NOT EXISTS QueuedLevels (
		Id INTEGER PRIMARY KEY AUTOINCREMENT,
		Name TEXT DEFAULT 'Unknown',
		QuestName TEXT DEFAULT '',
		LevelId INTEGER DEFAULT 0,
		Difficulty TEXT DEFAULT 'None'
	);

CREATE TABLE
	IF NOT EXISTS Roles (
		Id INTEGER PRIMARY KEY AUTOINCREMENT,
		Name TEXT DEFAULT 'Unnamed Role',
		Permissions JSON DEFAULT '{}',
		NameColor TEXT DEFAULT '255,255,255'
	);

CREATE TABLE
	IF NOT EXISTS Users (
		Id INTEGER PRIMARY KEY AUTOINCREMENT,
		Name TEXT DEFAULT 'Unknown',
		GdId INTEGER DEFAULT 0,
		Trophies INTEGER DEFAULT 0,
		QuestsCompleted INTEGER DEFAULT 0,
		Roles JSON DEFAULT '["1"]'
	);

CREATE TABLE
  IF NOT EXISTS Auth {
    UserId INTEGER PRIMARY KEY,
    Token TEXT,
    Expires INTEGER
  };

CREATE TABLE
  IF NOT EXISTS AuthRequests {
    UserId INTEGER PRIMARY KEY,
    Challenge TEXT,
    Expires INTEGER
  };

CREATE TABLE
	IF NOT EXISTS GeneralData (
		Id INTEGER PRIMARY KEY AUTOINCREMENT,
		MaximumTrophies INTEGER DEFAULT 0,
		LastRequestDate INTEGER DEFAULT 0,
		LastUpdeasy INTEGER DEFAULT 0,
		LastUpdmedium INTEGER DEFAULT 0,
		LastUpdhard INTEGER DEFAULT 0,
		LastUpdexpert INTEGER DEFAULT 0,
		LastUpdmaster INTEGER DEFAULT 0,
		LastUpdgrandmaster INTEGER DEFAULT 0
	);

INSERT INTO
	Roles (Name, Permissions, NameColor)
VALUES
	(
		'User',
		'{"SubmitToLeaderboard": true}',
		'255,255,255'
	),
	(
		'Banned',
		'{"SubmitToLeaderboard": false}',
		'50,50,50'
	),
	(
		'Moderator',
		'{"BanPlayers": true}',
		'232,72,72'
	),
	(
		'Manager',
		'{"ManageQuests": true, "ManageLevels": true}',
		'237,156,50'
	);

INSERT INTO
	Quests (
		Name,
		Description,
		Reward,
		Difficulty,
		Type,
		Quantity,
		Specifications
	)
VALUES
	(
		'Level Master!',
		"Beat {{levelname}} (id {{levelid}}).",
		2,
		'easy',
		'BeatLevel',
		1,
		'{"id": {{levelid}}}'
	);

INSERT INTO
	Levels (
		Name,
		QuestName,
		LevelId,
		Difficulty
	)
VALUES
	(
		'Tidal Wave',
		'Dap Dodum Doo',
		86407629,
		'easy'
	);

INSERT INTO
	GeneralData (
		Id
	)
VALUES
	(
		0
	);
