#include "Sales.h"

struct productBuyers
{
	char **listN, **listP;
	int cntN, cntP;
};

typedef struct entry
{
	int cnt[12]/*Number of Index entries per Month (width of matrix at that row)*/, cntS[12];
	int units;
	int **records; /* Matrix Month x Index */
	char name[8];/*debug only*/
}*Entry_s;


struct montly_purchases
{
	char **list;
	int *cnt;
	int size;
};


typedef struct sales
{
	int cntEC, cntEP, sizeEC, sizeEP, sizeS, cntS;
	Entry_s *entriesCli, *entriesPr;
	Sale *sales;
}*Sales_s;

Entry_s initEntry();

Sales_s initSales()
{
	Sales_s acc = malloc(sizeof(*acc));
	acc->cntEC = 0;
	acc->cntEP = 0;
	acc->sizeEC = 0;
	acc->sizeEP = 0;
	acc->sizeS = 0;
	acc->cntS = 0;
	acc->entriesCli = NULL;
	acc->entriesPr = NULL;
	acc->sales = NULL;
	return acc;
}


Sales_s addSale(Sales_s acc, ClientCatalog cCat, ProductCatalog pCat, Sale sale)
{
	/*Misc vars*/
	int i, monthIdx;

	/*Vars for product binding*/
	Product pr = NULL;
	Entry_s tE = NULL;
	Client cl = NULL;

	/*Check if the sizes allow for insertion, if not realloc stuff*/

	if (acc->cntEC == acc->sizeEC)/*Client Entry_s array is full*/
	{
		if (!acc->sizeEC)
			acc->sizeEC = 2;
		else
			acc->sizeEC += acc->sizeEC;

        acc->entriesCli = realloc( acc->entriesCli,
                            ( sizeof(*acc->entriesCli) * acc->sizeEC ) );

		for (i = acc->sizeEC / 2; i < acc->sizeEC; i++)
			acc->entriesCli[i] = NULL;

	}

	if (acc->cntEP == acc->sizeEP)/*Product Entry_s array is full*/
	{

		if (!acc->sizeEP)
			acc->sizeEP = 2;
		else
			acc->sizeEP += acc->sizeEP;

        acc->entriesPr = realloc( acc->entriesPr,
                            ( sizeof(*acc->entriesPr) * acc->sizeEP ) );

		for (i = acc->sizeEP / 2; i < acc->sizeEP; i++)
			acc->entriesPr[i] = NULL;

	}

	if (acc->cntS == acc->sizeS)/*Sales array is full*/
	{

		if (!acc->sizeS)
			acc->sizeS = 2;
		else
			acc->sizeS += acc->sizeS;

        acc->sales = realloc( acc->sales,
                        (sizeof( *acc->sales ) * acc->sizeS ) );

		for (i = acc->sizeS / 2; i < acc->sizeS; i++)
			acc->sales[i] = NULL;

	}

	/*Done size checking*/



	/* First, get, if valid, Product/Client */
	if( matchProductPattern( getSaleProduct( sale ) ) )
        pr = getProduct(pCat, getSaleProduct( sale ) );
	if (!pr)
		return acc;

    if( matchClientPattern( getSaleClient( sale ) ) )
        cl = getClient(cCat, getSaleClient( sale ) );
	if (!cl)
		return acc;

	monthIdx = getSaleMonth( sale ) - 1;

	/*Next, add sale to the registry*/
	if (!copySale(&acc->sales[acc->cntS], sale))
		return acc;

	/*Now bind the sale to the product*/
	/*Check if the product already has an entry bound by checking metadata*/
	if (getProductDataSize(pr) != 0)
	{
		i = getProductMetaData(pr); /*Index of entriePr where the product is bound*/

		tE = acc->entriesPr[i];

		/*Check if there is space to add Entry_s, else realloc*/
		if (tE->cnt[monthIdx] == tE->cntS[monthIdx])
		{
			tE->cntS[monthIdx] *= 2;
			tE->records[monthIdx] = realloc(tE->records[monthIdx], sizeof(int)*tE->cntS[monthIdx]);
		}
		tE->records[monthIdx][tE->cnt[monthIdx]] = acc->cntS;
		tE->units += getSaleQtd( sale );
		tE->cnt[(monthIdx)] ++;

		/*copyEntry(&tE, tE);*/
	}/*Else create new Product Entry_s, update metadata*/else{

		tE = acc->entriesPr[ acc->cntEP ] = initEntry();
		tE->cnt[monthIdx] = 1;
		tE->records[monthIdx][0] = acc->cntS;
		tE->units += getSaleQtd( sale );

		allocProductMetaData(pr,sizeof(int));
		setProductMetaData(pr,acc->cntEP);
		setProductDataSize(pr,sizeof(int));
		strcpy(tE->name, getProductName(pr));
		/*copyEntry(&tE, tE);*/
        acc->cntEP++;
	}


	/*Now bind the sale to the client*/

	/*Check if the client already has an entry bound by checking metadata*/
	if (getClientDataSize(cl) != 0)
	{
		i = getClientMetaData(cl); /*Index of entriecl where the client is bound*/
		tE = acc->entriesCli[i];

		/*Check if there is space to add Entry_s, else realloc*/
		tE->cnt[ monthIdx ] = tE->cnt[ monthIdx ];
		if (tE->cnt[monthIdx] == tE->cntS[monthIdx])
		{
			tE->cntS[monthIdx] *= 2;
			tE->records[monthIdx] = realloc(tE->records[monthIdx], sizeof(int)*tE->cntS[monthIdx]);
		}
		tE->records[monthIdx][tE->cnt[monthIdx]] = acc->cntS;
		tE->units += getSaleQtd( sale );
		tE->cnt[(monthIdx)] ++;


		/*copyEntry(&tE, tE);*/
	}/*Else create new client Entry_s, update metadata*/else{

		tE = acc->entriesCli[ acc->cntEC ] = initEntry();
		tE->cnt[monthIdx] = 1;
		tE->records[monthIdx][0] = acc->cntS;
		tE->units += getSaleQtd( sale );

		allocClientMetaData(cl, sizeof(int));
		setClientMetaData(cl, acc->cntEC);
		setClientDataSize(cl, sizeof(int));
		strcpy(tE->name, getClientName(cl));
		/*copyEntry(&acc->entriesCli[acc->cntEC], tE);*/
    acc->cntEC++;
	}

	acc->cntS++;

	return acc;
}



