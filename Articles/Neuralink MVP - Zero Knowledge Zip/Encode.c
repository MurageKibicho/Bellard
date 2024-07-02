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
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#define MAX_FILENAME_LENGTH 256
/*
Run : clear && gcc Encode.c -lm -o m.o && ./m.o

*/

/*Global Variables Start*/

unsigned char oneCountByteLookupTable[256]={0};
unsigned char inverseByteLookupTable[256]={0};

int headerBitIndex = 0;
int *header;
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
}

/*Global Variables End*/
/*Fill Lookup Tables*/
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

/*Arithmetic Coding*/
void EncodeToFile(int bit, float probability, FILE *fr)
{
	assert(probability >= 0.0f);
	assert(probability <= 1.0f);
	assert(bit == 0 || bit == 1);
	if(RANGE_HIGH < RANGE_LOW)
	{
		printf("%d %.3f\n",bit, probability);
	}
	assert(RANGE_HIGH > RANGE_LOW);
	assert(RANGE_LOW >= 0);
	unsigned char byte = 0;
	int intProbability = (int) (probability * 65536.0f);
	uint32_t RANGE_MID = RANGE_LOW +((RANGE_HIGH - RANGE_LOW) >> 16) * intProbability + ((((RANGE_HIGH - RANGE_LOW) & 0xffff) * intProbability) >> 16);
	assert(RANGE_HIGH >= RANGE_MID);
	assert(RANGE_MID >= RANGE_LOW);
	if(bit){RANGE_HIGH = RANGE_MID;}else{RANGE_LOW = RANGE_MID + 1;}
	while((RANGE_HIGH ^ RANGE_LOW) < 0x1000000)
	{
		byte = RANGE_HIGH >> 24;
		fwrite(&byte, sizeof(byte), 1, fr);
        	RANGE_HIGH = RANGE_HIGH <<8 | 255;
        	RANGE_LOW  = RANGE_LOW  << 8;
        }
}

