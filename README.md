# Plutus

a custom database for \_\_\_\_

## Roadmap

* [x] define the fix table schema\
   user id | phone | nickname | picture | token

## Notos

1. files should only be stored as names and one byte for there extention types

1. test must be a realistic scenario (random read/write in this case)

1. flag the users for deleation and keep a list of them in to registery
   so the new user will be replacing the old ones

## Todos

* [x] user_id + some random data = user_pic
* [x] find and implement the best data structure for phone number indexing: Tries
* [x] test all native types to ensure that all the types sizes are as expected\
      for example in my machine type `long` is `8` byte but the standard says that its `4` byte
* [x] a visual representation for the phone index
* [ ] check if the user picture is all null or not
      if it is get random bytes again
* [ ] handling the kill signals
* [x] go to the end of a file based on each item size not file size
      e.g. dont go to the SEEK_END instead calc the position of new item and go to there
* [ ] check invalid data