Sales_s orderSales(Sales_s acc, ProductCatalog pCat, ClientCatalog cCat)
{
	minHeap h1 = newHeap(acc->cntEC), h2 = newHeap(acc->cntEP);
	int letter, i, metaI, lSize, hUsed;
	char **lists;
	Client cl; Product pr;
	Entry_s cliE, prE, *tCE, *tPE;
	Elem e;

	StringList sl;
	for (letter = 0; letter < 26; letter++)
	{
		sl = getClientsByPrefix(cCat, 'A' + letter);
		lSize = getCountStringList(sl);
		lists = getStringList(sl);
		for (i = 0; i < lSize; i++)
		{
			cl = getClient(cCat, lists[i]);
			if (!getClientMetaDataAddr(cl))
			{
				/*Client has no records*/
			}
			else
			{
				metaI = getClientMetaData(cl);
				cliE = acc->entriesCli[metaI];
				insertHeap(h1, cliE->units, cliE, sizeof cliE);
			}
			/*free(lists[i]);*/
		}
		/*free(lists);*/
	}
	freeStringList(sl);
	for (letter = 0; letter < 26; letter++)
	{
		sl = getProductsByPrefix(pCat, 'A' + letter);
		lSize = getCountStringList(sl);
		lists = getStringList(sl);
		for (i = 0; i < lSize; i++)
		{
			pr = getProduct(pCat, lists[i]);
			if (!getProductMetaDataAddr(pr))
			{
				/*Client has no records*/
			}
			else
			{
				metaI = getProductMetaData(pr);
				prE = acc->entriesPr[metaI];
				insertHeap(h2, prE->units, prE, sizeof prE);
			}
			/*free(lists[i]);*/
		}
		/*free(lists);*/
	}
	freeStringList(sl);
	hUsed = getUsed(h1);
	acc->cntEC = hUsed;
	tCE = malloc(sizeof *tCE * acc->cntEC);
	for (i = 0; i < hUsed; i++)
	{
		e = extractMin(h1);
		tCE[i] = (Entry_s)getElemDataAddr(e);
		cl = getClient(cCat, tCE[i]->name);
		setClientMetaData(cl,i);
	}
	acc->entriesCli = tCE;

	hUsed = getUsed(h2);
	acc->cntEP = hUsed;
	tPE = malloc(sizeof *tPE * acc->cntEP);
	for (i = 0; i < hUsed; i++)
	{
		e = extractMin(h2);
		tPE[i] = (Entry_s)getElemDataAddr(e);
		pr = getProduct(pCat, tPE[i]->name);
		setProductMetaData(pr,i);
	}
	acc->entriesPr = tPE;
	return acc;
}

