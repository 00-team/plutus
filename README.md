# Plutus

a database

## Roadmap

1. define the fix table schema\
   user id | phone | nickname | picture | token

1. write a basic database in **python**, **C** and **sqlite**
1. test the performance of all databases

## Notos

1. database should be able to get/delete/insert/update and count the data

1. files should olny be stored as names and one byte for there extention

1. test must be a realistic scenario (random read/write in this case)

## Schema

```py
user_id: 8 bytes - unsigned int
phone: 2 bytes cc, 7 bytes phone - unsigned int
nickname: 52 bytes encoded - char
picture: 12 bytes - hex
token: hashed 64 bytes
```
