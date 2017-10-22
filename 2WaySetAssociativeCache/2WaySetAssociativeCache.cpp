// 2WaySetAssociativeCache.cpp

#include "stdafx.h"

char DRAM[DramSize];
int ProcessCounter = 1;
int DataCounter;
int HitCounter;
int MissCounter;
unsigned short SectorNo;
unsigned short UpperAddr;
unsigned short CpuAddr;
unsigned short DramIndex;
unsigned short Word;

struct cache_line {
	unsigned char TAG;
	bool VALID;
	bool LRU;
	char DATA[CacheLineSize];
}way0[SectorSize], way1[SectorSize];

int main()
{
	/* main memory init*/
	ifstream DataFile;
	ifstream AddrFile;
	/* open text files*/
    DataFile.open("data.txt");
	assert(DataFile.is_open());
	AddrFile.open("CPU_addrs.txt");
	assert(AddrFile.is_open());

	/* reading data from data file to DRAM*/
	while (!DataFile.eof()) {
		DataFile >> DRAM[DataCounter];
		DataCounter++;
	}

	/* cache init*/
	for (int i = 0; i< SectorSize; i++) {
		way0[i].VALID = 0;
		way1[i].VALID = 0;
		way0[i].LRU = 1;
		way1[i].LRU = 0;
	}

	cout << "------------------Reading Process------------------" << endl;

	/* cache reading process*/
	while (!AddrFile.eof()){
		/* reading cpu address from address file*/
		AddrFile >> hex >> CpuAddr;
		/* getting parameters of cache and main memory*/
		SectorNo = (CpuAddr & 0x00ff) >> 2;
		UpperAddr = (CpuAddr & 0xff00) >> 8;
		DramIndex = (CpuAddr & 0xfffc);
		Word = (CpuAddr & 0x0003);

		/* print out reading information*/
		cout << "Reading Process " << dec << ProcessCounter << endl;
		cout << "CPU address: 0x0" << hex << CpuAddr << endl;
		cout << "Main memory location being read: DRAM[" << dec << CpuAddr << "]" << endl;
		cout << "Sector Number: " << SectorNo << endl;

		/* hit and miss*/
		if (way0[SectorNo].TAG == UpperAddr && way0[SectorNo].VALID==1) {
			cout << "hit in cache way0" << endl;
			HitCounter++;
			/* set up cache*/
			way0[SectorNo].LRU = 0;
			way1[SectorNo].LRU = 1;
			/* fetching data from cacheline*/
			cout << "The cache line number being read: " << "line " << dec << Word << endl;
			cout << "Data sent to the CPU for this address: " << way0[SectorNo].DATA[Word] << endl<< endl;

		} else if (way1[SectorNo].TAG == UpperAddr && way1[SectorNo].VALID == 1) {
			cout << "hit in cache way1" << endl;
			HitCounter++;
			/* set up cache*/
			way1[SectorNo].LRU = 0;
			way0[SectorNo].LRU = 1;
			/* fetching data from cacheline*/
			cout << "The cache line number being read: " << "line " << dec << Word << endl;
			cout << "Data sent to the CPU for this address: " << way1[SectorNo].DATA[Word] << endl << endl;

		} else if (way0[SectorNo].LRU == 1) {
			cout << "miss, not in cache" << endl;
			MissCounter++;
			/* set up cache*/
			way0[SectorNo].TAG = UpperAddr;
			way0[SectorNo].VALID = 1;
			way0[SectorNo].LRU = 0;
			way1[SectorNo].LRU = 1;
			/* fetching data from DRAM*/
			for (int i = 0; i < 4; i++) {
				way0[SectorNo].DATA[i] = DRAM[DramIndex + i];
			}
			cout << "Data sent to the CPU for this address: " << DRAM[CpuAddr] << endl <<endl;

		} else if (way1[SectorNo].LRU == 1) { 
			cout << "miss, not in cache" << endl;
			MissCounter++;
			/* set up cache*/
			way1[SectorNo].TAG = UpperAddr;
			way1[SectorNo].VALID = 1;
			way1[SectorNo].LRU = 0;
			way0[SectorNo].LRU = 1;
			/* fetching data form DRAM*/
			for (int i = 0; i < 4; i++) {
				way1[SectorNo].DATA[i] = DRAM[DramIndex + i];
			}
			cout << "Data sent to the CPU for this address: " << DRAM[CpuAddr] << endl << endl;
		}
		ProcessCounter++;
	}

	/* end process*/
	DataFile.close();
	AddrFile.close();

	cout << "Number of Hit: " << HitCounter << endl;
	cout << "Number of Miss: " << MissCounter << endl;
	cout << "--------------------End Reading--------------------" << endl << endl;

	/* print out basic information*/
	cout << "-----------------Basic Information-----------------" << endl;
	cout << "2-Way Set Associative Cache (1 byte words)" << endl;
	cout << "DRAM Size: 65536" << endl;
	cout << "Number of sectors: 64 * 2" << endl;
	cout << "Number of addressable unit per line: 4" << endl;
	cout << "---------------------------------------------------" << endl;

	system("pause");
	return 0;
}

