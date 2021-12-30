#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

typedef unsigned char uchar;

typedef unsigned long ulong;


typedef struct {
	ulong index;

	ulong oid;

	/*************************************************************
	* You can store data internally in this structure.
	*
	* TODO: You will probably have to fix a few types here...
	*/

	/** UNSIGNED32 = ASN_UNSIGNED */
	ulong logMonRowIndex;

	/** DisplayString = ASN_OCTET_STR */
	uchar logMonRowLogFile[255]; long logMonRowLogFile_len;

	/** INTEGER = ASN_INTEGER */
	long logMonRowReadInterval;

	/** DisplayString = ASN_OCTET_STR */
	uchar logMonRowFilter[255]; long logMonRowFilter_len;

	/** DisplayString = ASN_OCTET_STR */
	uchar logMonRowDescription[255]; long logMonRowDescription_len;

	/** DateAndTime = ASN_OCTET_STR */
	uchar logMonRowCreatedTime[20]; long logMonRowCreatedTime_len;

	/** DisplayString = ASN_OCTET_STR */
	uchar logMonRowAlertMessage[255]; long logMonRowAlertMessage_len;

	/** RowStatus = ASN_INTEGER */
	long logMonRowStatus;

	/** DateAndTime = ASN_OCTET_STR */
	uchar logMonRowUpdatedTime[20]; long logMonRowUpdatedTime_len;

	/** INTEGER = ASN_INTEGER */
	long logMonRowLine;

	/** INTEGER = ASN_INTEGER */
	long logMonRowGetLine;

	/** INTEGER = ASN_INTEGER */
	long logMonRowFileSize;

	/** INTEGER = ASN_INTEGER */
	long logMonRowReadLine;

	/*
	* OR
	*
	* Keep a pointer to your data
	*/
	void * data;
	unsigned long idx;
	int iThreadActiveFlag; // 0 - False, 1 - True

	char acOldFileName[MAXPATHLEN+1];

	/** added by D.J Park in 2004.03.30 [start]*/
	char acChangedFileName[MAXPATHLEN+1];
	long logMonRowReadLineSeek;
	int isStart;
	/** added by D.J Park in 2004.03.30 [end]*/

} oldTable;


typedef struct {
	ulong index;

	ulong oid;

	/*************************************************************
	* You can store data internally in this structure.
	*
	* TODO: You will probably have to fix a few types here...
	*/
	/** UNSIGNED32 = ASN_UNSIGNED */
	unsigned long logMonRowIndex;

	/** DisplayString = ASN_OCTET_STR */
	unsigned char logMonRowLogFile[255]; long logMonRowLogFile_len;

	/** INTEGER = ASN_INTEGER */
	long logMonRowReadInterval;

	/** DisplayString = ASN_OCTET_STR */
	unsigned char logMonRowFilter[255]; long logMonRowFilter_len;

	/** DisplayString = ASN_OCTET_STR */
	unsigned char logMonRowDescription[255]; long logMonRowDescription_len;

	/** DateAndTime = ASN_OCTET_STR */
	unsigned char logMonRowCreatedTime[20]; long logMonRowCreatedTime_len;

	/** DisplayString = ASN_OCTET_STR */
	unsigned char logMonRowAlertMessage[255]; long logMonRowAlertMessage_len;

	/** RowStatus = ASN_INTEGER */
	long logMonRowStatus;

	/** DateAndTime = ASN_OCTET_STR */
	unsigned char logMonRowUpdatedTime[20]; long logMonRowUpdatedTime_len;

	/** INTEGER = ASN_INTEGER */
	long logMonRowLine;

	/** INTEGER = ASN_INTEGER */
	long logMonRowGetLine;

	/** INTEGER = ASN_INTEGER */
	long logMonRowFileSize;

	/** INTEGER = ASN_INTEGER */
	long logMonRowReadLine;

	/** added by D.J Park in 2005.02.22 [start]*/

	/** INTEGER = ASN_INTEGER */
	long logMonRowFileSizeThreshold;

	/** INTEGER = ASN_INTEGER */
	long logMonRowFilterCounter;

	/** INTEGER = ASN_INTEGER */
	long logMonRowTrapType;

	/** added by D.J Park in 2005.02.22 [end]*/

	/*
	* OR
	*
	* Keep a pointer to your data
	*/
	void * data;

	unsigned long idx;
	int iThreadActiveFlag; // 0 - False, 1 - True
	char acOldFileName[MAXPATHLEN+1];

	/** added by D.J Park in 2004.03.30 [start]*/
	char acChangedFileName[MAXPATHLEN+1];
	long logMonRowReadLineSeek;
	int isStart;
	/** added by D.J Park in 2004.03.30 [end]*/

	/** added by D.J Park in 2004.02.22 [start]*/
	long filterCounter;
	/** added by D.J Park in 2004.02.22 [end]*/

} newTable;


