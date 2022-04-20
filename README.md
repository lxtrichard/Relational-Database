# SP22-Assignment3
Getting started with table commands

#### Due Monday, April 18, 2022, @11:30pm

It's time to start building tables!

## Overview 

In this assignment, we're going to start handling "table-related" commands.  In order to so so, we're going to introduce our 3rd and final CommandProcessor: `SQLProcessor`.  This class will provide support for four new commands:

1. create table `table-name`
2. drop table `table-name`
3. describe `table-name` 
4. show tables

## Integrating files from assignment #3 with this assignment

To do this assignment, you will copy _almost_ all of your files from your assignment#2 project folder into the folder for this assignment.  There are three files you will **NOT** copy over from Assignment #2:

1. main.cpp
2. TestAutomatic.hpp

These files all have slight changes in Assignment #3, and should not be overridden.

## New Classes in This Assignment 

### The `Entity` class 

Consider the situation where a user wants to create a database table, by issuing a "create table..." command shown below: 

```
CREATE TABLE tasks (
  id INT AUTO_INCREMENT PRIMARY KEY,
  title VARCHAR(100) NOT NULL,
  price FLOAT DEFAULT 0.0,
  due_date TIMESTAMP, //2020-04-15 10:11:12
  status BOOLEAN DEFAULT FALSE,
)
CREATE TABLE tasks ( id INT AUTO_INCREMENT PRIMARY KEY, title VARCHAR(100) NOT NULL, price FLOAT DEFAULT 0.0, due_date TIMESTAMP, status BOOLEAN DEFAULT FALSE,)
```

The `Entity` class is an in-memory representation of a table defintion. It's a container class, that contains one-or-more `Attributes`. 
In this example, we defined a table with 5 attributes (task_id, title, price, due_date, status).  We provide you a (nearly) empty version of this file -- and you are free to make any changes you want. 

### The `Attribute` class

An `Attribute` represents a single "field" in a database table defintion (`Entity`). In the table defintion shown above, `task_id` is an example `Attribute`.  The attribute class needs to be able to store the following information:

```
- field_name
- field_type  (bool, float, integer, timestamp, varchar)  //varchar has length
- field_length (only applies to text fields)
- auto_increment (determines if this (integer) field is autoincremented by DB
- primary_key  (bool indicates that field represents primary key)
- nullable (bool indicates the field can be null)
```

We provide you an empty version of this class file, and you are free to make any changes you want. 

### The `Row` class

You are given a new `Row` class. This will hold user data for a given row. The `Row` class will also participate in the storage process. Eventually each `Row` will be stored in a `Block` inside the storage system, and retrieved later to be shown in a `TableView`.  You won't need this until next week.

> Note: At the bottom of `Row.hpp`. we've pre-defined a class called `RowCollection` that you'll use in subsequent assignments. 

### The `SQLProcessor` class

Our third (and final) CommandProcessor (subclass).  This class will handle commands outlined in this assignment.  We provide you a basic (mostly empty) class file and you will implement the associated features. You are free to make any changes or add any methods you want to this file. 

In this assignment, this class will be used to implement four new commands. It will handle additional commands in future assignments. 

### The `TabularView` class

A `TabularView` class is used to present user data (rows) as a result of a user issuing a `select...` statement.  You can (but are not required) to use this class to implement the `show tables` command. This class was added here as a placeholder, and to get you started thinking about how you might implement tabular views.

As you're building this class, consider how the `Entity` class might be helpful. 

## Implement the Following System Commands 

### Challenge 1: "create table `table-name`" 

When a user issues this command, your `SQLProcessor` will make a corresponding statement object (subclass), that will try to parse the given input. If the input is valid, your code should construct an `Entity` object from that input. Then you'll ask the `Database` to save the given `Entity` in a block within the storage system.  This will result in the `Entity` object being encoded into a `StorageBlock` and saved in a DB file. 

```
> create table test1 (id int NOT NULL auto_increment primary key, first_name varchar(50) NOT NULL, last_name varchar(50));
Query OK, 1 row affected (0.002 sec)
```

One thing to consider -- you will frequently need to retrieve the `Entity` for a given table from storage in order to perform other actions (like inserting records). How will you quickly find and retrieve the `Entity` for a given table from storage when you need it later?  You could, of course, perform a linear scan of every `Block` in the db storage file and try to locate the `Entity` for a named table. But that would be terribly slow in a large database.  Work with your partner (or your TA) to consider your options on ways to solve this. In class, we discussed the possibility of using an `Index` class that maps key/value pairs for each entity (key=entity_name, value=block_number);