Entry_s initEntry()
{
	Entry_s e = malloc(sizeof(*e));
	int i;


	e->records = malloc(sizeof(int*) * 12);
	for (i = 0; i < 12; i++)
	{
		e->cnt[i] = 0;
		e->cntS[i] = 1;
		e->records[i] = malloc(sizeof(int));
	}
	e->units = 0;
	strcpy( e->name, "" );

	return e;
}

int getSalesCount(Sales_s acc)
{
	return acc ? acc->cntS : -1;
}

int getEntriesClientsCount(Sales_s acc)
{
	return acc ? acc->cntEC : -1;
}

int getEntriesProductsCount(Sales_s acc)
{
	return acc ? acc->cntEP : -1;
}

int getProductSalesPerMonth(Sales_s acc, Product prod, int month, int *nSalesP, int *nSalesN, double *totalProfit)
{
	int i, idx;
	int count, countN, countP;
	double total;
	Entry_s e;


	if (!(acc->cntEP) || !getProductDataSize(prod))
		return 0;

	idx = getProductMetaData(prod);

	e = acc->entriesPr[idx];

	if (!e)
		return 0;

	month--;

	count = e->cnt[month];

	countN = countP = 0;
	total = 0.0;

	for (i = 0; i < count; i++)
	{
		if ( getSaleType( acc->sales[e->records[month][i]] ) == 'N')
			countN++;
		else
			countP++;
		total += getSalePrice( acc->sales[e->records[month][i]] );
	}

	*nSalesN = countN;
	*nSalesP = countP;
	*totalProfit = total;

	return count;
}


ProductBuyers productBuyers(Sales_s acc, ProductCatalog pCat, ClientCatalog cCat, char *product)
{
	ProductBuyers toRet = malloc(sizeof *toRet);
	ClientCatalog tmpP = initClientCatalog();
	ClientCatalog tmpN = initClientCatalog();
	Product pr = getProduct(pCat, product);
	int index = getProductMetaData(pr);
	int i = 0, j;
	Entry_s p = acc->entriesPr[index];
	Sale s;

	toRet->cntN = 0; toRet->cntP = 0; toRet->listN = NULL; toRet->listP = NULL;
	for (i = 0; i < 12; i++)
	{
		for (j = 0; j < p->cnt[i]; j++)
		{
			s = acc->sales[p->records[i][j]];
			if ( getSaleType( s ) == 'N')
			{
				if (!existsClient(tmpN, getSaleClient( s ) ) )
				{
					insertClient(tmpN, getSaleClient( s ) );
					toRet->listN = realloc(toRet->listN, sizeof(char*) * toRet->cntN + 1);
					toRet->listN[toRet->cntN] = malloc(sizeof(char) * 6);
					strcpy(toRet->listN[toRet->cntN++], getSaleClient( s ) );
				}
			}
			if ( getSaleType( s ) == 'P')
			{
				if (!existsClient(tmpP, getSaleClient( s ) ) )
				{
					insertClient(tmpP, getSaleClient( s ) );
					toRet->listP = realloc(toRet->listP, sizeof(char*) * toRet->cntP + 1);
					toRet->listP[toRet->cntP] = malloc(sizeof(char) * 6);
					strcpy(toRet->listP[toRet->cntP++], getSaleClient( s ) );
				}
			}
		}
	}
	return toRet;
}

