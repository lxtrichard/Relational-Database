# SP22-Assignment5
Selecting database rows

## Overview -- Let's Select Records!

In this assignment, we are going to implement code to handle "select" commands like, `SELECT * from Users`.  When run, this commmand will allow us to retrieve records from our database associated with a given table. This will build on the prior work we've been doing in previous assignments in the `SQLProcessor`, and `Entity` classes. 

<hr>

## Key Classes in This Assignment 

You'll discover that the following classes are significant in this assignment:

### The `Row` class

As with the case of inserting records, we'll need a working version of our `Row` class to be able select records.

### The `Rows` (or `RowCollection`) class

The `Rows` class will contain 0 or more `Row` objects. This is what actully gets returned by your `Database` object when it performs a `select` command.  

### The `SelectStatement` class

The `SelectStatement` class will perform the common task of helping to parse `SELECT...` commands. You'll implement this class in a manner similar to the other statement classes you have built for previous assignments.   

### The `Storage` class

Since we don't yet have an index, we'll use the `Storage::each()` (visitor) function to iterate blocks in a DB file, and find `Block`s that are associated with a given table/entity. You've probably already used this method for the `DUMP` and `SHOW TABLES` commands. 

### The `DBQuery` class

The `Query`class is used to describe (and help exectute) `SELECT`, `UPDATE` and `DELETE` statements. 

### The `TabularView` class

A `TabularView` will be the class that shows a view  of a `RowCollection`  when a user issues `SELECT * ...` command. 

<hr>

## Integrating files from assignment #4 with this assignment

To do this assignment, you will copy all your files from assignment#4 project folder into this folder. As before, do NOT overwrite the `main.cpp` file or the `TestAutomatica.hpp` file.

<hr>


## Challenge #1 : Implement the `SelectStatement` class

The `SELECT` command allows a user to retrieve (one or more) records from a given table. The command accepts one or more fields to be retrieved (or the wildcard `*`), along with a series of **optional** arguments (e.g. `ORDER BY`, `LIMIT`).  Below, are examples of the basic `SELECT` statement (presumes the existence of a `Users` and `Accounts` table): 
    
```
SELECT * from Accounts;
SELECT first_name, last_name from Users order by last_name;
```

When a user issues a `SELECT...` command, you'll create as `SelectStatement` class and have it parse the underlying tokens to validate the command. Your `SelectStatement` class should store all of the associated state for the `SELECT` command in a `Query` object.  Assuming the parsing process is successful, you'll eventually call a `selectRows` method -- and pass the `Query` object as an argument. 

For this challenge, your code must handle the following tasks:

1. Given a `SELECT` command - one of your processors should create an `SelectStatement`  object
2. The `SelectStatement` should help parse and validate the input (example given above); validation includes insuring that the table specified in the command is a known table in the active `Database`.  The `SelectStatement` should construct a reusable `Query` object that contains all the state information specified in the select command. We will later use the `Query` object when we implement `UPDATE` and `DELETE` commands.

## Challenge #2 : Implement `SQLProcessor::showQuery`

For this challenge, you'll implement the `SQLQuery::showQuery(const Query &aQuery, const RowCollection &aCollection)` method. We presume that your `SelectStatement` has successfully parsed the `SELECT...` command (and constructed a `Query` object). When the `SelectStatement` is run, it should dispatch to the `SQLProcessor::showQuery` method, and pass along the `Query` object.

1. Build a `RowCollection` by calling the `Database::selectRows` method.
2. The `Database::selectRows` method will rely on the `Storage` class to retrieve rows for that table.  Since we don't have an index, we'll have to iterate all the `Blocks` in a DB file searching for "data blocks" that are associated with the given table. Use `Storage::each` for that purpose.
3. Pass the `RowCollection` to a `TabularView` to present the collection to your user.
5. If an **optional** "order by" clause was provided, sort the data before presenting to the user.

## Challenge #3  : Implement `Database::selectRows` 

For this challenge, you'll implement the `Database::selectRows(const Query &aQuery, RowCollection &aRows)` method. We don't yet have a row index, so the task of selecting rows associated with a table will require that we iterate storage blocks. Fortunately, our `Storage` class provides the `Storage::each()` (visitor) method for cases of this nature. 

For every data `block` you find in storage, verify that the block is associated with the table/entity specified in the `Query`. Decode each of these `Blocks` into a `Row` object, and store each of them in the `Rows` argument (type: `RowCollection`) passed to this method.

## Challenge #4 : Implement the `TabularView` class

In this challenge, you'll construct a `TabularView` to "show" the selected `Rows` to the user. Your `TabularView` should use the `RowCollection` you assembled in the `Database::selectRows()` method as the list of `Row` objects to show to the user. The `Query` object that was created by your `SelectStatement::parse` method can be used to determine which columns to present to the user in your `TabularView`. 

> **NOTE:** If you user specified an `ORDER BY` clause in the query, make sure to sort your `Rows` accordingly before presenting them to the user.

<hr>

## Testing This Assignment

For our autograder, we're going to validate your `select` commands are working by retrieving records from a previously created table. Let's assume we already created a Users table and added 3 records. Next we'll issue the `SELECT` command:

```
SELECT * from Users;
```

The output of your describe statement should show something like this:

```
> SELECT * from Users;
+--------------------+--------------+
| id  | first_name   | last_name    |
+-----+--------------+--------------+
| 1   | david        | he           |
| 3   | rick         | gessner      |
+-----+--------------+--------------+
3 rows in set (0.00231 sec)
```

As always you can use the auto-grader to help get your code to perform as expected.

All of the tests for this assignment will be based on properly gathering the correct records (and number of records) for the given query. The queries won't be any more difficult than the ones shown above.

#### Grading
```
- Commpile Test 4pts
- App Test 4pts
- DB Test 4pts
- Tables Test 4pts
- Insert Test 4pts
- Select Test 80pts
```

## Submitting Your Work
#### Due Wednesday, May 7, 2022 at 11:30pm

Make sure your code compiles, and meets the requirements given above. Also make sure you updated your students.json file with the name of each contributor. Also make sure that your "version" statement outputs "version 0.5". 


