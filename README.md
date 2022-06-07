

# ECE141b -- Relational Database

This project is to build a relational database system from scratch that follows MVC pattern.

[TOC]

## Overview

Compile and run with no arguments, the system should be ready for inputs.

The following arguments are automated tests, please use them once at a time.

```
App, Compile, DB, Tables, Drop, Insert, Select, Join, Update, Delete, Index, Alter
```

## Working With This Database System

Create an Application instance, create an `std::istream` instance with input commands and call `Application::handleInput(std::istream &anInput)` method, the rest will be taken care of.

`StatusResult` is a struct that holds error message if occurs, check `Errors.hpp` for error codes.

`Config.hpp` store the path to databases and the timer. Windows user need to change Line 23 in `Config.hpp` before running the code.

## Supporting Commands

### Application Level

```sql
help;
version;
quit;
```

Help command is just a place holder for future implementation, no existing helping system is implemented.

### Database Level

```sql
CREATE DATABASE {db-name};
SHOW DATABASES;
USE {db-name};
DROP DATABASE {db-name};
DUMP DATABASE {db-name};
```

These commands relate to creating, listing, managing database containers, deleting the database and show internal blocks.

### Table Level

#### Create Table

This command will create a table.

```sql
CREATE TABLE {table-name};
```

Below is an example to create a table:

```sql
> CREATE DATABASE library;
Query OK, 1 row affected (0.001 secs)

> USE library;
Database changed

> CREATE TABLE Users (id int NOT NULL auto_increment primary key, first_name varchar(50) NOT NULL, last_name varchar(50), age int, zipcode int);
Query OK, 1 row affected (0.001 sec)

> CREATE TABLE Books (id int NOT NULL auto_increment primary key, title varchar(25) NOT NULL, subtitle varchar(25), user_id int);
Query OK, 1 row affected (0.001 sec)
```

In the table definition shown above, `first_name` is an example `Attribute`. The attribute class needs to be able to store the following information:

```
- field_name
- field_type  (bool, float, integer, timestamp, varchar)  //varchar has length
- field_length (only applies to text fields)
- auto_increment (determines if this (integer) field is autoincremented by DB
- primary_key  (bool indicates that field represents primary key)
- nullable (bool indicates the field can be null)
```

#### Show Tables

This command will show all the tables in the database.

```sql
> USE library;
Database changed

> SHOW TABLES;
+----------------------+
| Tables_in_library    |
+----------------------+
| Books                |
| Users                |
+----------------------+
2 rows in set (0.002 sec)
```

#### Drop Table

This command will delete the associated table.

```sql
> DROP TABLE {table-name};
Query OK, 0 rows affected (0.02 sec)
```

#### Describe Table

This command will attempt to locate and describe the associated `Schema`.

```sql
> DESCRIBE Users;
+----------------+--------------+------+-----+---------+-----------------------------+
| Field          | Type         | Null | Key | Default | Extra                       |
+----------------+--------------+------+-----+---------+-----------------------------+
| id             | int          | NO   | YES | NULL    |                             |
| first_name     | varchar(50)  | NO   |     | NULL    |                             |
| last_name      | varchar(50)  | YES  |     | NULL    |                             |
| age            | int          | YES  |     | NULL    |                             |
| zipcode        | int          | YES  |     | NULL    |                             |
+----------------+--------------+------+-----+---------+-----------------------------+
5 rows in set (0.008 sec)
```

### SQL Level

#### Insert

The `insert` command allows a user to insert (one or more) records into a given table. The command accepts a list of field, and a collection of value lists -- one for each record you want to insert. 

```sql
> USE library;
Database changed

> INSERT INTO Users (first_name, last_name, age, zipcode) VALUES ("Terry", "Pratchett", 70,  92124), ("Ian", "Tregellis", 48,  92123), ("Jody", "Taylor",    50,  92120), ("Stephen",   "King", 74,  92125), ("Ted",  "Chiang",    56,  92120), ("Anthony", "Doerr",52,  92122), ("J.R.R.", "Tolkien",   130, 92126), ("Aurthur C.","Clarke",    105, 92127), ("Seldon", "Edwards",   81,  92128), ("Neal","Stephenson",62,  92121);
Query OK, 10 rows affected (0.002 sec)

> INSERT INTO Books (title, user_id) VALUES ("The Green Mile",4), ("The Stand",4), ("Misery",4), ("11/22/63",4), ("The Institute",4), ("Sorcerer",1), ("Wintersmith",1), ("Mort",1), ("Thud",1), ("Time Police",3), ("The Mechanical",2), ("The Liberation",2), ("The Rising",2), ("Exhalation",5);
Query OK, 14 rows affected (0.002 sec)
```

