# SP22-Assignment10
### Due Sunday - June 5, 2022 - 11:30pm 

## Overview -- Adding a Cache for Performance

In this assignment, we are going to add a cache to improve overall system performance. 

<hr>

## Key Classes in This Assignment : `LRUCache`

The main puprose of the `LRUCache` class is to hold a set of data in memory (avoiding disk operations) so that you can improve overall system performance. You'll want to adapt your code slightly to support the cache (for reading and writing). 

We've provided a minimal version of the `LRUCache` class. It's up to you to adapt and implement this code to meet your needs. You can change it any way you like, or replace all of it with your own.

```
  template<typename KeyT, typename ValueT>
  class LRUCache {
  public:
        
    //OCF 

    void    put(const KeyT &key, const ValueT& value);
    ValueT& get(const KeyT& key);    
    bool    contains(const KeyT& key) const;
    size_t  size() const; //current size
    
  protected:
    size_t maxsize; //prevent cache from growing past this size...

    //data members here...
  };
```

You should design your `LRUCache` so that can cache `Block` or `Row` objects. For `Block` objects, the cache "key" will likely be the block number, and the value an actual `Block`. For a `Row` cache, you will likely use a primary key (int) for the "key", and a `Row` or `Row*` for the value.

> **NOTE:** The `Config` class has changed slightly in this version, so that we can dynamically enable/disable caching. In order to use it, you must make a one-line change to your `Application.cpp` file. Put this line at the top of the file (inside the namespace specifier).

```
  bool Config::cacheSize[]={0,0,0};
```

> **NOTE:** The size of your `LRUCache` memory cache should be obtained at run-time by calling `Config::getCacheSize(CacheType)`. This approach allows our testing system to test performance of your system using different size caches.  Caching should only be enabled for a given `CacheType` (block, row, view) if `Config::getCacheSize(CacheType)` returns a non-zero value. 

<hr>

## Integrating files from assignment #9 with this assignment

To do this assignment, copy your files from PA9 as usual.  Don't overwrite the new versions of:

```
Config.hpp
main.cpp
TestAutomatic.hpp
makefile
```

<hr>

## Adding Support For the `LRUCache` Class

We discussed how to build an `LRUCache` in lecture two weeks ago. Please refer to that lecture for more details. Google offers many examples -- but be careful -- because some are very poor.

After you have designed and built your `LRUCache` class, you'll integrate into the flow of your logic. However, recall that we only _use_ the cache if the `Config::getCacheSize(CacheType)` returns a non-zero value.  `Config::CacheType` argument may be 'block', 'rows', or 'views'. 

### Challenge 1 

In this challenge you'll implement a block cache. We discussed several ways you can implement this in your system. The only hard requirement is that your code must be able to run with the cache enabled or disabled. The testing system will vary this `Config` setting. 

When enabled, you want to load blocks into your cache, so that they may be retrieve more quickly on a subsequent request. Your caching policy algorithm should be designed to keep "most-recently-used" blocks, and discard the "least recently used". If block requests exceed the capacity of your cache, you may have to discard recently used blocks. 

### Challenge 2 -- Optional

In this challenge, you'll implement a cache for `Row` objects. The template we provided you as a starter for your `LRUCache` can be used for either blocks or rows -- given the correct set of template arguments.  So this challenge involves integrating the cache with your logic for reading/writing `Row` objects. 

### Challenge 3 -- Optional

In this challenge -- you'll add a "view" cache to your system. Since views are text based, you can cache your views on disk, and reuse them (by loading the view output text from disk) if the same query is sent to your database.  You can also use the `LRUCache` to cache views in memory if you prefer.

## Testing This Assignment

As always you can use the auto-grader to help get your code to perform as expected.

```
- Joins Test 10pts
- Index Test 30pts
- Block Cache Test 55pts
- Row Cache Test 15pts
- View Cache Test 15pts
```

## Turning in your work 

Make sure your code compiles, emits ZERO warnings, and meets the requirements given above. 

Submit your work by checking it into git by <b>June 5, 2022 - 11:30pm  </b>. Good luck! 

