#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <gmp.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>
#define MAX_FILENAME_LENGTH 256

/*
Run : clear && gcc Decode.c -lm -o m.o && ./m.o

*/
/*Global Variables Start*/

unsigned char oneCountByteLookupTable[256]={0};
unsigned char inverseByteLookupTable[256]={0};
int headerBitIndex = 0;
int *header;
int *temporaryStarts;
int *temporaryEnds;
int bookIndex = 0;
FILE *fr;

uint32_t RANGE_LOW = 1;
uint32_t RANGE_HIGH = 0xffffffff;
uint32_t RANGE_CURRENT = 0;

void ResetGlobalVariables()
{
	RANGE_LOW = 1;
	RANGE_HIGH = 0xffffffff;
	RANGE_CURRENT = 0;
	headerBitIndex = 0;
	bookIndex = 0;
}

int Decode(float probability, int characterLength, unsigned char *characters, size_t *characterIndex)
{
	assert(probability >= 0.0f);assert(probability <= 1.0f);assert(RANGE_HIGH > RANGE_LOW);assert(RANGE_LOW >= 0);
	int intProbability = (int) (probability * 65536.0f);
	uint32_t RANGE_MID = RANGE_LOW +((RANGE_HIGH - RANGE_LOW) >> 16) * intProbability + ((((RANGE_HIGH - RANGE_LOW) & 0xffff) * intProbability) >> 16);
	assert(RANGE_HIGH >= RANGE_MID);
	assert(RANGE_MID >= RANGE_LOW);
	int bit = RANGE_CURRENT <= RANGE_MID;
	if(bit){RANGE_HIGH = RANGE_MID;}else{RANGE_LOW = RANGE_MID + 1;}
	while((RANGE_HIGH ^ RANGE_LOW) < 0x1000000)
	{
        	RANGE_HIGH = RANGE_HIGH << 8 | 255;
        	RANGE_LOW  = RANGE_LOW  << 8;
        	if(*characterIndex < characterLength-1)
        	{
        		assert(characters[*characterIndex] > -1);assert(characters[*characterIndex] < 256);
        		RANGE_CURRENT=RANGE_CURRENT<<8|characters[*characterIndex];
        		*characterIndex += 1;        	
        	}
        	else
        	{
        		assert(characters[*characterIndex] > -1);assert(characters[*characterIndex] < 256);
        		RANGE_CURRENT=RANGE_CURRENT<<8|characters[*characterIndex];
        	}
        }
        return bit;
}

/*Global Variables End*/
/*Start Fill Lookup Tables*/
void FillByteLookupTable()
{
	int oneCount = 0;
	unsigned char byte = 0;
	int bit = 0;
	for(int i = 0; i < 256; i++)
	{
		oneCount = 0;byte=0;
		for(int j = 0 ; j < 8; j++)
		{
			bit = (i >> j) & 1;
			oneCount += bit;
			byte<<=1;
			byte += abs(bit-1);
		}
		oneCountByteLookupTable[i] = oneCount;
		inverseByteLookupTable[i]=byte;
	}
}


int Book(size_t startIndex, size_t endIndex, int currentLevel, int maxLevel, int *temporaryStarts, int *temporaryEnds)
{
	if(currentLevel == maxLevel){return 1;}	
	size_t midPoint = ((endIndex - startIndex) / 2) + startIndex;

	temporaryStarts[bookIndex] = startIndex;
	temporaryEnds[bookIndex] = endIndex;
	bookIndex += 1;
	Book(startIndex, midPoint, currentLevel+1, maxLevel, temporaryStarts, temporaryEnds);
	Book(midPoint, endIndex, currentLevel+1, maxLevel,  temporaryStarts, temporaryEnds);
}

int MakeMoreUniform(size_t startIndex, size_t endIndex, size_t fileSize, unsigned char *fileData, int maxLevel)
{
	int temporaryLength = 1 << maxLevel;
	temporaryStarts = calloc(temporaryLength, sizeof(int));
	temporaryEnds = calloc(temporaryLength, sizeof(int));
	int currentLevel = 0;
	Book(startIndex, endIndex,currentLevel, maxLevel, temporaryStarts, temporaryEnds);
	for(int i = temporaryLength - 2; i >= 0; i--)
	{
		//printf("%d %d : %d \n",temporaryStarts[i], temporaryEnds[i], header[i]);
		if(header[i] == 1)
		{
			for(int j = temporaryStarts[i]; j < temporaryEnds[i]; j++)
			{
				fileData[j] = inverseByteLookupTable[fileData[j]];
			}			
		}
	}
	free(temporaryStarts);free(temporaryEnds);
}
/*File IO Start*/
size_t GetFileSize(char *fileName)
{
	FILE *fp = fopen(fileName, "rb");assert(fp != NULL);
	fseek(fp, 0L, SEEK_END);
	size_t currentFileSize = ftell(fp);rewind(fp);
	fclose(fp);
	return currentFileSize;
}

