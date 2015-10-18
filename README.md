# cs576proj
Our password manager for CS576.

#usage
./client [command]

Commands:
add:	Adds a new entry to the database.

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