void printOldTable(oldTable* ps);
void printNewTable(newTable* ps);
int isOldTable(char* file);

int numOldTable(char* file);
int numNewTable(char* file);

int copyOldItem(char* filename, oldTable* otbl, int item);
int copyNewItem(char* filename, newTable* ntbl, int item);

void convertItem(oldTable* otbl, newTable* ntbl, int item);
int	restoreItem(char* filename, newTable* ntbl, int item);

int main(int argc, char* argv[])
{
	int i = 0;
	int c = 0;
	int num = 0;
	int item = -1;
	int convert = 0;
	int print = 0;
	int help = 0;
	int err = 0;
	char* filename = NULL;

	oldTable* otbl = NULL;
	newTable* ntbl = NULL;

    while((c = getopt(argc, argv, "hpcf:")) != EOF) {
        switch(c) {
        case 'f': filename = strdup(optarg); break;
        case 'c': convert = 1; break;
        case 'p': print = 1; break;
        case 'h': help = 1; break;
        default : err = 1; break;
        }
    }

	if (err > 0 || help > 0 || filename == NULL) {
		printf("Usage: %s [-c] [-p] -f filename\n", argv[0]);
		printf("    -c : convert file\n");
		printf("    -p : print file\n");
		printf("sizeof(oldTable)=[%d]\n", sizeof(oldTable));
		printf("sizeof(newTable)=[%d]\n", sizeof(newTable));
		exit(0);
	}

	item = isOldTable(filename);

	if (item > 0) {					/* old structure */
		otbl = (oldTable*)malloc(sizeof(oldTable) * item);
		ntbl = (newTable*)malloc(sizeof(newTable) * item);

		memset(otbl, 0, sizeof(oldTable) * item);
		memset(ntbl, 0, sizeof(newTable) * item);

		copyOldItem(filename, otbl, item);

		if (print > 0) {
			for (i = 0; i < item; i++) {
				printOldTable(&otbl[i]);
			}
		}

		if (convert > 0) {
			convertItem(otbl, ntbl, item);
			restoreItem(filename, ntbl, item);
		}
	}

	if (item == 0) {				/* new structure */
		num = numNewTable(filename);

		ntbl = (newTable*)malloc(sizeof(newTable) * num);

		memset(ntbl, 0, sizeof(newTable) * num);

		copyNewItem(filename, ntbl, num);

		if (print > 0) {
			for (i = 0; i < num; i++) {
				printNewTable(&ntbl[i]);
			}
		}
	}

	exit(0);
}


int copyOldItem(char* filename, oldTable* otbl, int item)
{
	int i = 0;
	int len = 0;
	int read = 0;
	FILE* fp = NULL;

	fp = fopen(filename, "r");

	if (fp == NULL) {
		fprintf(stderr, "%s file failed to open for reading\n", filename);
		return (-1);
	}

	for(i = 0; i < item; i++) {
		read = fread((void*)&otbl[i], sizeof(oldTable), 1, fp);

		if (read != 1) {
			fprintf(stderr, "%s fread failed\n", filename);
			fclose(fp);
			return (-1);
		}
	}

	fclose(fp);

	return (1);
}

int copyNewItem(char* filename, newTable* ntbl, int item)
{
	int i = 0;
	int len = 0;
	int read = 0;
	FILE* fp = NULL;

	fp = fopen(filename, "rb");

	if (fp == NULL) {
		fprintf(stderr, "%s file failed to open for reading\n", filename);
		return (-1);
	}

	for(i = 0; i < item; i++) {
		read = fread((void*)&ntbl[i], sizeof(newTable), 1, fp);

		if (read != 1) {
			fprintf(stderr, "%s fread failed\n", filename);
			fclose(fp);
			return (-1);
		}
	}

	fclose(fp);

	return (1);
}