void SortFileNames(char **fileNames, int fileCount)
{
	char temporary[MAX_FILENAME_LENGTH] = {0};
	for(int k = 0; k < fileCount; k++)
	{
		for(int j = k + 1; j < fileCount; j++)
		{
			if(strcasecmp(fileNames[k], fileNames[j]) > 0)
			{
				strcpy(temporary, fileNames[k]);
				strcpy(fileNames[k], fileNames[j]);
				strcpy(fileNames[j], temporary);
				memset(temporary,0,MAX_FILENAME_LENGTH);
			}
		}
	}
}
char *CreateStorageDirectory(char *path)
{
	char outputFolder[MAX_FILENAME_LENGTH] = {0};
	snprintf(outputFolder, sizeof(outputFolder), "%s",path);
	DIR *directory = opendir(outputFolder);
	if(directory == NULL)
	{
		int outputFolderCreationSuccess = mkdir(outputFolder,0777);assert(outputFolderCreationSuccess == 0);
	}
	else
	{
		closedir(directory);
	}
	char *result = calloc(MAX_FILENAME_LENGTH, sizeof(char));
	for(int i = 0; i < MAX_FILENAME_LENGTH; i++)
	{
		result[i] = outputFolder[i];
	}
	return result;
}
int CountNumberOfFilesInDirectory(char *path)
{
	int result = 0;
	DIR *directory = opendir(path);assert(directory != NULL);
	struct dirent *directoryEntry;
	while((directoryEntry = readdir(directory)) != NULL)
	{
		if(directoryEntry->d_name[0] != '.')
		{
			int fileNameLength = (int)sizeof(directoryEntry->d_name);assert(fileNameLength <= MAX_FILENAME_LENGTH);result++;
		}
	}
	closedir(directory);
	return result;
}
char **GetFileNamesInDirectory(char *path, int *fileCount)
{	
	/*Count files and ensure length <= 256*/
	*fileCount = CountNumberOfFilesInDirectory(path);
	/*Array to store file names*/
	char **result = malloc(*fileCount * sizeof(char*));
	for(int i = 0; i < *fileCount; i++)
	{
		result[i] = calloc(MAX_FILENAME_LENGTH, sizeof(char));
	}
	/*Get file names*/
	DIR *directory = opendir(path);assert(directory != NULL);
	struct dirent *directoryEntry;
	int currentIndex = 0;
	while((directoryEntry = readdir(directory)) != NULL)
	{
		if(directoryEntry->d_name[0] != '.')
		{
			assert(strlen(directoryEntry->d_name) < MAX_FILENAME_LENGTH);
			for(int i = 0; i < strlen(directoryEntry->d_name); i++)
			{
				result[currentIndex][i] = directoryEntry->d_name[i];
			}
			currentIndex++;
		}
	}
	SortFileNames(result, *fileCount);
	closedir(directory);
	return result;
}

void PrintFileNames(int fileCount, char **fileNames)
{
	for(int i = 0; i < fileCount; i++)
	{
		printf("%3d %s\n", i, fileNames[i]);
	}
}

void DestroyFileNames(int fileCount, char **fileNames)
{
	for(int i = 0; i < fileCount; i++)
	{
		free(fileNames[i]);
	}
	free(fileNames);
}
/*File IO End*/

