#cs576proj -- KeyLocker
Our password manager for CS576.

#Usage
`./client <command>`

**Commands:**

`add`:	Adds a new entry to the database.

`get [<service> <username>]`:    Retrieves the entry for key: `<service>_<username>` from the database if they were provided, else returns a list of all entries.


## Database file structure
The encrypted database is a JSON file, stored as `$HOME/.keylocker/$USER_keylocker.db`.

```
{
   "dbuser" : "username"
   "dbentry" : {
      "service_username" : {
         "notes" : "notes go here",
         "password" : "password goes here",
         "service" : "service goes here",
         "username" : "username for service goes here"
      },
			...
   },
}
```