> Note: When you save an `Entity` in the storage file, the associated "block-type" will be an "EntityBlock". If you were to use the `dump database {name}` command from last week, an "EntityBlock", make sure your output can distinguish an "EntityBlock" from a "Meta" or "Data" block by properly setting the `Block.header.type` field.


### Challenge 2: "show tables" 

As we discussed in class -- when a user issues this command in a SQL database, the list of known tables (from the database in use) are shown to the user in the terminal.  You will implement that now.   Let's assume that your user created a few tables. Your output should be similar to what you see in MySQL:

```
> show tables;
+----------------------+
| Tables_in_mydb       |
+----------------------+
| groups               |
| users                |
+----------------------+
2 rows in set (0.000025 sec.)
```

1. Create a `ShowTables` statement class that can recognize and parse this type of command
2. Have the `SQLProcessor` handle this command, create the `ShowTables` statement object, and handle routing as usual 
3. If all goes well, ask a controller object (maybe the `Database`?) to exectute the command and get the list of known `Entities` 
4. Create a new `EntityView` class, and let that present a view that contains the list of tables in the DB 

### Challenge 3: "drop table `table-name`"

When your user issues this command, then your system should attempt to locate and delete the associated `Entity` (table) from the storage system. If the table doesn't exist, report an "unknown table" error to your user.  

```
> drop table groups;
Query OK, 0 rows affected (0.02 sec)
```

> **NOTE:** The number of rows affected depends on how many data rows are stored in the database for that table. 

As we discussed in lecture, "deleting" a `Block` means to set (and save) the `Block.header.type`='F' (for free). 

> NOTE: If a user deletes a table that has associated records (or indexes), these elements must also be deleted from Storage. This isn't a concern today, because we can't add records yet. But give this some consideration for a future assignment.

1. Create a `DropTableStatement` class (subclass of `Statement`) that can parse this syntax
2. Let the `SQLProcessor` process and route this command
3. Assuming this given `table-name` actually exists, ask the `Database` object to delete that `Entity` and associated data.
4. Report the results back to your user (see example above)

### Challenge 4: "describe `table-name`"

When your user issues this command, then your system should attempt to locate and describe the associated `Schema`. If the table doesn't exist, report an "unknown table" error to your user.

As we discussed in lecture, "describing" a `Schema` means to print out a textual description of the attributes of the given table. For example, if we were to issue the command "describe tasks" (using the table description above) we would see:

```
> DESCRIBE tasks;
+-----------+--------------+------+-----+---------+-----------------------------+
| Field     | Type         | Null | Key | Default | Extra                       |
+-----------+--------------+------+-----+---------+-----------------------------+
| id        | integer      | NO   | YES | NULL    | auto_increment primary key  |
| title     | varchar(100) | NO   |     | NULL    |                             |
| price     | float        | YES  |     | 0.0     |                             |
| due_date  | date         | YES  |     | NULL    |                             |
| status    | boolean      | YES  |     | FALSE   |                             |
+-----------+--------------+------+-----+---------+-----------------------------+
5 rows in set (0.000043 sec.)
```

1. Create a `DescribeTableStatement` that can parse this statement
2. Let the `SQLProcessor` process and route this command
3. Assuming this given `table-name` actually exists, ask the `Database` object to handle this command 
4. Create a `TableDescriptionView` to show the table description (shown above)


## Testing

As before, we recommend you perform testing on your solution by creating a test script. You can also use the auto-grader provided on github classroom for this assignment.

> **NOTE:**: Update the output value for your version command, so to that this application is version 1.3. 

## Grading

1. You have implemented the `SQLProcessor`, `Attribute`, `Entity`,  and `ShowsTablesView` classes
2. You have implemented the `CreateTable`, `DropTable`, `DescribeTable`, and `ShowTables` statement classes
3. You have implemented the code neccesary to get the four commands (listed above) to work correctly
4. Your code passes the auto-grader tests. 

```
Compile-test : 5pts
App-test: 5pts
Reader-test: 10pts
DB-test: 10pts
Table-test: 70pts
```

## Submitting Your Work
#### Due Monday, April 18, 2022, @11:30pm

You and your partner should make sure you turn your solution in on time!  Also -- make sure you have updated the `students.json` file to contain the name of each contributor to your project. Also -- update the assignment number in the students.json file to assignment #3.

Good luck everyone!



