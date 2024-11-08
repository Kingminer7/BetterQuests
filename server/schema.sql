DROP TABLE IF EXISTS Quests;
CREATE TABLE
  IF NOT EXISTS Quests (
    Id INTEGER PRIMARY KEY,
    Name TEXT DEFAULT 'Unnamed Quest',
    Reward INTEGER DEFAULT 0,
    Difficulty TEXT DEFAULT 'None',
    Type TEXT DEFAULT 'None',
    Specifications OBJECT DEFAULT '{}'
  );

DROP TABLE IF EXISTS Roles;
CREATE TABLE
  IF NOT EXISTS Roles (
    Id INTEGER PRIMARY KEY,
    Name TEXT DEFAULT 'Unnamed Role',
    Permissions OBJECT DEFAULT '{}',
    NameColor TEXT DEFAULT '255,255,255'
  );

DROP TABLE IF EXISTS Users;
CREATE TABLE
  IF NOT EXISTS USERS (
    Id INTEGER PRIMARY KEY,
    Name TEXT DEFAULT 'Unknown',
    GdId INTEGER DEFAULT 0,
    Trophies INTEGER DEFAULT 0,
    QuestsCompleted INTEGER DEFAULT 0,
    Roles ARRAY DEFAULT '[1]'
  );

INSERT INTO
  Roles (Name, Permissions, NameColor)
VALUES
  ('User', '{"SubmitToLeaderboard": true}', '255,255,255'),
  ('Banned', '{"SubmitToLeaderboard": false}', '50,50,50'),
  ('Moderator', '{"BanPlayers": true}', '232,72,72'),
  ('Moderator', '{"ManageQuests": true, "ManageLevels": true}', '237,156,50'),

INSERT INTO
  Quests (Name, Reward, Difficulty, Type, Specifications)
VALUES
  ('Test Quest', 2, 'Easy', 'Test', '{"test": "true"}');