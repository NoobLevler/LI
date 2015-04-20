#pragma once
#include "BST.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "StringList.h"
#include "HashTable.h"

typedef struct productCatalog *ProductCatalog;

typedef struct product *Product;

ProductCatalog initProductCatalog();
ProductCatalog insertProduct(ProductCatalog cat, char *product);
int matchProductPattern( char *p );
int existsProduct( ProductCatalog cat, char *product );
int getProductCount( ProductCatalog prodCat );
Product getProduct(ProductCatalog cat, char* product);
int freeProductCatalog(ProductCatalog cat);
StringList getProductsByPrefix(ProductCatalog cat, char t);
int getProductMetaData(Product pr);
void *getProductMetaDataAddr(Product pr);
void allocProductMetaData(Product pr, int size);
void allocProductDataSize(Product pr, int size);
void setProductDataSize(Product pr, int size);
void setProductMetaData(Product pr, int x);
char *getProductName(Product pr);
int getProductDataSize(Product pr);
