# SP22-Assignment4
#### Due Wednesday - April 27, 2022 - 11:30pm 

## Overview -- Let's Insert Records!

In this assignment, we are going to implement the `Insert into` command that will allow us to add records to our database. As usual, this will build on the work we've been doing in prior assignments in the `SQLProcessor`, `Database` and `Entity` classes.

Let's get started!

> **NOTE:** Be careful not to overwrite the files we provided with prior versions.

<hr>

## Key Classes in This Assignment 

You'll discover that the following classes are significant in this assignment:

### The `Value` class

We need a class that can represent each "field" of user data in a record/row. In lecture we've been discussing the use of the `std::variant` class for this purpose.  Our value class can simply be the `std::variant`, or you can create your own custom class, and add some extra utility (helper) functions. 

### The `Row` class

A `Row` is a collection of user data. Imagine that the user creates a `Users` table and inserts a few records. If they subsequently issue a `SELECT * from Users where  id=1` statement -- the database will return a collection of `Row` objects for this purpose. The row will contain a collection of key/value pairs (fieldnames + values). 

### The `Entity` class

When a user inserts a records into a table, numerous validations must occur. We must ensure that the specified table exists, and that all the fields specified in the `insert` statement are declared in the `Entity`.  The `Entity` class is essential for both of these tasks.

We recommend that you take advantage of the `BlockIO::writeBlock()` method for storing your new data rows. Data rows are meant to be stored as a single block. 

<hr>

## Challenges for Assignment #4

### Challenge #1: Insert Into... 

The `insert` command allows a user to insert (one or more) records into a given table. The command accepts a list of field, and a collection of value lists -- one for each record you want to insert. Below, is an examplw where we insert two records -- for our very own TA's!  `<applause>` 
    

```
INSERT INTO users 
  ('first_name', 'last_name', 'email') 
VALUES 
  ('David','He', 'dyhe@ucsd.edu'),
  ('Sai', 'Komatineni', 'saikomatineni@engucsd.edu');
```

For this assignment, your code must handle the following tasks:

1. given an "insert..." command - one of your processors should create an `InsertStatement`  statement 
2. The `InsertStatement` should parse and validate the input (example given above); validation includes insuring that the table specified in the command is a known table in the active `Database`
3. Assuming a valid statement and a known `Entity`, your processor(s) should route this command a `controller` object (e.g. `Database`) to be executed
4. A new `Row` should be constructed - and assigned a new primary key (type int). 
5. Think carefully about how your system will manage primary keys for given table. Each time you insert a new `Row` it will need an integer value for its primary key field.  Some object in your system needs to be responsible for generating primary key values.  Keys are never reused.
6. The new `Row` should be encoded/saved into to storage, and "associated" with the given table. This means that a new data block is added to the storage file, and that it contains attributes that associated the data row with the given table. Considering storing this information in the header portion of the `Block` used to store the data. 

This seems like a lot, right?  We'll discuss how to break this down into something you can handle in lecture this week. 

### Challenge #2: Preparing for `select * from table` (for next week) [OPTIONAL]

Next week (assignment #5), your team will implement a basic vesrion of the `select` command.  Ordinarily, a database would attempt to load rows associated with a table by iterating an index. Since we don't yet have a general purpose indexing mechanism for our database, we need an alternative.

As we discussed in lecture, your `Storage` class offers a `Storage::each(visitor)` method. Many of you used that method to implement the `dump database foo` command last week. As we've shown in the class a "visitor" pattern as implemented in `Storage::each` can be combined with either functors or closures to make easy work of iterating/searching all the blocks in a database storage file. 

Let's presume we've created a "Users" table and inserted some records (Challenge #1). Consider what happens when someone issues the command, `SELECT * from Users`. You'll need to iterate the blocks in your DB storage file, and find those that are associated with the given table (users). 

#### Question 1
How will you find blocks that are associated with the given table?  Can the existing `Storage::each()` function be helpful here?

#### Question 2 
How do you convert a `Block` from a DB storage file back into a `Row` object (and vice versa)?

#### Question 3
How will you build a collection of associated `Row` objects specified in the given `select * from Users` query?

Considering these questions can help you correclty implement the `Insert INTO...` statement, and prepare to implement a basic `SELECT` in assignment #5.

<hr>

## Testing This Assignment

> **NOTE**: Make sure you update the "version" value for your `Version` command. The new version number should be 0.4. 

For our autograder, we're going to validate your `insert` command is working by monitoring the state of your storage file (.db).  A sample test might look like this:

```
create database foo;
use database foo;
create table users ... (etc.);
insert into users (field1, field2...) values (record-1-values), (record-2-values);
dump database foo;
```

The output of your dump statement should show something like this (we don't validate the columns you choose to output):

```
> dump database foo;
+------+-----------+-----+
| #    | Type      | Id  |
+------+-----------+-----+
| 0    | meta      |     |
| 1    | entity    |     |
| 2    | data      | 1   |
| 3    | data      | 2   |
+------+-----------+-----+
4 rows in set (0.00024 secs)
```

As always you can use the auto-grader to help get your code to perform as expected.

## Grading
- You have implemented the Insert Into statement
- You can add rows to the storage 
- Your code passes the auto-grader tests.

```
Compile-test : 5pts
App-test: 5pts
DBCommands: 20pts
Table-test: 20pts
Insert-test: 50pts
```

## Submitting Your Work

### Due Wednesday - April 27, 2022 - 11:30pm
You and your partner should make sure you turn your solution in on time! Also -- make sure you have updated the students.json file to contain the name of each contributor to your project.

Good luck everyone!