#### Select

The `SELECT` command allows a user to retrieve (one or more) records from a given table. The command accepts one or more fields to be retrieved (or the wildcard `*`), along with a series of **optional** arguments (e.g. `ORDER BY`, `LIMIT`). Below, are examples of the basic `SELECT` statement (presumes the existence of a `Users` and `Accounts` table).

```sql
> USE library;
Database changed

> SELECT * FROM Users;
+---------+------------+------------+---------+---------+
| id      | first_name | last_name  | age     | zipcode |
+---------+------------+------------+---------+---------+
| 1       | Terry      | Pratchett  | 70      | 92124   |
| 2       | Ian        | Tregellis  | 48      | 92123   |
| 3       | Jody       | Taylor     | 50      | 92120   |
| 4       | Stephen    | King       | 74      | 92125   |
| 5       | Ted        | Chiang     | 56      | 92120   |
| 6       | Anthony    | Doerr      | 52      | 92122   |
| 7       | J.R.R.     | Tolkien    | 130     | 92126   |
| 8       | Aurthur C. | Clarke     | 105     | 92127   |
| 9       | Seldon     | Edwards    | 81      | 92128   |
| 10      | Neal       | Stephenson | 62      | 92121   |
+---------+------------+------------+---------+---------+
10 rows in set (0.019 sec)

> SELECT * FROM Books;
+---------+----------------+----------+---------+
| id      | title          | subtitle | user_id |
+---------+----------------+----------+---------+
| 1       | The Green Mile | NULL     | 4       |
| 2       | The Stand      | NULL     | 4       |
| 3       | Misery         | NULL     | 4       |
| 4       | 11/22/63       | NULL     | 4       |
| 5       | The Institute  | NULL     | 4       |
| 6       | Sorcerer       | NULL     | 1       |
| 7       | Wintersmith    | NULL     | 1       |
| 8       | Mort           | NULL     | 1       |
| 9       | Thud           | NULL     | 1       |
| 10      | Time Police    | NULL     | 3       |
| 11      | The Mechanical | NULL     | 2       |
| 12      | The Liberation | NULL     | 2       |
| 13      | The Rising     | NULL     | 2       |
| 14      | Exhalation     | NULL     | 5       |
+---------+----------------+----------+---------+
14 rows in set (0.023 sec)
```

- Select...WHERE ...

The records to be filtered according to the given WHERE clause, using a `Filter` object.

```SQL
> SELECT * FROM Users WHERE age>70;
+---------+------------+-----------+---------+---------+
| id      | first_name | last_name | age     | zipcode |
+---------+------------+-----------+---------+---------+
| 4       | Stephen    | King      | 74      | 92125   |
| 7       | J.R.R.     | Tolkien   | 130     | 92126   |
| 8       | Aurthur C. | Clarke    | 105     | 92127   |
| 9       | Seldon     | Edwards   | 81      | 92128   |
+---------+------------+-----------+---------+---------+
4 rows in set (0.011 sec)
```

- Select...ORDER BY `fieldname` ...

The rows will shown in order.

```SQL
> SELECT * FROM Users ORDER BY age;
+---------+------------+------------+---------+---------+
| id      | first_name | last_name  | age     | zipcode |
+---------+------------+------------+---------+---------+
| 2       | Ian        | Tregellis  | 48      | 92123   |
| 3       | Jody       | Taylor     | 50      | 92120   |
| 6       | Anthony    | Doerr      | 52      | 92122   |
| 5       | Ted        | Chiang     | 56      | 92120   |
| 10      | Neal       | Stephenson | 62      | 92121   |
| 1       | Terry      | Pratchett  | 70      | 92124   |
| 4       | Stephen    | King       | 74      | 92125   |
| 9       | Seldon     | Edwards    | 81      | 92128   |
| 8       | Aurthur C. | Clarke     | 105     | 92127   |
| 7       | J.R.R.     | Tolkien    | 130     | 92126   |
+---------+------------+------------+---------+---------+
10 rows in set (0.022 sec)
```

