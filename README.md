#cs576proj -- KeyLocker
Our password manager for CS576.

##Usage
`./client` This starts an interactive prompt.

`./server <port #> <certfile> <privkey> [userdb]` This starts the server. If no user database (`userdb`) is specified, then the administrative user is prompted to create one.

**Commands:**

`add [<length>]`: 	Adds a new entry to the database with a random password of specified `length` (or prompts user for password if `length` was 0 or not included).

`gen <length>`:   Generates a random password of length `length`.

`get [<service> <username> | <numRef>]`:    Retrieves the entry for key: `<service>_<username>` or the entry for `<numRef>` from the database if they were provided, else returns a list of all entries. Reports error message if no such key exists.

`list [<service> <username> | <numRef>]`:   List is an alias for `get`.

`print [<service> <username> | <numRef>]`:   Print is also an alias for `get`.

`search <pattern>`:    Retrieves all entries that contain the string `pattern` in either their "service", "username" or "notes" fields.

`clip (<service> <username> | <numRef>)`:	Copies the password for key: `<service>_<username>` or the entry for `<numRef>` to the clipboard if the entry exists, then overwrites the clipboard. Requires X window manager / xclip.

`edit (<service> <username> | <numRef>)`:	 Edits an existing entry for key: `<service>_<username>` or the entry for `<numRef>` with new values provided by user. Reports error message if no such key exists.

`delete (<service> <username> | <numRef>)`: Deletes an existing entry for key: `<service>_<username>` or the entry for `<numRef>`. Reports error message if no such key exists.

`save`:         Save a local copy of the database without uploading to server.

`register`:     Attempt to register with previously specified server.

`chpass`:       Attempt to change database password and update record on server.

`upload`:       Save database to disk and upload to server.

`download`:     Download the database file stored on the server.

`quit`: Exits the program.

`help`: Displays this list of commands.

##Database file structure
The encrypted database is a JSON file, stored as `$HOME/.keylocker/$USER_keylocker.db`.

```
{
   "dbuser"  : "username",
   "srvhost" : "server_hostname",
   "srvport" : "server_port",
   "srvuname": "server_username",
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

##Documentation
The `doc/` directory contains LaTeX documents for the Proposal and Design documents. The Makefile in `src/` has a target for compiling the documentation.

