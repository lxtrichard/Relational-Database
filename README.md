# SP22-Assignment9
Adding index support to improve performance

### Due Tuesday - May 31, 2022 - 11:30pm 

Building an index to improve performance

## Overview -- Working with an Index

In this assignment, we are going to add a primary-key index to tables that we create.  We will update the index as we add/update/delete rows in the table. And we will make our index persistent, so it's available whenever we open a `Database` to work with data rows.

<hr>

## Key Classes in This Assignment 

You'll discover that the following classes are significant in this assignment:

### The `Index` class

We've provide a basic version for your `Index` class. It's up to you to adapt this code to meet your needs. You can change it any way you like, or replace all of it with your own.

The main job of the `Index` class is to hold a set of key/value pairs that map a field (in every row of a table) with the blocknumber where that row was located in your `Storage` DB file. Therefore, inserting, deleting, and finding block number (values) based on a field (key) are essential methods.  We provided a basic set of methods for this purpose. 

```
- addkey/value
- find key/value
- remove key/value
- getvalue(key)
- bool contains(key)
```

In the version we provide, the `Index` class is `Storable` so it can be saved to storage with a stream interface. You don't have to use that approach, for example, if you prefer to insert your index data into storage using some other method.

We provided an iteration method, `Index::each(BlockVisitor)`, to visit blocks assocated with data rows in the index. This should make it easier to integrate the use of the index into your logic where you process a "select" query. 

When you call the `Index.each()` method, it will call your visitor object one time for each value in the index. This will allow you to read/search/use associated rows without having to scan every block in a `Storage` DB file.  We've also provided an `eachKV()` method, which lets you iterate all the key/value pairs (key,blocknum) in a given index. This method makes it easy to implement a "show index..." statement later.

<hr>

## Integrating files from assignment #8 with this assignment

To do this assignment, copy your files from PA8 as usual.  Don't overwrite the new versions of the `main.cpp`, `TestAutomatic.hpp`, or `makefile` files.

<hr>

## Adding Support For the `Index` Class

Your system will interact with `Index` classes in the following circumstances:

### `CREATE` Table

When you `CREATE` a table, you will automatically create an index associated for the primary key of the table. The value of every index is the block number of the row associated with the primary key.

### `DROP` Table

When you `DROP` a table, you will automatically delete all the associated indexes, as well as the associated data rows and Entity.

### `INSERT` Rows

When you `INSERT` rows, you will add an entry (primary key) from the row into the associated table index.

### `SELECT` Rows

When you 'SELECT" rows, you try to locate the associated rows by searching your indexes. This is naturally dependent on the nature of the query, and whether an appropriate index is available.  Remember -- in the worst case (where no index exists matches the conditions specified in your `WHERE` clause) -- you can always use the primary key index to load records for your table, rather than scanning all block in a storage file.

### `DELETE` Rows

Before you delete the associated rows, make sure to remove the prmary key (field) for each row from the index.

## Implement the `SHOW INDEXES` Command

The next step in this assignment is to add support for the `SHOW INDEXES` command, which shows all the indexes defined for a given database:

```
> show indexes
+-----------------+-----------------+
| table           | field(s)        | 
+-----------------+-----------------+
| users           | id              |  
+-----------------+-----------------+
1 rows in set (nnnn secs)
```

## Implement the `SHOW INDEX {field1,...} FROM {tablename}` Command

The last step in this assignment is to add support for the `SHOW INDEX` command, that shows all the key/value pairs found in an index (shown below). For now, you just have to be able to retrieve the values for the primary key (id). 

```
> SHOW index id FROM Users; 
+-----------------+-----------------+
| key             | block#          | 
+-----------------+-----------------+
| 1               | 35              |  
+-----------------+-----------------+
| 2               | 36              |  
+-----------------+-----------------+
| 3               | 47              |  
+-----------------+-----------------+
3 rows in set (nnnn secs)
```

> NOTE:  In the example above, we cite the `id` field, which is found in our index for the primary key. The field(s) we name in the field list {field1,...} must match the fields stored in the key of the index. 

## Testing This Assignment

As always you can use the auto-grader to help get your code to perform as expected.

```
- Insert Test 8pts
- Delete Test 8pts
- Select Test 8pts
- Drop table 8pts
- Joins Test 8pts
- Index Test 60pts
```

## Turning in your work 

Make sure your code compiles, and meets the requirements given above. 

Submit your work by checking it into git by <b>Tuesday - May 31, 2022 - 11:30pm  </b>. Good luck! 