void convertItem(oldTable* otbl, newTable* ntbl, int item)
{
	int i = 0; 
	oldTable o;

	/* block copy */

	for(i = 0; i < item; i++) {
		memcpy((void*)&ntbl[i], (void*)&otbl[i], 
			sizeof(oldTable) - (sizeof(o.acOldFileName)*2 + 20));
	}

	/* copy remain fields */

	for(i = 0; i < item; i++) {
		ntbl[i].logMonRowFileSizeThreshold = 0;
		ntbl[i].logMonRowFilterCounter = 1;
		ntbl[i].logMonRowTrapType = 1;

		ntbl[i].data = otbl[i].data;
		ntbl[i].idx = otbl[i].idx;
		ntbl[i].iThreadActiveFlag = otbl[i].iThreadActiveFlag;

		strncpy(ntbl[i].acOldFileName, otbl[i].acOldFileName, MAXPATHLEN+1);
		strncpy(ntbl[i].acChangedFileName, otbl[i].acChangedFileName, MAXPATHLEN+1);
		ntbl[i].logMonRowReadLineSeek = otbl[i].logMonRowReadLineSeek;
		ntbl[i].isStart = otbl[i].isStart;
		ntbl[i].filterCounter = 0;
	}
}

int	restoreItem(char* filename, newTable* ntbl, int item)
{
	int i = 0;
	int len = 0;
	int write = 0;
	FILE* fp = NULL;

	fp = fopen(filename, "wb");

	if (fp == NULL) {
		fprintf(stderr, "%s file failed to open for writing\n", filename);
		return (-1);
	}

	for(i = 0; i < item; i++) {
		write = fwrite((void*)&ntbl[i], sizeof(newTable), 1, fp);

		if (write != 1) {
			fprintf(stderr, "%s fwrite failed\n", filename);
			fclose(fp);
			return (-1);
		}
	}

	fclose(fp);

	return (1);
}

int isOldTable(char* file)
{
	struct stat stbuf;
	int file_size;
	int ret = -1;

	memset(&stbuf, 0, sizeof(stbuf));

	if (stat(file, &stbuf) == -1) {		/* error */
		ret = -1;
	} else {
		file_size = stbuf.st_size;	

		if ((file_size % sizeof(oldTable)) == 0) {
			ret = (file_size / sizeof(oldTable));		/* old table */
		} else if ((file_size % sizeof(newTable)) == 0) {
			ret = 0; 									/* new table */
		}
	}

	return (ret);
}

int numNewTable(char* file)
{
	struct stat stbuf;
	int file_size;
	int ret = -1;

	memset(&stbuf, 0, sizeof(stbuf));

	if (stat(file, &stbuf) == -1) {		/* error */
		ret = -1;
	} else {
		file_size = stbuf.st_size;	

		if ((file_size % sizeof(newTable)) == 0) {
			ret = (file_size / sizeof(newTable));		/* new table */
		}
	}

	return (ret);
}