- Select...LIMIT *N* ...

```SQL
> SELECT * FROM Users ORDER BY age LIMIT 4;
+---------+------------+------------+---------+---------+
| id      | first_name | last_name  | age     | zipcode |
+---------+------------+------------+---------+---------+
| 2       | Ian        | Tregellis  | 48      | 92123   |
| 3       | Jody       | Taylor     | 50      | 92120   |
| 6       | Anthony    | Doerr      | 52      | 92122   |
| 5       | Ted        | Chiang     | 56      | 92120   |
+---------+------------+------------+---------+---------+
4 rows in set (0.013 sec)
```

- All in One

```
> SELECT * FROM Users ORDER BY age LIMIT 4 WHERE age>70;
+---------+------------+-----------+---------+---------+
| id      | first_name | last_name | age     | zipcode |
+---------+------------+-----------+---------+---------+
| 4       | Stephen    | King      | 74      | 92125   |
| 9       | Seldon     | Edwards   | 81      | 92128   |
| 8       | Aurthur C. | Clarke    | 105     | 92127   |
| 7       | J.R.R.     | Tolkien   | 130     | 92126   |
+---------+------------+-----------+---------+---------+
4 rows in set (0.014 sec)
```

#### Update

The `UPDATE` command allows a user to select records from a given table, alter those records in memory, and save the records back out to the storage file.

```SQL
> UPDATE Users SET "zipcode" = 92127 WHERE zipcode>92125;
Query Ok. 3 rows in set (0.002 sec)

> SELECT * FROM Users;
+---------+------------+------------+---------+---------+
| id      | first_name | last_name  | age     | zipcode |
+---------+------------+------------+---------+---------+
| 1       | Terry      | Pratchett  | 70      | 92124   |
| 2       | Ian        | Tregellis  | 48      | 92123   |
| 3       | Jody       | Taylor     | 50      | 92120   |
| 4       | Stephen    | King       | 74      | 92125   |
| 5       | Ted        | Chiang     | 56      | 92120   |
| 6       | Anthony    | Doerr      | 52      | 92122   |
| 7       | J.R.R.     | Tolkien    | 130     | 92127   |
| 8       | Aurthur C. | Clarke     | 105     | 92127   |
| 9       | Seldon     | Edwards    | 81      | 92127   |
| 10      | Neal       | Stephenson | 62      | 92121   |
+---------+------------+------------+---------+---------+
10 rows in set (0.022 sec)
```

#### Delete

The `DELETE` command allows a user to select records from a given table, and remove those rows from `Storage`.

```sql
> DELETE FROM Users WHERE zipcode=92127;
Query Ok. 3 rows in set (0.001 sec)

> SELECT * FROM Users;
+---------+------------+------------+---------+---------+
| id      | first_name | last_name  | age     | zipcode |
+---------+------------+------------+---------+---------+
| 1       | Terry      | Pratchett  | 70      | 92124   |
| 2       | Ian        | Tregellis  | 48      | 92123   |
| 3       | Jody       | Taylor     | 50      | 92120   |
| 4       | Stephen    | King       | 74      | 92125   |
| 5       | Ted        | Chiang     | 56      | 92120   |
| 6       | Anthony    | Doerr      | 52      | 92122   |
| 10      | Neal       | Stephenson | 62      | 92121   |
+---------+------------+------------+---------+---------+
7 rows in set (0.015 sec)
```

#### Joins

- Left Join

The `LEFT JOIN` selects all data from the left table whether there are matching rows exist in the right table or not.

```SQL
> SELECT last_name, title FROM Users LEFT JOIN Books ON Users.id=Books.user_id;
+---------+------------+----------------+
| id      | last_name  | title          |
+---------+------------+----------------+
| 1       | Pratchett  | Sorcerer       |
| 1       | Pratchett  | Sorcerer       |
| 1       | Pratchett  | Sorcerer       |
| 1       | Pratchett  | Sorcerer       |
| 2       | Tregellis  | The Mechanical |
| 2       | Tregellis  | The Mechanical |
| 2       | Tregellis  | The Mechanical |
| 3       | Taylor     | Time Police    |
| 4       | King       | The Green Mile |
| 4       | King       | The Green Mile |
| 4       | King       | The Green Mile |
| 4       | King       | The Green Mile |
| 4       | King       | The Green Mile |
| 5       | Chiang     | Exhalation     |
| 6       | Doerr      | NULL           |
| 7       | Tolkien    | NULL           |
| 8       | Clarke     | NULL           |
| 9       | Edwards    | NULL           |
| 10      | Stephenson | NULL           |
+---------+------------+----------------+
19 rows in set (0.021 sec)
```

