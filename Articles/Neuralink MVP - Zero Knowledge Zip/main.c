#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#define MAX_FILENAME_LENGTH 2056

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

/*Compress all .wav files in the data folder and store output in the Output folder*/
void EncodeBzip()
{
	char *directoryToEncode = "data";assert(directoryToEncode[0]!= '.');
	int directoryFileCount = 0;
	char **toEncodeFileNames = GetFileNamesInDirectory(directoryToEncode, &directoryFileCount);assert(toEncodeFileNames != NULL);
	for(int i = 0; i < directoryFileCount; i++)
	{
		char inputFileName[MAX_FILENAME_LENGTH] = {0};
		char bzipCommand[MAX_FILENAME_LENGTH] = {0};
		snprintf(bzipCommand, sizeof(bzipCommand), "bzip2 -z --best %s/%s",directoryToEncode,toEncodeFileNames[i]);
		printf("\n%s\n", bzipCommand);
		system(bzipCommand);
	}
	DestroyFileNames(directoryFileCount, toEncodeFileNames);
}

void DecodeBzip(char *directoryToEncode)
{
	assert(directoryToEncode[0]!= '.');
	int directoryFileCount = 0;
	char **toEncodeFileNames = GetFileNamesInDirectory(directoryToEncode, &directoryFileCount);assert(toEncodeFileNames != NULL);
	for(int i = 0; i < directoryFileCount; i++)
	{
		char inputFileName[MAX_FILENAME_LENGTH] = {0};
		char bzipCommand[MAX_FILENAME_LENGTH] = {0};
		snprintf(bzipCommand, sizeof(bzipCommand), "bzip2 -d %s/%s",directoryToEncode,toEncodeFileNames[i]);
		printf("\n%s\n", bzipCommand);
		system(bzipCommand);
	}
	DestroyFileNames(directoryFileCount, toEncodeFileNames);
}


int main()
{
	/*Command to download dataset*/
	char *downloadDataset = "wget https://content.neuralink.com/compression-challenge/data.zip";
	
	/*Command to unzip data*/
	char *unzipDataset= "unzip -q data.zip";
	
	/*Command to compile all Programs*/
	char *compileEncoder = "clear && gcc Encode.c -lm -o Encode.o";
	char *compileDecoder = "clear && gcc Decode.c -lm -o Decode.o";
	char *compileCompare = "gcc compare.c -lm -o compare.o";
	
	char *runEncoder = "clear && ./Encode.o";
	char *runDecoder = "clear && ./Decode.o";
	char *runTest = "clear && ./compare.o";
	
	char *originalDirectory = "data";
	char *newDirectory = "DecodedOutput";
	
	
	system(downloadDataset);
	system(unzipDataset);
	
	system(compileEncoder);
	system(compileDecoder);
	system(compileCompare);
	
	EncodeBzip();
	system(runEncoder);
	//sleep(1);
	system(runDecoder);
	DecodeBzip(newDirectory);


	//Run tests
	DecodeBzip(originalDirectory);
	system(runTest);
	return 0;
}