Monthly_Purchases mostBoughtMonthlyProductsByClient(Sales_s acc, ProductCatalog pCat, ClientCatalog cCat, char *client, int month)
{
	StringList sl = initStringList();
	ProductCatalog tmp = initProductCatalog();	Product pr;
	Client cl = getClient(cCat, client);
	Entry_s e;
	Sale s;
	minHeap h;
	int i, j, letter, lSize, cnt = 0, used, *tmpMP;
	Elem el;
	char **lists;
	Monthly_Purchases mp = initMonthlyPurchases();
	int metaI;


	if (!cl)
		return NULL;
	metaI = getClientMetaData(cl);
	e = acc->entriesCli[metaI];
	if (!e)
		return NULL;
	h = newHeap(acc->cntEP);
	for (i = 0; i < 12; i++)
	{
		for (j = 0; j < e->cnt[i]; j++)
		{
			s = acc->sales[e->records[i][j]];
			if (existsProduct(tmp, getSaleProduct( s ) ))
			{
				pr = getProduct(tmp, getSaleProduct( s ) );
				/*letter = *(*(int**)pr->data);*/
				setProductMetaData(pr, getProductMetaData(pr) + 1);
			}
			else
			{
				insertProduct(tmp, getSaleProduct( s ) );
				pr = getProduct(tmp, getSaleProduct( s ) );
				allocProductDataSize(pr, sizeof(int));
				setProductMetaData(pr, 1);
				pr = getProduct(tmp, getSaleProduct( s ) );
				cnt++;
			}
		}
	}

	for (letter = 0; letter < 26; letter++)
	{
		sl = getProductsByPrefix(tmp, 'A' + letter);
		lSize = getCountStringList(sl);
		lists = getStringList(sl);
		for (i = 0; i < lSize; i++)
		{
			pr = getProduct(tmp, lists[i]);
			if (getProductMetaDataAddr(pr) == NULL)
			{
				/*Client has no records*/
			}
			else
			{
				j = getProductMetaData(pr);
				insertHeap(h, j, getProductName(pr), sizeof(char) * 6);
			}
			/*free(lists[i]);*/
		}
		/*free(lists);*/
	}
	freeStringList(sl);
	sl = initStringList();
	used = getUsed(h);
	lists = malloc(sizeof(char*)*used);
	tmpMP = malloc(sizeof(int)*used);
	cnt = 0;
	for (i = 0; i < used; i++)
	{
		el = extractMin(h);
		lists[cnt] = getElemDataAddr(el);
		tmpMP[cnt++] = getElemKey(el);
	}
	for (i = cnt - 1; i > 0; i--)
	{
		registerMonthlyPurchase(mp, lists[i],tmpMP[i]);
		printf("%s: %d\n", mp->list[mp->size - 1], mp->cnt[mp->size - 1]);
	}
	return mp;

}

