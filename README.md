#cs576proj -- KeyLocker
Our password manager for CS576.

##Usage
`./client` This starts an interactive prompt.

`./server <port #> <certfile> <privkey> <userdb>` This starts the server.

**Commands:**

`add [<length>]`: 	Adds a new entry to the database with a random password of specified `length` (or prompts user for password if `length` was 0 or not included).

`gen <length>`:   Generates a random password of length `length`.

`get [<service> <username>]`:    Retrieves the entry for key: `<service>_<username>` from the database if they were provided, else returns a list of all entries. Reports error message if no such key exists.

`list [<service> <username>]`:   List is an alias for `get`.

`print [<service> <username>]`:   Print is also an alias for `get`.

`search <pattern>`:    Retrieves all entries that contain the string `pattern` in either their "service", "username" or "notes" fields.

`clip <service> <username>`:	Copies the password for key: `<service>_<username>` to the clipboard if the entry exists, then overwrites the clipboard. Requires X window manager / xclip.

`edit <service> <username>`:	 Edits an existing entry for key: `<service>_<username>` with new values provided by user. Reports error message if no such key exists.

`delete <service> <username>`: Deletes an existing entry for key: `<service>_<username>`. Reports error message if no such key exists.

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

##Contributing
The tasks for each assignment deadline have been setup as issues.
These can be discussed remotely by commenting on them.
If someone wants to work on a task they should assign themselves to the issue and create a branch for it.
(Branches can be created by doing `git checkout -b <branch_name>` then the branch can be pushed to Github by doing `git push -u origin <branch_name>` (this only needs to be done the first time, you can push normally once Github has the branch).
When you think your feature is done you should create a pull request for it the "base" of the pull request should be the `master` branch and the "compare"ed branch should be your `<branch_name>`.
The pull request can then be reviewed by the rest of the team (if necessary) and eventually merged into the master branch.
If your pull request notifies you that a merge conflict exists you can ping me by commenting on the pull request with `@bradford-smith94` and I will take care of resolving the conflict.
After your branch has been merged you need to checkout the master branch again (`git checkout master`) and make sure you are up to date by fetching/pulling as necessary.

If we all try to keep work to separate branches we shouldn't have any problems even when working remotely.