/*Decode Single File*/
void DecodeSingleFile(char *inputFileName, char *outputFileName)
{
	ResetGlobalVariables();
	size_t fileSize = GetFileSize(inputFileName);
	FILE *fp = fopen(inputFileName, "rb");assert(fp != NULL);
	fr = fopen(outputFileName, "wb");assert(fr != NULL);
	int fileNumber = fileno(fp);
	unsigned char *fileData = mmap(NULL,fileSize, PROT_READ|PROT_WRITE, MAP_PRIVATE, fileNumber, 0);assert(fileData != NULL);
	unsigned char *originalFile = calloc(fileSize*2, sizeof(char));
	/*Retrieve mod to help with decoding*/
	size_t currentFileIndex = 0;int bit = 0;
	unsigned char byte = fileData[0]; assert(byte != EOF);assert(byte < 100);
	currentFileIndex += 1;
	size_t remainderBytes=byte;
	
	/*Retrieve maxLevel*/
	byte = fileData[1];
	currentFileIndex += 1;
	int maxLevel = byte;
	//assert((size_t) 1 << maxLevel < fileSize);
	assert(maxLevel > -1);assert(maxLevel < 32);

	/*Retrieve Header*/
	header = calloc(1 << maxLevel, sizeof(int));
	assert(header != NULL);
	size_t headerByteCount = (1 << maxLevel) / 8;
	
	for(size_t i = currentFileIndex; i < currentFileIndex + headerByteCount; i++)
	{
		byte = fileData[i];
		for(int j = 7 ; j >= 0; j--)
		{
			bit = (byte >> j) & 1;
			header[headerBitIndex] = bit;
			headerBitIndex += 1;
		}
	}
	currentFileIndex += headerByteCount;

	int currentCount = 1;
	int currentTotal = 2;
	int bitCount = 0;
	size_t byteCount = 0;
	float probability = 0.5f;
	byte = 0;
	//printf("|%u|\n", fileData[currentFileIndex]);
	//Load first 4 bytes
	for(int i = 0 ; i < 4; i++)
	{
		RANGE_CURRENT = RANGE_CURRENT << 8 | fileData[currentFileIndex];
		currentFileIndex += 1;
	}
	
	byteCount = 0;
	while(currentFileIndex < fileSize-1)
	{
		bitCount  = 0;
		byte = 0;
		while(bitCount < 8)
		{
			probability = (float) (currentCount);probability /=  (float) (currentTotal);	
			bit = Decode(probability, (int)fileSize, fileData, &currentFileIndex);
			//printf("%d %d\n", bitCount,  bit);
			bitCount += 1;
			currentCount += bit;
			currentTotal += 1;
			byte <<= 1;
			byte += bit;
		}
		assert(byteCount < fileSize * 2);
		originalFile[byteCount] = byte;
		//printf("(%ld %u)\n", byteCount, byte);
		byteCount += 1;
		//if(byteCount > 10)break;
		//printf("\n");	
	}
	size_t temporaryByteCount = byteCount;
	temporaryByteCount /= 100; temporaryByteCount *= 100;
	temporaryByteCount += remainderBytes; 
	while(byteCount < temporaryByteCount)
	{
		bitCount  = 0;
		byte = 0;
		while(bitCount < 8)
		{
			probability = (float) (currentCount);probability /=  (float) (currentTotal);	
			bit = Decode(probability, (int)fileSize, fileData, &currentFileIndex);
			//printf("%d %d\n", bitCount,  bit);
			bitCount += 1;
			currentCount += bit;
			currentTotal += 1;
			byte <<= 1;
			byte += bit;
		}
		//printf("(%ld %u)\n", byteCount, byte);
		originalFile[byteCount] = byte;
		byteCount += 1;
	}
	//printf("%ld %ld\n", byteCount, temporaryByteCount);
	
	size_t startIndex = 0;size_t endIndex = byteCount;int currentLevel = 0;int totalCount = 0;
	MakeMoreUniform(startIndex, endIndex, byteCount, originalFile, maxLevel);
	
	for(size_t i = 0; i < byteCount; i++)
	{
		byte = originalFile[i];
		fwrite(&byte, 1,1, fr);
	}
	fclose(fp);fclose(fr);
	assert(munmap(fileData, fileSize) != -1);
	free(header);
	free(originalFile);
}

/*Skip encoding hardcoded file*/
void SkipFile(char *inputFileName, char *outputFileName)
{
	ResetGlobalVariables();
	size_t fileSize = GetFileSize(inputFileName);
	FILE *fp = fopen(inputFileName, "rb");assert(fp != NULL);
	int fileNumber = fileno(fp);
	fr = fopen(outputFileName, "wb");assert(fr != NULL);
	unsigned char *fileData = mmap(NULL,fileSize, PROT_READ|PROT_WRITE, MAP_PRIVATE, fileNumber, 0);assert(fileData != NULL);
	fwrite(fileData, 1, fileSize,fr);
	assert(munmap(fileData, fileSize) != -1);
	fclose(fp);fclose(fr);
}

int main()
{
	FillByteLookupTable();
	char *directoryToDecode = "Output";assert(directoryToDecode[0]!= '.');
	char *storageDirectoryTemplate = "DecodedOutput";
	char *storageDirectory = CreateStorageDirectory(storageDirectoryTemplate);
	int directoryFileCount = 0;
	char **toDecodeFileNames = GetFileNamesInDirectory(directoryToDecode, &directoryFileCount);assert(toDecodeFileNames != NULL);
	assert(directoryFileCount > 0);
	//PrintFileNames(directoryFileCount, toDecodeFileNames);
	
	for(int i = 0; i < directoryFileCount; i++)
	//for(int i = 0; i < 1; i++)
	{
		char inputFileName[MAX_FILENAME_LENGTH] = {0};
		char outputFileName[MAX_FILENAME_LENGTH] = {0};
		snprintf(inputFileName, sizeof(inputFileName), "%s/%s",directoryToDecode,toDecodeFileNames[i]);
		snprintf(outputFileName, sizeof(outputFileName), "%s/%s",storageDirectory,toDecodeFileNames[i]);
		
		if(outputFileName[strlen(outputFileName)-3] != 'o' && outputFileName[strlen(outputFileName)-2] != 'u')
		{
			char copyCommand[5000] = {0};
			snprintf(outputFileName, sizeof(outputFileName), "%s/%s",storageDirectory,toEncodeFileNames[i]);
			snprintf(copyCommand, sizeof(copyCommand), "cp %s %s",inputFileName, outputFileName);
			system(copyCommand);
		}
		else
		{
			for(int j = strlen(outputFileName)-4; j < strlen(outputFileName); j++){outputFileName[j] = 0;}
			DecodeSingleFile(inputFileName, outputFileName);
		}

		printf("(%d %d) %s\n",i+1, directoryFileCount, outputFileName);
	}
	
	/*Free memory*/
	DestroyFileNames(directoryFileCount, toDecodeFileNames);
	free(storageDirectory);
	return 0;
}