StringList yearRoundClients(Sales_s acc, ProductCatalog pCat, ClientCatalog cCat)
{
	StringList toRet = initStringList();
	int i = acc->cntEC - 1, j,k;
	Entry_s ent = acc->entriesCli[i--];
	while (ent && ent->units > 12 &&  i > 0)
	{
		k = 1;
		for (j = 0; j < 12 && k; j++)
		{
			if (ent->cnt[j] < 1)
				k = 0;
		}
		if (k)
			insertStringList(toRet, ent->name, sizeof ent->name);
		ent = acc->entriesCli[i--];
	}
	return toRet;
}

StringList mostSoldProducts(Sales_s acc, ProductCatalog pCat, ClientCatalog cCat, int N)
{
	StringList sl = initStringList();
	int i,j,k;
	int cli = 0;
	Entry_s ent;
	ClientCatalog tmp = initClientCatalog();
	Sale s;
	char *buff = calloc(50, sizeof (char));
	char b[10];
	for (i = acc->cntEP - 1, N; i > 0 && N > 0; i--, N--)
	{
		ent = acc->entriesPr[i];
		for (j = 0; j < 12; j++)
			for (k = 0; k < ent->cnt[j]; k++)
			{
				s = acc->sales[ent->records[j][k]];
				if (!existsClient(tmp, getSaleClient( s ) ))
				{
					insertClient(tmp, getSaleClient( s ) );
					cli++;
				}
			}
		memset(buff, '\0', 50);
		strcat(buff, ent->name);
		strcat(buff, " for ");
		sprintf( b, "%d", cli );
		strcat(buff, b);
		strcat(buff, " clients and ");
		sprintf( b, "%d", ent->units );
		strcat(buff, b);
		strcat(buff, " units\n");
		insertStringList(sl, buff, 50);
		cli = 0;
	}
	return sl;
}


StringList productsWithoutPurchases(Sales_s acc)
{
	int i;
	StringList sl = initStringList();
	for (i = 0; i < acc->cntEP && acc->entriesPr[i]->units == 0; i++)
	{
		insertStringList(sl, acc->entriesPr[i]->name, 7);
	}
	return sl;

}

StringList clientsWithoutPurchases(Sales_s acc)
{
	int i;
	StringList sl = initStringList();
	for (i = 0; i < acc->cntEC && acc->entriesCli[i]->units == 0; i++)
	{
		insertStringList(sl, acc->entriesCli[i]->name, 6);
	}
	return sl;

}
/*MONTHLY PURCHASES FUNCTIONS*/

Monthly_Purchases initMonthlyPurchases()
{
	Monthly_Purchases toRet = malloc(sizeof *toRet);
	toRet->cnt = NULL;
	toRet->list = NULL;
	toRet->size = 0;
	return toRet;
}

Monthly_Purchases registerMonthlyPurchase(Monthly_Purchases mp, char *product, int cnt)
{
	mp->list = realloc(mp->list, sizeof(char*) * mp->size + 1);
	mp->list[mp->size] = malloc(sizeof(char) * 6);
	strcpy(mp->list[mp->size], product);
	mp->cnt = realloc(mp->cnt, sizeof(int) * mp->size + 1);
	mp->cnt[mp->size++] = cnt;
	return mp;
}

char **getMonthlyPurchasesList(Monthly_Purchases mp)
{
	return mp->list;
}
int *getMonthlyPurchasesCounts(Monthly_Purchases mp)
{
	return mp->cnt;
}
int getMonthlyPurchasesSize(Monthly_Purchases mp)
{
	return mp->size;
}

char ** getProductBuyersN(ProductBuyers pb)
{
	return (pb->listN ? pb->listN : NULL);
}
char ** getProductBuyersP(ProductBuyers pb)
{
	return (pb->listP ? pb->listP : NULL);
}
int getProductBuyersCntN(ProductBuyers pb)
{
	return (pb->cntN ? pb->cntN : 0);
}
int getProductBuyersCntP(ProductBuyers pb)
{
	return (pb->cntP ? pb->cntN : 0);
}
/*END MONTHLY PURCHASES FUNCTIONS*/