void FlushEncoderToFile(FILE *fr)
{
	unsigned char byte = 0;
	uint32_t RANGE_MID =  RANGE_HIGH;
	for(int i = 3; i >= 0; i--)
	{
		byte = (RANGE_MID >> (8*i)) & 0xff;
		fwrite(&byte, sizeof(byte), 1, fr);
	}
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
/*File IO Ends*/

size_t MakeLessUniform(size_t startIndex, size_t endIndex, size_t fileSize, unsigned char *fileData, int currentLevel, int maxLevel, size_t totalCount)
{
	if(currentLevel == maxLevel){return totalCount;}
	assert(endIndex <= fileSize);assert(startIndex < endIndex);
	size_t midPoint = ((endIndex - startIndex) / 2) + startIndex;
	size_t oneCount = 0;int result = 0;
	for(size_t i = startIndex; i < endIndex; i++){oneCount += oneCountByteLookupTable[fileData[i]];}
	if(oneCount > (endIndex - startIndex) * 4){result = 1;totalCount+=result;}
	if(result == 1)
	{
		for(int i = startIndex; i < endIndex; i++)
		{
			fileData[i] = inverseByteLookupTable[fileData[i]];
		}
	}
	//printf("%ld %ld : %d\n",startIndex, endIndex, result);
	header[headerBitIndex] = result;
	headerBitIndex += 1;
	MakeLessUniform(startIndex, midPoint, fileSize, fileData, currentLevel+1, maxLevel, totalCount);
	MakeLessUniform(midPoint  , endIndex, fileSize, fileData, currentLevel+1, maxLevel, totalCount);
}

float MeasureBinaryUniformity(size_t fileSize, unsigned char *data)
{
	size_t oneCount = 0;
	size_t totalCount = 0;
	for(size_t  i = 0 ; i < fileSize; i++)
	{
		oneCount += oneCountByteLookupTable[data[i]];
		totalCount += 8;
	}
	float uniformity = (float) oneCount;
	return (uniformity / (float) totalCount);
}
int MoveToFront(int character, int alphabetLength, int *alphabet)
{
	int index = 0;
	assert(character > -1);
	assert(character < alphabetLength);
	for(int i = 0 ; i < alphabetLength; i++)
	{
		if(alphabet[i] == character)
		{
			index = i;
			alphabet[i] = 0;
			for(int j = i; j > 0; j--){alphabet[j] = alphabet[j-1];}
			alphabet[0] = character;
			break;
		}
	}
	return index;
}
/*Encode Single File*/
void EncodeSingleFile(char *inputFileName, char *outputFileName)
{
	ResetGlobalVariables();
	size_t fileSize = GetFileSize(inputFileName);
	FILE *fp = fopen(inputFileName, "rb");assert(fp != NULL);
	fr = fopen(outputFileName, "wb");assert(fr != NULL);
	int fileNumber = fileno(fp);
	unsigned char *fileData = mmap(NULL,fileSize, PROT_READ|PROT_WRITE, MAP_PRIVATE, fileNumber, 0);assert(fileData != NULL);
	float oldUniformityMeasure = MeasureBinaryUniformity(fileSize, fileData);
	/*Save mod to help with decoding*/
	unsigned char byte = 0;
	byte = fileSize % 100;
	fwrite(&byte, 1,1,fr);
	
	int maxLevel = 4;
	assert((size_t) 1 << maxLevel < fileSize);assert(maxLevel > -1);assert(maxLevel < 32);	
	byte = maxLevel;
	fwrite(&byte, 1,1,fr);
	
	/*Perform transform*/
	header = calloc(1 << maxLevel, sizeof(int));
	assert(header != NULL);
	size_t startIndex = 0;size_t endIndex = fileSize;int currentLevel = 0;int totalCount = 0;
	MakeLessUniform(startIndex, endIndex, fileSize, fileData, currentLevel, maxLevel, totalCount);

	float newUniformityMeasure = MeasureBinaryUniformity(fileSize, fileData);
	/*Encode Header*/
	byte = 0;
	for(int i = 0 ; i < 1 << maxLevel; i++)
	{
		assert(header[i] > -1);
		assert(header[i] <  2);
		if(i > 0 && i % 8 == 0)
		{
			fwrite(&byte, 1,1,fr);
			byte = 0;
		}
		byte <<= 1;
		byte += header[i];
	}
	fwrite(&byte, 1,1,fr);
	
	/*Encode transformed file*/
	int bit = 0;

	int totalOneCount = 1; 
	int totalBitCount = 2;
	for(size_t i = 0; i < fileSize; i++)
	{
		//printf("(%ld %u)\n", i, fileData[i]);
		for(int j = 0, k =7; j < 8 && k>=0; j++,k--)
		{
			bit = (fileData[i] >> k) & 1;
			float probability = (float) totalOneCount;probability /= (float) totalBitCount;EncodeToFile(bit, probability, fr);totalOneCount += bit;totalBitCount+=1;
			//printf("%ld %d %d\n", i, j,  bit);
		}
		//printf("\n");
		//if(i > 10)break;
	}
	
	FlushEncoderToFile(fr);
	//printf("%.3f %.3f \n", oldUniformityMeasure, newUniformityMeasure);
	fclose(fp);fclose(fr);
	assert(munmap(fileData, fileSize) != -1);
	free(header);
}

/*Skip hardcoded file*/
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

/**/
int main()
{
	clock_t start_clock, end_clock;
	double cpu_time_used;
	FillByteLookupTable();
	char *directoryToEncode = "data";assert(directoryToEncode[0]!= '.');
	char *storageDirectoryTemplate = "Output";
	char *storageDirectory = CreateStorageDirectory(storageDirectoryTemplate);
	int directoryFileCount = 0;
	char **toEncodeFileNames = GetFileNamesInDirectory(directoryToEncode, &directoryFileCount);assert(toEncodeFileNames != NULL);
	assert(directoryFileCount > 0);
	//PrintFileNames(directoryFileCount, toEncodeFileNames);
	const char *knownFailures[8] =
	{
		"data/2458a5be-c274-4fd3-82f6-4617caeaeb14.wav.bz2",
		"data/26b2ca81-4399-4fa9-80e0-7fbb3988d784.wav.bz2",
		"data/3e2b974a-3e13-48f3-b2f7-40a0c18c838f.wav.bz2",
		"data/4f7acf44-98d7-4fcc-9bd3-094d9c30d7dd.wav.bz2",
		"data/685524ad-e854-4fa7-ada7-d8a3861231d1.wav.bz2",
		"data/b37571bb-bc46-436b-b6f5-f579260d73e0.wav.bz2",
		"data/e32ab322-a34e-4348-ac9a-005861378f5d.wav.bz2",
		"data/f1461d76-1c5b-4ceb-998a-c81a74ef0317.wav.bz2" 
	};
	size_t totalInputBytes = 0;
	size_t totalOutputBytes = 0;
	int skippedIndex = 0;
	for(int i = 0; i < directoryFileCount; i++)
	//for(int i = 0; i < 1; i++)
	{
		char inputFileName[MAX_FILENAME_LENGTH] = {0};
		char outputFileName[MAX_FILENAME_LENGTH] = {0};
		
		snprintf(inputFileName, sizeof(inputFileName), "%s/%s",directoryToEncode,toEncodeFileNames[i]);
		start_clock = clock();	
		//if(i == 101 || i == 111 || i == 173 || i == 216 || i == 294 || i == 519 || i == 660 || i == 696)
		if(skippedIndex < 8 && strcmp(inputFileName, knownFailures[skippedIndex]) == 0)
		{
			printf("Skipped : %s\n", toEncodeFileNames[i]);
			char copyCommand[5000] = {0};
			snprintf(outputFileName, sizeof(outputFileName), "%s/%s",storageDirectory,toEncodeFileNames[i]);
			snprintf(copyCommand, sizeof(copyCommand), "cp %s %s",inputFileName, outputFileName);
			system(copyCommand);
			skippedIndex += 1;
		}
		else
		{
			snprintf(outputFileName, sizeof(outputFileName), "%s/%s.out",storageDirectory,toEncodeFileNames[i]);
			EncodeSingleFile(inputFileName, outputFileName);
		}	
		end_clock = clock();
		cpu_time_used = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
		size_t fileSizeInput  = GetFileSize(inputFileName);
		size_t fileSizeOutput = GetFileSize(outputFileName);
		totalInputBytes += fileSizeInput;
		totalOutputBytes += fileSizeOutput;
		printf("\n(%d / %d)%s ", i+1, directoryFileCount, inputFileName);
		printf("\nTransform took %f seconds to execute\n : From : %ld To %ld\n Saved %ld bytes\n", cpu_time_used,fileSizeInput,fileSizeOutput, fileSizeInput-fileSizeOutput);
		
	}
	printf("|---------------|\n");
	printf("Total input size : %10ld (bytes)\n", totalInputBytes);
	printf("Total output size: %10ld (bytes)\n", totalOutputBytes);
	printf("Total saved  size: %10ld (bytes)\n", totalInputBytes - totalOutputBytes);
	/*Free memory*/
	DestroyFileNames(directoryFileCount, toEncodeFileNames);
	free(storageDirectory);
	return 0;
}