void printOldTable(oldTable* ps)
{
	printf("index=[%d], oid=[%d.%d.%d.%d], Index=[%d]\n",
		ps->index, 
		(ps->oid && 0xFF000000) >> 24, 
		(ps->oid && 0x00FF0000) >> 16, 
		(ps->oid && 0x0000FF00) >> 8, 
		(ps->oid && 0x000000FF),
		ps->logMonRowIndex); 

	printf("Mon_File=[%d][%s]\n", ps->logMonRowLogFile_len, ps->logMonRowLogFile);
	printf("Mon_Interval=[%d]\n", ps->logMonRowReadInterval);
	printf("Mon_Filter=[%d][%s]\n", 
		ps->logMonRowFilter_len, ps->logMonRowFilter);
	printf("Mon_Desc=[%d][%s]\n", 
		ps->logMonRowDescription_len, ps->logMonRowDescription);
	printf("Mon_Ctime=[%d][%s]\n", 
		ps->logMonRowCreatedTime_len, ps->logMonRowCreatedTime);
	printf("Mon_AlertM=[%d][%s]\n", 
		ps->logMonRowAlertMessage_len, ps->logMonRowAlertMessage);
	printf("Mon_Status=[%d]\n", ps->logMonRowStatus);
	printf("Mon_Utime=[%d][%s]\n", 
		ps->logMonRowUpdatedTime_len, ps->logMonRowUpdatedTime);
	printf("Mon_RowLine=[%d]\n", ps->logMonRowLine);
	printf("Mon_GetLine=[%d]\n", ps->logMonRowGetLine);
	printf("Mon_FileSize=[%d]\n", ps->logMonRowFileSize);
	printf("Mon_ReadLine=[%d]\n", ps->logMonRowReadLine);

	printf("Mon_data=[%d.%d.%d.%d]\n", ps->data);
		(ps->data && 0xFF000000) >> 24, 
		(ps->data && 0x00FF0000) >> 16, 
		(ps->data && 0x0000FF00) >> 8, 
		(ps->data && 0x000000FF),
	printf("Mon_idx=[%d]\n", ps->idx);
	printf("ThreadFlag=[%d]\n", ps->iThreadActiveFlag);

	printf("oldFileName=[%s]\n", ps->acOldFileName);
	printf("changedFileName=[%s]\n", ps->acChangedFileName);
	printf("ReadLineSeek=[%d]\n", ps->logMonRowReadLineSeek);
	printf("isStart=[%d]\n", ps->isStart);
	printf("----------------------------------------\n");
}

void printNewTable(newTable* ps)
{
	printf("index=[%d], oid=[%d.%d.%d.%d], Index=[%d]\n",
		ps->index, 
		(ps->oid && 0xFF000000) >> 24, 
		(ps->oid && 0x00FF0000) >> 16, 
		(ps->oid && 0x0000FF00) >> 8, 
		(ps->oid && 0x000000FF),
		ps->logMonRowIndex); 

	printf("Mon_File=[%d][%s]\n", ps->logMonRowLogFile_len, ps->logMonRowLogFile);
	printf("Mon_Interval=[%d]\n", ps->logMonRowReadInterval);
	printf("Mon_Filter=[%d][%s]\n", 
		ps->logMonRowFilter_len, ps->logMonRowFilter);
	printf("Mon_Desc=[%d][%s]\n", 
		ps->logMonRowDescription_len, ps->logMonRowDescription);
	printf("Mon_Ctime=[%d][%s]\n", 
		ps->logMonRowCreatedTime_len, ps->logMonRowCreatedTime);
	printf("Mon_AlertM=[%d][%s]\n", 
		ps->logMonRowAlertMessage_len, ps->logMonRowAlertMessage);
	printf("Mon_Status=[%d]\n", ps->logMonRowStatus);
	printf("Mon_Utime=[%d][%s]\n", 
		ps->logMonRowUpdatedTime_len, ps->logMonRowUpdatedTime);
	printf("Mon_RowLine=[%d]\n", ps->logMonRowLine);
	printf("Mon_GetLine=[%d]\n", ps->logMonRowGetLine);
	printf("Mon_FileSize=[%d]\n", ps->logMonRowFileSize);
	printf("Mon_ReadLine=[%d]\n", ps->logMonRowReadLine);
	printf("Mon_Threshold=[%d]\n", ps->logMonRowFileSizeThreshold);
	printf("Mon_FilterCnt=[%d]\n", ps->logMonRowFilterCounter);
	printf("Mon_TrapType=[%d]\n", ps->logMonRowTrapType);

	printf("Mon_data=[%d.%d.%d.%d]\n", ps->data);
		(ps->data && 0xFF000000) >> 24, 
		(ps->data && 0x00FF0000) >> 16, 
		(ps->data && 0x0000FF00) >> 8, 
		(ps->data && 0x000000FF),
	printf("Mon_idx=[%d]\n", ps->idx);
	printf("ThreadFlag=[%d]\n", ps->iThreadActiveFlag);

	printf("oldFileName=[%s]\n", ps->acOldFileName);
	printf("changedFileName=[%s]\n", ps->acChangedFileName);
	printf("ReadLineSeek=[%d]\n", ps->logMonRowReadLineSeek);
	printf("isStart=[%d]\n", ps->isStart);
	printf("filterCounter=[%d]\n", ps->filterCounter);
	printf("----------------------------------------\n");
}

