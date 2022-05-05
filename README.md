# SP22-Assignment6
Let's apply filters to our search queries.

## Overview -- Applying Search Filters!

In this assignment, we are going to improve the `select` command to allow us to retrieve records from our database that meet specified search criteria. We'll also ORDER our data, and limit the number of rows we return.

<hr>

## Key Classes in This Assignment 

You'll discover that the following classes are significant in this assignment:

### The `Filter` class

This class helps us perform conditional logic on data when we perform a select. The `Filter` class can represent complex conditional logic, and validate whether a `Row` meets the conditions or not.

<hr>

## Integrating files from assignment #5 with this assignment

To do this assignment, you will copy MOST your files from assignment#5 project folder into this folder. Please don't overwrite the following files:
```
main.cpp
testAutomatic.hpp
keywords.hpp
Helpers.hpp
compare.hpp
Filters.*
```

If you've made your own changes to `Keywords` or `Helpers`, you may have to do a manual merge of the differences.

<hr>

## Implementing The `Filter` Class

The `Filter` class provides support for conditional logic. We'll use it to perform the conditional checks in the `SELECT..WHERE` clause. The filter class can contain one or more `Expression` objects. An `Expression` contains two `Operands` and one logical `Operator` (e.g. first_name="Megan"). The purpose of this class is to determine whether a row meets the criteria given by the user. Consider the query:

```
SELECT * from Users where age>20 AND zipcode=92100 ORDER BY last_name;
```

The filter class in this case would contain two expressions (age>50) (zipcode=92100) and one logical operator (AND). Records from the `Users` table are loaded, and tested by the given Filter. If a row matches the expression criteria specified in the `Filter`, it will be added to the `RowCollection`.  

We've provided a starter version of the `Filter.hpp` file. You're free to use (or discard) this class. However you will need to implement a `Filter` class so that your code can properly filter records based on values set in the `SELECT` (or other) statements.

> **NOTE**: The `Filter` class can handle basic expressions, but it doesn't (yet) know how to deal logically combining them (AND, OR, NOT).  You and your teammate need to add logic to the `Filter` class to support logically combining expressions. 

### Implementing the Comparison Logic

Also included in this assignment is a file called `Compare.hpp`.  This file provides template functions that implements the "Equals" comparision operator. While we have provided you starting code for the `Equal` condition, you must implement your own variations for the other logical operations (LessThan, GreaterThan, NotEqual, LessThanEqual, GreaterThanEqual,...). 

<hr>

## Implementing Commands for Assignment #6

### Challenge 1: Select...WHERE ... 

This version of the `SELECT` command builds upon the work we've already done. In this case, however, we expect the the records to be filtered according to the given WHERE clause, using a `Filter` object.

```
SELECT * from Users where first_name="Anirudh";
```

For this task, your code must handle the following tasks:

1. Retrieve `Rows` as a `RowCollection` as you did for the basic `SELECT` case (above)
2. Filter the collection of rows to exlude those that don't pass the conditional logic (using a `Filter`)
3. Present the user data using your `TableView` class in the specified order


> NOTE: You may be asked to handle these clauses in any combination or order. For example, you may be asked to retrieve N records and order them: `SELECT first_name, last_name from Users order by last_name LIMIT 3`

### Challenge 2: Select...ORDER BY `fieldname`... 

In this challenge, you'll handle ordering the resulting collection of Rows according to the "ORDER BY" field provided. Your database does not store data in an ordered format -- so we must do this step manually before we present the results to the user. 

```
SELECT last_name, email from Users ORDER BY age;
```

You'll only be asked to order data by a single field, and only in ascending order (smallest first).

### Challenge 3: Select...LIMIT _N_... 

In this version of the `SELECT` command we are also expected to limit the number of records retrieved. Assuming a table has 100 records, the `LIMIT` clause applies a limit of the total records retrieved, so we might only ask to retrieve N records.

```
SELECT last_name, email from Users LIMIT 5;
```

For this task, your code must handle the following tasks:

1. Retrieve `Rows` as a `RowCollection` as you did for the basic `SELECT` case (above)
2. Stop adding rows once you have reached the given LIMIT value
3. Present the user data using your `TableView` 

<hr>

## Testing This Assignment

For our autograder, we're going to validate your `select` commands are working by retrieving records from a previously created table. Let's assume we create a Users table and add some records. Then we'll issue a command like:

```
> SELECT id, first_name, last_name, zipcode from Users where zipcode>92120 order by zipcode LIMIT 2
+--------------------+--------------+--------------+
| id  | first_name   | last_name    | Zipcode      |
+-----+--------------+--------------+--------------+
| 3   | Anirudh      | Swaninthan   | 92126        |
| 1   | Pu           | Cheng        | 92127        |
+-----+--------------+--------------+--------------+
2 rows in set (0.00123 sec)
```


As always you can use the auto-grader to help get your code to perform as expected.

All of the tests for this assignment will be based on properly gathering the correct records (and number of records) for the given query. The queries won't be any more difficult than the ones shown above.

#### Grading
```
- Commpile Test 10pts
- Tables Test 10pts
- Insert Test 25pts
- Select Test 55pts
```

## Submitting Your Work
#### Due Tuesday, May 10, 2022 at 11:30pm

Make sure your code compiles, and meets the requirements given above. Also make sure you updated your students.json file with the name of each contributor. Also make sure that your "version" statement outputs "version 0.6".  Check your work into git as usual. (If you aren't using the command line version of git by now...it's time you start).

Good luck! 
