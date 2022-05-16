# SP22-Assignment7
## Overview -- Let's Update and Delete Records!
### Due Tuesday - May 17, 2022 - 11:30pm 

In this assignment, we are going to implement the `update` and `delete` commands that will allow us to chanage records in our database. This will build on the prior work we've been doing in previous assignments.in the `SQLProcessor`, `Entity`, and `Database` classes.

<hr>

## Integrating files from assignment #6 with this assignment

To do this assignment, you will copy all your files from assignment-#6 project folder into this folder.  Do not overwrite the `main.cpp` or `TestAutomatic.hpp`, or the `makefile`. 

<hr>

## Key Classes in This Assignment 

You'll discover that the following classes are significant in this assignment:


### The `UpdateStatement` class

```
UPDATE Users set zipcode="12345" WHERE id=10;
```

Your team will create an `UpdateStatement` command, similar to the other commands you've created in your database. This statement will handle the `UPDATE` command. If you planned your code carefully, you can reuse parsing code you developed for the `SelectStatement` class to deal with the `WHERE` clause. 


### The `DeleteStatement` class

```
DELETE FROM Users WHERE zipcode>92000;
```

Your team will implement the `DeleteStatement` command. Once again, you might consider how this command can take advantage of code you wrote for the `SelectStatement`. 


<hr>

## Implementing Commands for Assignment #7

### Challenge #1:  The `UPDATE table` Command

The `UPDATE` command allows a user to select records from a given table, alter those records in memory, and save the records back out to the storage file.     

```
UPDATE Users SET "zipcode" = 92127 WHERE zipcode>92100;
```

For this challenge, your code must handle the following tasks:

1. The `UpdateStatement` should parse and validate the input (example given above); validation includes insuring that the table specified in the command is a known table in the active database. It must also gather conditional filters.
2. Assuming a valid statement and a known `Entity`, your processor(s) should route this command a `controller` object (e.g. `Database`) to be executed
3. A controller class (maybe `Database`) will implement the `updateRows` method, where it will work with the `Storage` class to retrieve records in the given table that match the (optional) set of given `Filters`. 
4. Iterate the selected records and change them according to given key/value pairs specified by user
5. Store the updated `Rows` back into the database into their original block
6. Provide output to the user indicating success or error of the command

```
UPDATE Users SET "zipcode" = 92127 WHERE zipcode>92100;
Query Ok. 2 rows affected (0.000087 sec)
```

> NOTE: If the `UPDATE` statement doesn't include filters (`WHERE...`), the given changes will apply to every row in the table.


### Challenge #2:  The `DELETE FROM table WHERE..` command

```
DELETE FROM Users WHERE zipcode>92000;
```

The `DELETE` command allows a user to select records from a given table, and remove those rows from `Storage`.   When a user issues the `DELETE FROM...`  command, your system will find rows that match the given constraints (in the `WHERE` clause). Next, you will remove the associated rows from storage by marking the related data blocks as `FREE`.    

For this challenge, your code must handle the following tasks:

1. The `DeleteStatement` should parse and validate the input (example given above); validation includes insuring that the table specified in the command is a known table in the active database. It must also gather conditional filters.
2. Assuming a valid statement and a known `Entity`, your processor(s) should route this command a `controller` object (e.g. `Database`) to be executed
3. A controller class (maybe `Database`) will implement the `deleteRows` method to achieve the `DELETE` operation.
5. Use the `Storage` class to mark the blocks associated with each selected row as `FREE`. 
6. Provide output to the user indicating success or error of the command

```
DELETE from Users where zipcode>92124;
Query Ok. 1 rows affected (0.000023 sec)
```

> NOTE: If the user omits the `WHERE` clause, then you should delete _all the rows_ associated with the given table. 

### Challenge #3:  Updating the `DROP` table command

Now  that we can delete rows from our database, we need to update our `DROP TABLE ...` command do delete any rows associated with the table we are dropping. Fortunately, we reuse our `DELETE FROM table` logic -- which will delete every row in the database associated with this table.  Assuming we have a "Users" table with 5 rows, your output will look list this (your time may vary):

```
drop table Users;
Query Ok. 5 rows affected (0.000023 sec)
```
> Note:  If you carefully plan out the logic for your `DELETE FROM table...` command, you can reuse this logic to automatically delete all the rows associated with a table that you are dropping. 

<hr>

## Testing This Assignment

As always you can use the auto-grader to help get your code to perform as expected.  All of the tests for this assignment will be based on properly gathering the correct records (and number of records) for the given query. The queries won't be any more difficult than the ones shown above.

> NOTE:  We have changed the testing system slightly. All the test names passed in command line arguments to `main.cpp` now start with an upper case letter (e.g. "Select" instead of "select"). This only matters if you're running these automated tests yourself.

#### Grading
```
- Insert Test 10pts
- Select Test 10pts
- Update Test 30pts
- Delete Test 30pts
- DropTtable Test 20pts
```

## Turning in your work 

Make sure your code compiles, and meets the requirements given above.

Submit your work by checking it into git by <b>Tuesday - May 17, 2022 - 11:30pm </b>. Good luck! 
