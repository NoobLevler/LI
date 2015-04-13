#pragma once
#include "BST.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "StringList.h"

typedef struct clientCatalog *ClientCatalog;

typedef struct client
{
	char *name;
	void **data;
	int *dataSize;
}*Client;

ClientCatalog initClientCatalog();
ClientCatalog insertClient(ClientCatalog cat, char *client);
int existsClient(ClientCatalog cat, char *client);
int getClientCount( ClientCatalog clientCat );
Client getClient(ClientCatalog cat, char *client);
int freeClientCatalog(ClientCatalog cat);
StringList getClientsByPrefix(ClientCatalog cat, char t );
