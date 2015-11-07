MAKE   = make
CLIENT = ./client/
SERVER = ./server/

#call all from client/Makefile and server/Makefile
all:
	$(MAKE) -C $(CLIENT)
	$(MAKE) -C $(SERVER)

#call clean from client/Makefile and server/Makefile
clean:
	$(MAKE) clean -C $(CLIENT)
	$(MAKE) clean -C $(SERVER)

#call fclean from client/Makefile and server/Makefile
fclean:
	$(MAKE) fclean -C $(CLIENT)
	$(MAKE) fclean -C $(SERVER)

#call re from client/Makefile and server/Makefile
re:
	$(MAKE) re -C $(CLIENT)
	$(MAKE) re -C $(SERVER)

#call all from client/Makefile
client_all:
	$(MAKE) -C $(CLIENT)

#call clean from client/Makefile
client_clean:
	$(MAKE) clean -C $(CLIENT)

#call fclean from client/Makefile
client_fclean:
	$(MAKE) fclean -C $(CLIENT)

#call re from client/Makefile
client_re:
	$(MAKE) re -C $(CLIENT)

#call all from server/Makefile
server_all:
	$(MAKE) -C $(SERVER)

#call clean from server/Makefile
server_clean:
	$(MAKE) clean -C $(SERVER)

#call fclean from server/Makefile
server_fclean:
	$(MAKE) fclean -C $(SERVER)

#call re from server/Makefile
server_re:
	$(MAKE) re -C $(SERVER)