- Right Join

The `RIGHT JOIN` starts selecting data from the right table instead of the left table. 

```SQL
> SELECT last_name, title FROM Users RIGHT JOIN Books ON Users.id=Books.user_id;
+---------+-----------+----------------+
| id      | last_name | title          |
+---------+-----------+----------------+
| 1       | King      | The Green Mile |
| 2       | King      | The Stand      |
| 3       | King      | Misery         |
| 4       | King      | 11/22/63       |
| 5       | King      | The Institute  |
| 6       | Pratchett | Sorcerer       |
| 7       | Pratchett | Wintersmith    |
| 8       | Pratchett | Mort           |
| 9       | Pratchett | Thud           |
| 10      | Taylor    | Time Police    |
| 11      | Tregellis | The Mechanical |
| 12      | Tregellis | The Liberation |
| 13      | Tregellis | The Rising     |
| 14      | Chiang    | Exhalation     |
+---------+-----------+----------------+
14 rows in set (0.019 sec)
```

#### Index

- `Show Indexes`

This command shows all the indexes defined for a given database.

```sql
> SHOW INDEXES;
+-----------------+-----------------+
| table           | field(s)        |
+-----------------+-----------------+
| Users           | id              |
| Books           | id              |
+-----------------+-----------------+
2 rows in set (0.006 sec)
```

- `SHOW INDEX {field1,...} FROM {tablename}`

This command shows all the key/value pairs found in an index (shown below).

```sql
> SHOW index id FROM Users;
+-----------------+-----------------+
| key             | block#          |
+-----------------+-----------------+
| 1               | 5               |
| 2               | 6               |
| 3               | 7               |
| 4               | 8               |
| 5               | 9               |
| 6               | 10              |
| 7               | 11              |
| 8               | 12              |
| 9               | 13              |
| 10              | 14              |
+-----------------+-----------------+
10 rows in set (0.016 sec)
```

#### Alter

 This command changes an entity, and updates the associated row column(s) in the table. 

`ALTER TABLE {table-name} ADD{field-name} {field-info};` : Add a new column.

`ALTER TABLE {table-name} DROP{field-name};` : Drop an existing column.

```sql
> ALTER TABLE Users ADD middle_name varchar(25);
Query OK, 10 row affected (0.001 sec)

> DESCRIBE Users;
+----------------+--------------+------+-----+---------+-----------------------------+
| Field          | Type         | Null | Key | Default | Extra                       |
+----------------+--------------+------+-----+---------+-----------------------------+
| id             | int          | NO   | YES | NULL    |                             |
| first_name     | varchar(50)  | NO   |     | NULL    |                             |
| last_name      | varchar(50)  | YES  |     | NULL    |                             |
| age            | int          | YES  |     | NULL    |                             |
| zipcode        | int          | YES  |     | NULL    |                             |
| middle_name    | varchar(25)  | YES  |     | NULL    |                             |
+----------------+--------------+------+-----+---------+-----------------------------+
6 rows in set (0.015 sec)

> ALTER TABLE Users DROP middle_name;
Query OK, 10 row affected (0.001 sec)

> DESCRIBE Users;
+----------------+--------------+------+-----+---------+-----------------------------+
| Field          | Type         | Null | Key | Default | Extra                       |
+----------------+--------------+------+-----+---------+-----------------------------+
| id             | int          | NO   | YES | NULL    |                             |
| first_name     | varchar(50)  | NO   |     | NULL    |                             |
| last_name      | varchar(50)  | YES  |     | NULL    |                             |
| age            | int          | YES  |     | NULL    |                             |
| zipcode        | int          | YES  |     | NULL    |                             |
+----------------+--------------+------+-----+---------+-----------------------------+
5 rows in set (0.013 sec)
```



