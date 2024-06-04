/*
Created: 25.05.2024
Modified: 25.05.2024
Project: MessengerDatabaseModel
Model: SQLite
Database: SQLite 3.32
*/

-- Create tables section -------------------------------------------------

-- Table user

CREATE TABLE user
(
  id_user INTEGER NOT NULL,
  login TEXT,
  password TEXT,
  surname TEXT,
  user_name TEXT,
  id_user_rights INTEGER NOT NULL,
  CONSTRAINT user_identifier PRIMARY KEY (id_user),
  CONSTRAINT user_rights_dependence
    FOREIGN KEY (id_user_rights)
    REFERENCES user_rights (id_user_rights)
)
;

CREATE INDEX IX_user_rights_dependence
  ON user (id_user_rights)
;

-- Table user_rights

CREATE TABLE user_rights
(
  id_user_rights INTEGER NOT NULL,
  user_rights_name TEXT,
  description TEXT,
  CONSTRAINT user_rights_identifier PRIMARY KEY (id_user_rights)
)
;

-- Table chat

CREATE TABLE chat
(
  id_chat INTEGER NOT NULL,
  char_name TEXT,
  id_chat_type INTEGER NOT NULL,
  CONSTRAINT chat_identifier PRIMARY KEY (id_chat),
  CONSTRAINT chat_type_dependence
    FOREIGN KEY (id_chat_type)
    REFERENCES chat_type (id_chat_type)
)
;

CREATE INDEX IX_chat_type_dependence
  ON chat (id_chat_type)
;

-- Table message

CREATE TABLE message
(
  id_message INTEGER NOT NULL,
  message_time timestamp,
  message_text TEXT,
  id_chat INTEGER NOT NULL,
  id_user INTEGER NOT NULL,
  CONSTRAINT message_identifier PRIMARY KEY (id_message),
  CONSTRAINT char_messages_dependence
    FOREIGN KEY (id_chat)
    REFERENCES chat (id_chat),
  CONSTRAINT user_message_dependence
    FOREIGN KEY (id_user)
    REFERENCES user (id_user)
)
;

CREATE INDEX IX_user_message_dependence
  ON message (id_user)
;

-- Table chat_member

CREATE TABLE chat_member
(
  id_user INTEGER NOT NULL,
  id_chat INTEGER NOT NULL,
  CONSTRAINT chat_member_identifier PRIMARY KEY (id_user,id_chat),
  CONSTRAINT user_member_chat_dependence
    FOREIGN KEY (id_user)
    REFERENCES user (id_user),
  CONSTRAINT members_chat_dependence
    FOREIGN KEY (id_chat)
    REFERENCES chat (id_chat)
)
;

-- Table unread_message

CREATE TABLE unread_message
(
  id_user INTEGER NOT NULL,
  id_message INTEGER NOT NULL,
  CONSTRAINT unread_message_identifier PRIMARY KEY (id_user,id_message),
  CONSTRAINT user_unread_message_dependence
    FOREIGN KEY (id_user)
    REFERENCES user (id_user),
  CONSTRAINT unread_message_list_dependence
    FOREIGN KEY (id_message)
    REFERENCES message (id_message)
)
;

-- Table chat_type

CREATE TABLE chat_type
(
  id_chat_type INTEGER NOT NULL,
  chat_type_name TEXT,
  CONSTRAINT chat_type_identifier PRIMARY KEY (id_chat_type)
)
;

