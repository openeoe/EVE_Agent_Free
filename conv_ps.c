#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

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
	ulong psMonitorIndex;

	/** DisplayString = ASN_OCTET_STR */
	uchar psMonitorName[255]; long psMonitorName_len;

	/** DisplayString = ASN_OCTET_STR */
	uchar psMonitorParameter[255]; long psMonitorParameter_len;

	/** INTEGER = ASN_INTEGER */
	long psMonitorNumber;

	/** INTEGER = ASN_INTEGER */
	long psMonitorCheckInterval;

	/** DisplayString = ASN_OCTET_STR */
	uchar psMonitorFilter[80]; long psMonitorFilter_len;

	/** DisplayString = ASN_OCTET_STR */
	uchar psMonitorDescription[255]; long psMonitorDescription_len;

	/** DateAndTime = ASN_OCTET_STR */
	uchar psMonitorCreatedDateTime[20]; long psMonitorCreatedDateTime_len;

	/** INTEGER = ASN_INTEGER */
	long psMonitorReaction;

	/** DisplayString = ASN_OCTET_STR */
	uchar psMonitorReactionPath[255]; long psMonitorReactionPath_len;

	/** DisplayString = ASN_OCTET_STR */
	uchar psMonitorAlertMessage[255]; long psMonitorAlertMessage_len;

	/** RowStatus = ASN_INTEGER */
	long psMonitorStatus;

	/**********************************
	* OR
	*
	* Keep a pointer to your data
	*/

	void* data;
	ulong idx;
	int iThreadActiveFlag; // 0 - False, 1 - True
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
	ulong psMonitorIndex;

	/** DisplayString = ASN_OCTET_STR */
	uchar psMonitorName[255]; long psMonitorName_len;

	/** DisplayString = ASN_OCTET_STR */
	uchar psMonitorParameter[255]; long psMonitorParameter_len;

	/** INTEGER = ASN_INTEGER */
	long psMonitorNumber;

	/** INTEGER = ASN_INTEGER */
	long psMonitorCheckInterval;

	/** DisplayString = ASN_OCTET_STR */
	uchar psMonitorFilter[80]; long psMonitorFilter_len;

	/** DisplayString = ASN_OCTET_STR */
	uchar psMonitorDescription[255]; long psMonitorDescription_len;

	/** DateAndTime = ASN_OCTET_STR */
	uchar psMonitorCreatedDateTime[20]; long psMonitorCreatedDateTime_len;

	/** INTEGER = ASN_INTEGER */
	long psMonitorReaction;

	/** DisplayString = ASN_OCTET_STR */
	uchar psMonitorReactionPath[255]; long psMonitorReactionPath_len;

	/** DisplayString = ASN_OCTET_STR */
	uchar psMonitorAlertMessage[255]; long psMonitorAlertMessage_len;

	/** RowStatus = ASN_INTEGER */
	long psMonitorStatus;


	/** added by D.J Park in 2004.10.15 */
	/** INTEGER = ASN_INTEGER */
	long psMonitorThreshold;

	/** INTEGER = ASN_INTEGER */
	long psMonitorCPU;

	/** INTEGER = ASN_INTEGER */
	long psMonitorMEM;

	/** added by D.J Park in 2005.03.02 */
	/** INTEGER = ASN_INTEGER */
	long psMonitorStartHour;

	/** INTEGER = ASN_INTEGER */
	long psMonitorEndHour;

	/*
	* OR
	*
	* Keep a pointer to your data
	*/
	void* data;
	ulong idx;
	int iThreadActiveFlag; // 0 - False, 1 - True

	/** added by D.J Park in 2004.10.15 */
	int thrcpu;
	int thrmem;
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

	/* block copy */

	for(i = 0; i < item; i++) {
		memcpy((void*)&ntbl[i], (void*)&otbl[i], sizeof(oldTable)-12);
	}

	/* copy remain fields */

	for(i = 0; i < item; i++) {
		ntbl[i].data = otbl[i].data;
		ntbl[i].idx = otbl[i].idx;
		ntbl[i].iThreadActiveFlag = otbl[i].iThreadActiveFlag;
		ntbl[i].psMonitorStartHour = -1;
		ntbl[i].psMonitorEndHour = -1;
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
		ps->psMonitorIndex); 

	printf("Mon_Name=[%d][%s]\n", ps->psMonitorName_len, ps->psMonitorName);
	printf("Mon_Para=[%d][%s]\n", ps->psMonitorParameter_len, ps->psMonitorParameter);
	printf("Mon_Num=[%d]\n", ps->psMonitorNumber);
	printf("Mon_Interval=[%d]\n", ps->psMonitorCheckInterval);
	printf("Mon_Filter=[%d][%s]\n", 
		ps->psMonitorFilter_len, ps->psMonitorFilter);
	printf("Mon_Desc=[%d][%s]\n", 
		ps->psMonitorDescription_len, ps->psMonitorDescription);
	printf("Mon_Ctime=[%d][%s]\n", 
		ps->psMonitorCreatedDateTime_len, ps->psMonitorCreatedDateTime);
	printf("Mon_React=[%d]\n", ps->psMonitorReaction);
	printf("Mon_ReactPath=[%d][%s]\n", 
		ps->psMonitorReactionPath_len, ps->psMonitorReactionPath);
	printf("Mon_AlertM=[%d][%s]\n", 
		ps->psMonitorAlertMessage_len, ps->psMonitorAlertMessage);
	printf("Mon_Status=[%d]\n", ps->psMonitorStatus);
	printf("Mon_data=[%d.%d.%d.%d]\n", ps->data);
		(ps->data && 0xFF000000) >> 24, 
		(ps->data && 0x00FF0000) >> 16, 
		(ps->data && 0x0000FF00) >> 8, 
		(ps->data && 0x000000FF),
	printf("Mon_idx=[%d]\n", ps->idx);
	printf("ThreadFlag=[%d]\n", ps->iThreadActiveFlag);
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
		ps->psMonitorIndex); 

	printf("Mon_Name=[%d][%s]\n", ps->psMonitorName_len, ps->psMonitorName);
	printf("Mon_Para=[%d][%s]\n", ps->psMonitorParameter_len, ps->psMonitorParameter);
	printf("Mon_Num=[%d]\n", ps->psMonitorNumber);
	printf("Mon_Interval=[%d]\n", ps->psMonitorCheckInterval);
	printf("Mon_Filter=[%d][%s]\n", 
		ps->psMonitorFilter_len, ps->psMonitorFilter);
	printf("Mon_Desc=[%d][%s]\n", 
		ps->psMonitorDescription_len, ps->psMonitorDescription);
	printf("Mon_Ctime=[%d][%s]\n", 
		ps->psMonitorCreatedDateTime_len, ps->psMonitorCreatedDateTime);
	printf("Mon_React=[%d]\n", ps->psMonitorReaction);
	printf("Mon_ReactPath=[%d][%s]\n", 
		ps->psMonitorReactionPath_len, ps->psMonitorReactionPath);
	printf("Mon_AlertM=[%d][%s]\n", 
		ps->psMonitorAlertMessage_len, ps->psMonitorAlertMessage);
	printf("Mon_Status=[%d]\n", ps->psMonitorStatus);

	printf("T:C:M=[%d:%d:%d], Hour=[%d:%d]\n", 
		ps->psMonitorThreshold,
		ps->psMonitorCPU,
		ps->psMonitorMEM,
		ps->psMonitorStartHour,
		ps->psMonitorEndHour);

	printf("Mon_data=[%d.%d.%d.%d]\n", ps->data);
		(ps->data && 0xFF000000) >> 24, 
		(ps->data && 0x00FF0000) >> 16, 
		(ps->data && 0x0000FF00) >> 8, 
		(ps->data && 0x000000FF),
	printf("Mon_idx=[%d]\n", ps->idx);
	printf("ThreadFlag=[%d]\n", ps->iThreadActiveFlag);
	printf("cpu:mem=[%d:%d]\n", ps->thrcpu, ps->thrmem);
	printf("----------------------------------------\n");
}

