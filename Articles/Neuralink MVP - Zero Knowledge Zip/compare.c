#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>
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

void DecodeBzip()
{
	char *directoryToEncode = "DecodedOutput";assert(directoryToEncode[0]!= '.');
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

void CompareFolders()
{
	char *originalDirectory = "data";
	char *newDirectory = "DecodedOutput";
	
	int originalDirectoryFileCount = 0;
	int newDirectoryFileCount = 0;
	
	char **originalFileNames = GetFileNamesInDirectory(originalDirectory, &originalDirectoryFileCount);assert(originalFileNames != NULL);
	char **newFileNames      = GetFileNamesInDirectory(newDirectory, &newDirectoryFileCount);assert(newFileNames != NULL);
	
	//Ensure similar number of files
	assert(originalDirectoryFileCount > 0);assert(newDirectoryFileCount > 0);
	assert(originalDirectoryFileCount == newDirectoryFileCount);
	printf("\n\n\nTest 1 Passed : Equal number of files\n");
	for(int i = 0 ; i < originalDirectoryFileCount; i++)
	{
		char inputFileName[MAX_FILENAME_LENGTH] = {0};
		char outputFileName[MAX_FILENAME_LENGTH] = {0};
		snprintf(inputFileName, sizeof(inputFileName), "%s/%s",originalDirectory,originalFileNames[i]);
		snprintf(outputFileName, sizeof(outputFileName), "%s/%s",newDirectory,newFileNames[i]);

		FILE *fp = fopen(inputFileName, "rb");assert(fp != NULL);
		FILE *fr = fopen(outputFileName, "rb");assert(fr != NULL);
		
		int fileNumber0 = fileno(fp);
		int fileNumber1 = fileno(fr);
		
		size_t fileSize0 = GetFileSize(inputFileName);
		size_t fileSize1 = GetFileSize(outputFileName);
		
		unsigned char *fileData0 = mmap(NULL,fileSize0, PROT_READ, MAP_PRIVATE, fileNumber0, 0);assert(fileData0 != NULL);
		unsigned char *fileData1 = mmap(NULL,fileSize1, PROT_READ, MAP_PRIVATE, fileNumber1, 0);assert(fileData1 != NULL);
		
		if(fileSize0 != fileSize1)
		{
			printf("%s %ld\n%s %ld", inputFileName,fileSize0, outputFileName, fileSize1);
			break;
		}
		assert(fileSize0 == fileSize1);
		assert(memcmp(fileData0, fileData1,fileSize0) == 0);
		assert(munmap(fileData0, fileSize0) != -1);
		assert(munmap(fileData1, fileSize1) != -1);
		fclose(fp);fclose(fr);
	}
	printf("Test Passed : Equal bytes\n");
	DestroyFileNames(originalDirectoryFileCount, originalFileNames);
	DestroyFileNames(newDirectoryFileCount, newFileNames);
}

int main()
{
	/*Function to compile all Programs*/
	CompareFolders();
	
	return 0;
}
