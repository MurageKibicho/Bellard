#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
int byteEncodeCounter = 0;
int freqs[256]={0};
uint32_t RANGE_LOW = 1;
uint32_t RANGE_HIGH = 0xffffffff;
uint32_t RANGE_CURRENT = 0;
unsigned char *byteHolder;
uint32_t byteHolderIndex = 0;
uint32_t byteHolderLength = 0;
void ResetEncoder()
{
	byteEncodeCounter = 0;
	RANGE_LOW = 1;
	RANGE_HIGH = 0xffffffff;
	RANGE_CURRENT = 0;
}

void ResetEncoderValues(uint32_t startLow, uint32_t startHigh, uint32_t startCurrent, int previousCharacterIndex, int *characterIndex)
{
	*characterIndex = previousCharacterIndex;
	RANGE_LOW = startLow;
	RANGE_HIGH = startHigh;
	RANGE_CURRENT = startCurrent;
}


void Encode(int bit, float probability)
{
	assert(probability >= 0.0f);
	assert(probability <= 1.0f);
	assert(bit == 0 || bit == 1);
	assert(RANGE_HIGH > RANGE_LOW);
	assert(RANGE_LOW >= 0);
	int intProbability = (int) (probability * 65536.0f);
	uint32_t RANGE_MID = RANGE_LOW +((RANGE_HIGH - RANGE_LOW) >> 16) * intProbability + ((((RANGE_HIGH - RANGE_LOW) & 0xffff) * intProbability) >> 16);
	assert(RANGE_HIGH >= RANGE_MID);
	assert(RANGE_MID >= RANGE_LOW);
	if(bit){RANGE_HIGH = RANGE_MID;}else{RANGE_LOW = RANGE_MID + 1;}
	while((RANGE_HIGH ^ RANGE_LOW) < 0x1000000)
	{
		//printf("%d, ", RANGE_HIGH >> 24);
		freqs[RANGE_HIGH >> 24] += 1;
		if(byteHolderIndex < byteHolderLength){byteHolder[byteHolderIndex]=RANGE_HIGH >> 24;}
		byteHolderIndex += 1;
		byteEncodeCounter+=1;
        	RANGE_HIGH = RANGE_HIGH <<8 | 255;
        	RANGE_LOW  = RANGE_LOW  << 8;
        }
}

void FlushEncoder()
{
	uint32_t mid = (RANGE_LOW + RANGE_HIGH) / 2;
	for(int i = 4 ; i > 0; i--)
	{
		
	}
	/*while(((RANGE_LOW ^ RANGE_HIGH) & 0xff000000) ==0)
	{
		//if(byteEncodeCounter < 6000){byteHolder[byteEncodeCounter]=RANGE_HIGH >> 24;}
		byteEncodeCounter+=1;
		RANGE_LOW <<= 8;
		RANGE_HIGH = (RANGE_HIGH << 8) + 255;
	}
	//if(byteEncodeCounter < 6000){byteHolder[byteEncodeCounter]=RANGE_HIGH >> 24;}
	byteEncodeCounter+=1;
	//printf("\n");*/
}


int Decode(float probability, int characterLength, unsigned char *characters, uint32_t *characterIndex)
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

void EncodeToFile(int bit, float probability, FILE *fr)
{
	assert(probability >= 0.0f);
	assert(probability <= 1.0f);
	assert(bit == 0 || bit == 1);
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
		//printf("%d, ", RANGE_HIGH >> 24);
		byte = RANGE_HIGH >> 24;
		fwrite(&byte, sizeof(byte), 1, fr);
		byteHolderIndex += 1;
		byteEncodeCounter+=1;
        	RANGE_HIGH = RANGE_HIGH <<8 | 255;
        	RANGE_LOW  = RANGE_LOW  << 8;
        }
}
void FlushEncoderToFile(FILE *fr)
{
	unsigned char byte = 0;
	/*while(((RANGE_LOW ^ RANGE_HIGH) & 0xff000000) ==0)
	{
		byte = RANGE_HIGH >> 24;
		fwrite(&byte, sizeof(byte), 1, fr);
		byteEncodeCounter+=1;
		RANGE_LOW <<= 8;
		RANGE_HIGH = (RANGE_HIGH << 8) + 255;
	}
	byte = RANGE_LOW >> 24;
	fwrite(&byte, sizeof(byte), 1, fr);
	byteEncodeCounter+=1;*/
	uint32_t RANGE_MID =  RANGE_HIGH;
	for(int i = 3; i >= 0; i--)
	{
		byte = (RANGE_MID >> (8*i)) & 0xff;
		fwrite(&byte, sizeof(byte), 1, fr);
		byteEncodeCounter+=1;
	}
}

