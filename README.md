# cs576proj
Our password manager for CS576.

#usage
./client add ...

## Database file structure
The encrypted database is a JSON file, stored as `$HOME/.keylocker/$USER_keylocker.db`.

```
{
    dbuser = "username",
    dbpubkey = <public key>,
    dbentries : {
        service_username : [ service, username, password, note ]
        ...
    }
}
```
