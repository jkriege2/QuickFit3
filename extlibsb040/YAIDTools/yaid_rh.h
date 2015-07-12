#ifndef YAID_RH__H
#define YAID_RH__H
#include <stdio.h>
#include <iostream>
#include <iomanip>
#define __STDC_FORMAT_MACROS
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "crc_ccitt.h"
#include "crc.h"
#include "textcolor.h"
#include "correlator_multitau.h"
#include "helper.h"

//#define DEBUG(s) std::cerr<<__FUNCTION__<<" in " << __FILE__ << " @ " << __LINE__ << ":" << s << std::endl;

#define DEBUG(s)

namespace yaid_rh {
  
  struct frameIdent{
    uint64_t size;
    char* pos;
		unsigned short count;
  };
	
    inline void swap(struct frameIdent *a,struct frameIdent *b){
        uint64_t size=a->size;
		char *pos=a->pos;
		
		a->size=b->size;
		a->pos=b->pos;
		b->size=size;
		b->pos=pos;
	}
  class frameAnalyzer{
	private:
		uint32_t frameSize;
		uint32_t bytesToCheck;
		uint32_t validFrames;
        char *fileBuffer;
        char *fileBufferEnd;
        int64_t fileSize;
        uint64_t maxFrames;
		struct frameIdent *fID;
        void (*callbackProgress)(int,unsigned char*);
        void (*callbackLog)(int,unsigned char*);
	public:
		
		
        frameAnalyzer(const char *filename):frameSize(0),bytesToCheck(1024*1024),validFrames(0),fileBuffer(NULL),maxFrames(0),fID(NULL){
            fileSize=readFileToMem(filename);
			fileBufferEnd=fileBuffer+fileSize-1;
            callbackLog=NULL;
            callbackProgress=NULL;
        }
		
		~frameAnalyzer(){
			if(fileBuffer!=NULL){
				free(fileBuffer);
			}
			if(fID!=NULL){
				free(fID);
			}
		}

		
        uint32_t get_bytesToCheck() { return bytesToCheck; }
        uint32_t get_frameSize() { return frameSize; }

        void set_bytesToCheck(uint32_t d) {bytesToCheck=d;}
        void set_frameSize(uint32_t d) {frameSize=d;}


		
		/**
		* @brief Set callbacks.
		* @param cbProgress Logger function for progress bar.
		* @param cbLog Logger function for text output.
		*
		* Sets up callback functions. Progress ist used to show a progress dialog including some text. Log is used for text output. 
		* Log levels are from error (0) to info (2 or higher). 
		*/
        void callbackSetup(void (*cbLog)(int level, unsigned char* message)=NULL, void (*cbProgress)(int percentage, unsigned char* message)=NULL){
			callbackLog=cbLog;
			callbackProgress=cbProgress;
		}
		
		
		/**
		* @brief Searches for next SOF marker.
		* @param position Position inside fileBuffer to start search.
		* @return position of next marker
		*
		* Searches for a new SOF marker in file starting (0xFF).
		*/
    char *findSOF(char *position){
			DEBUG("position="<<(void*)position);
			char *bufferPos=position,*pos=NULL;
			do{
				if(*((unsigned char*)bufferPos)==0xFFU){
					DEBUG("pos="<<(void*)pos);
					pos=bufferPos;
				}
				bufferPos++;
			DEBUG("bufferPos="<<(void*)bufferPos<<" fBP="<<(void*)fileBufferEnd);
			}while((pos==NULL)&&(bufferPos<=fileBufferEnd));
			DEBUG("pos="<<(void*)pos);
			return pos;
		}

	/**
		* @brief Go through file until CRC16 is valid.
		* @param position Position inside fileBuffer to start search.
		* @return size of frame
		*
		* Iterate through file until a valid CRC16 is found.
		*/
        uint64_t calculateFrameSize(char *position, unsigned int sizeMin=0,unsigned int sizeMax=0) {
									DEBUG("");

			char *bufferPos=position;
            uint64_t pos=0;
            uint64_t frameSize=0;
			uint16_t crc=0xFFFFU;
			do {
				crc=crc_ccitt(crc,(const unsigned char*)bufferPos,1);
				bufferPos++;
				pos++;
				uint16_t crc0=*((uint16_t*)bufferPos);
				uint16_t crc1=crc16::reverse<uint16_t>(crc);
				if(crc0==crc1){
					frameSize=pos+2;
					if(frameSize<sizeMin)frameSize=0;
				}
				if((sizeMax!=0)&&(frameSize>sizeMax)){
					frameSize=0;
					break;
				}
			//printf("%i %4.4x %4.4x %4.4x\n",pos,crc0,crc1,crc);
			}while((frameSize==0)&&(bufferPos<=fileBufferEnd));
			//printf("FRAME found: @%8.8x %i %i %4.4x\n",buffer,frameSize,pos,crc);
			return frameSize;
		}
		
	/**
		* @brief Find frame in file.
		* @param position Position inside file to start search.
		* @param sizeMin Minimum frame size.
		* @param sizeMax Maximum frame size.
		* @return success
		*
		* Tries to find new frame in file.
		*/		
		unsigned int findFrame(const char *position, unsigned int frameSizeMin=0, unsigned int frameSizeMax=0){
			DEBUG("position="<<(void*)position<<" fSMn="<<frameSizeMin<<" fSMx="<<frameSizeMax);
			fID[validFrames].pos=(char*)position;
			fID[validFrames].size=0;
			unsigned int done=0;
			do{
				fID[validFrames].pos=findSOF(fID[validFrames].pos);
				DEBUG("validFrames="<<validFrames<<" fID="<<fID);
				if(fID[validFrames].pos!=NULL){
					fID[validFrames].size=calculateFrameSize(fID[validFrames].pos,frameSizeMin,frameSizeMax);
					if((fID[validFrames].size==0)||(((fID[validFrames].size)<frameSizeMin)||((frameSizeMax!=0)&&((fID[validFrames].size)>frameSizeMax)))){
						fID[validFrames].pos+=1;
					}else{//frame found
						validFrames++;
						return 0;
					}
				}
			}while((done==0)&&(fID[validFrames].pos!=NULL)&&(fID[validFrames].pos<fileBufferEnd));
			return 1;
		}

	/**
		* @brief Find next frame in file.
		* @param position Position inside file to start search.
		* @param sizeMin Minimum frame size.
		* @param sizeMax Maximum frame size.
		* @return success
		*
		* Tries to find a new frame in file, starting after the last frame.
		*/
        uint64_t findNextFrame(unsigned int sizeMin=0, unsigned int sizeMax=0){
				DEBUG("");
            return findFrame(fID[validFrames-1].pos+1,sizeMin,sizeMax);
		}
		
	/**
		* @brief Find next frame in file.
		* @param position Position inside file to start search.
		* @param frameSize Size of frame.
		* @return success
		*
		* Tries to find a new frame in file, starting after the last frame. 
		* Optimized version, taking frame size into account
		*/	
        uint64_t findNextFrameConsecutive(unsigned int frameSizeMin=0, unsigned int frameSizeMax=0){
				DEBUG("");
			if((validFrames>maxFrames)||(validFrames==0))std::cerr<<"ERROR: MEMORY LEAK! validFrames="<<validFrames<<", maxFrames="<<maxFrames<<std::endl;
      fID[validFrames].pos=findSOF(fID[validFrames-1].pos+fID[validFrames-1].size);
      if((frameSize!=0)&&(fID[validFrames].pos==(fID[validFrames-1].pos+fID[validFrames-1].size))){
				//consecutive start marker found
        fID[validFrames].size=calculateFrameSize(fID[validFrames].pos,frameSizeMin,frameSizeMax);
        if((fID[validFrames].size==frameSize)&&(((*((uint32_t*)(fID[validFrames].pos)))-(*((uint32_t*)(fID[validFrames-1].pos))))==0x100)){
					//framesize of new frame correct and consecutive IDs
          if(((fID[validFrames].size)>=frameSizeMin)&&((frameSizeMax==0)||((fID[validFrames].size)<=frameSizeMax))){
						validFrames++;
						return 0;
					}
				}
			}
            uint64_t result=findNextFrame(frameSizeMin,frameSizeMax);
			return result;
		}
		
	/**
		* @brief Read contents of file to buffer.
		* @param buffer target buffer
		* @param filename Filename
		* @return Size of file.
		*
		* Reads contents of file to buffer.
		*/			
    static int64_t readFileToMem(char **buffer, const char *filename){
			DEBUG("");
            FILE *pFile = fopen (filename,"rb");
			if(pFile==NULL){
				std::cerr<<"File Error!"<<std::endl;
				exit(1);
			}
			// obtain file size:
			fseek (pFile , 0 , SEEK_END);
            int64_t lSize = ftell(pFile);
			rewind (pFile);
			// allocate memory to contain the whole file:
			*buffer=(char*)malloc(sizeof(char)*lSize);
			if(*buffer==NULL){
				std::cerr<<"Memory Error!"<<std::endl;
				exit(2);
			}
			// copy the file into the buffer:
			size_t result=fread(*buffer,sizeof(char),lSize,pFile);
            if (int64_t(result)!=lSize){
				std::cerr<<"Reading Error!"<<std::endl;
				exit (3);
			}
			fclose (pFile);
			return lSize;
		}
		
        int64_t readFileToMem(const char *filename){
			DEBUG("");
			return readFileToMem(&(this->fileBuffer),filename);
		}
		
	/**
		* @brief Sort frames by size.
		*
		* Sort frames by size with bubble sort
		*/			
	
		void sortFrames(){
            uint64_t sorted;
            uint64_t n=validFrames-1;
			do{
				sorted=0;
                for(uint64_t i=0;i<n;i++){
					if(fID[i].size>(fID[i+1]).size){
						swap(&fID[i],&(fID[i+1]));
						sorted++;
					}
				}
				n--;
			}while((sorted!=0)&&(n>0));
		}
		
	/**
		* @brief Find frames in file.
		* @param filename Filename
		* @return Size of file.
		*
		* Reads contents of file to buffer.
		*/		
        void findFrames(unsigned int pass, uint64_t maxFramesCount=1024*1024,int (*callback)(int)=NULL, unsigned int verbose=0){
			DEBUG("");
			do{
				if(verbose==1){
                    std::cout<<"PASS "<<pass<<std::endl;
				}
				validFrames=0;
				
				if(fID!=NULL){
					free(fID);
				}
				fID=(struct frameIdent *) calloc(maxFramesCount,sizeof(struct frameIdent));
				maxFrames=maxFramesCount;
				
				if(findFrame(fileBuffer,frameSize,frameSize)==0){
					fID[0].count=1;
					
					int percent=-1,percentTmp;
					do{
						percentTmp=((fID[validFrames-1]).pos-fileBuffer)*100/(fileBufferEnd-fileBuffer);
						if(percent!=percentTmp){
							percent=percentTmp;
							if(callback!=0)callback(percent);
						}
						if(findNextFrameConsecutive(frameSize,frameSize)==0){
							fID[validFrames-1].count=1;
						}else{
							//No frame found
							break;
						}
                        if((pass==0)&&(((fID[validFrames-1]).pos-fileBuffer)>bytesToCheck))break;
					}while(((fID[validFrames-1]).size!=0)&&((fID[validFrames-1]).pos!=NULL));
					//validFrames-=1;
					if(callback!=0)callback(100);
					if(verbose==1){std::cout<<validFrames<<" frames found"<<std::endl;}

					if(verbose==2){
						uint64_t lastPos=(uint64_t)(fID[0]).pos-(fID[0]).size;
                        uint32_t frameIDExpected=0;
                        uint64_t frameCount=0;

                        for(uint64_t i=0;i<validFrames;i++){
							if(fID[i].size==frameSize){
								uint32_t frameID=(*((uint32_t*)(fID[i].pos)))>>8;
								uint32_t frameRH=(*((uint32_t*)(fID[i].pos+4)));
								if(frameID!=frameIDExpected){
									printf("FRAMES MISSING: %8.8x-%8.8x (%i)\n",frameIDExpected,frameID-1,frameID-frameIDExpected);
								}
                                printf("(%8llu) %8.8x %8.8x s=%8.8llx d=%16.16llx\n",frameCount,frameID,frameRH,fID[i].size,((uint64_t)fID[i].pos)-lastPos);
								lastPos=(uint64_t)fID[i].pos;
								frameIDExpected=frameID+1;
								frameCount++;
							}
						}
					}
					if(verbose==1){std::cout<<"Sorting frames... "<<std::flush;}
					sortFrames();
					if(verbose==1){std::cout<<"[ OK ]"<<std::endl;}

					if(verbose==1){std::cout<<"Searching for consequtive frames... "<<std::flush;}
                    for(uint64_t i=0;i<validFrames;i++){
						if(fID[i].size==(fID[i+1]).size){
							if((fID[i].pos+fID[i].size)==(fID[i+1]).pos){
								if((frameSize!=0)&&(fID[i].size!=0)&&(frameSize!=fID[i].size)){
                                    fprintf(stderr,"File with different frame sizes is not supported yet.\nProbably a very unlikely case has happend.\nAquire new data and chek those! ;-)\n%8.8x %8.8llx\n",frameSize,fID[i].size);
									exit(1);
								}
								frameSize=fID[i].size;
							}
                            if((fID[i].pos+fID[i].size)==(fileBufferEnd+1)){
								printf("<FUN>\n");
							}
						}
					}
					if(verbose==1){std::cout<<"[ OK ]"<<std::endl;}
					if(verbose==1){std::cout<<"frame size: "<<frameSize<<std::endl;}
					pass++;
					
					if(verbose==2){
						printf("These are the frames:\n");
                        uint32_t frameIDExpected=0;
                        uint64_t frameCount=0;
                        for(uint64_t i=0;i<validFrames;i++){
							if(fID[i].size==frameSize){
								uint32_t frameID=(*((uint32_t*)(fID[i].pos)))>>8;
								uint32_t frameRH=(*((uint32_t*)(fID[i].pos+4)));
								if(frameID!=frameIDExpected){
									printf("FRAMES MISSING: %8.8x-%8.8x (%i)\n",frameIDExpected,frameID-1,frameID-frameIDExpected);
								}
                                printf("(%8llu) %8.8x %8.8x\n",frameCount,frameID,frameRH);
								frameIDExpected=frameID+1;
								frameCount++;
							}
						}
					}
				}
			}while(pass<=1);
		}

	/**
		* @brief Find ghost frames.
		* @param filename Filename
		* @return Size of file.
		*
		* Tries to remove ghost frames. Ghost frames are randomly occuring valid frames with the correct length that overlap
		* valid frames with consecutive IDs.
		*/		
		void findGhostFrames(unsigned int verbose=0){
			if(verbose==1)std::cout << "Checking for CRC16 ghost frames (only single occurrences, still very rare!)..." << std::endl;
            uint64_t frameCount=0;
            uint32_t frameIDExpected=0;
            for(uint64_t i=0;i<validFrames;i++){
				if((fID[i]).size==frameSize){
					uint32_t frameID=(*((uint32_t*)((fID[i]).pos)))>>8;
					uint32_t frameRH=(*((uint32_t*)((fID[i]).pos+4)));
                    if(verbose==2)printf("(%8llu) <%8llu> %8.8u %8.8x s=%8.8llx d=%16.16llx\n",frameCount,i,frameID,frameRH,(fID[i]).size,((uint64_t)(fID[i+1]).pos)-((uint64_t)(fID[i]).pos));
					if(frameID!=frameIDExpected){
						std::cout<<
							color(RESET,YELLOW,BLACK)<<
							"FRAMES MISSING: "<< std::hex << frameIDExpected << "-" << frameID-1 << std::dec << " (" << (int)(frameID)-(int)frameIDExpected << ")" << 
							color(RESET,WHITE,BLACK) << std::endl;
                        if(i>0){
                            if((i>0)&&(frameID>((*((uint32_t*)((fID[i-1]).pos)))>>8))&&(frameID<((*((uint32_t*)((fID[i+1]).pos)))>>8))){
                                std::cout<<"Everything looks fine, just some frames missing."<<std::endl;
                            }else{
                                std::cout<<
                                    color(BRIGHT,RED,BLACK)<<
                                    "ghost frame detected, invalidating it..."<<std::endl<<
                                    color(RESET,RED,BLACK)<<
                                    "Keep in mind, that rescanning will unveil the same error again!"<<std::endl<<
                                    "Normally you can ignore the following warning."<<
                                    color(RESET,WHITE,BLACK)<<std::endl;
                                (fID[i]).size=0;
                            }
                        }
					}
					frameIDExpected=frameID+1;
					frameCount++;
				}
			}
		}

	/**
		* @brief Write frames to file.
		* @param filename Filename
		*
		* Writes all frames found to file. Only frames with correct size are considered.
		*/		
        void writeFileFromMem(const char* filename, unsigned int verbose=0){
            FILE *pFile = fopen (filename,"wb");
			if(verbose==1)std::cout<<"Writing frames to "<<filename<<std::endl;
			if (pFile==NULL){
				std::cerr<<"File Error"<<std::endl;
				exit(1);
			}
            uint64_t z=0;
            for(uint64_t i=0;i<validFrames;i++){
				if((fID[i]).size==frameSize){
					fwrite((fID[i]).pos,sizeof(char),(fID[i]).size,pFile);
					z++;
				}
			}
			if(verbose==1)std::cout<<z<<" frames written."<<std::endl;
			fclose(pFile);
		}

	/**
		* @brief Figure out number of steps. 
		* @return number of steps
		*
		* Returns the the value of the second word of the lasrt frame with the correct size.
		* This equals the number of steps for a RH2-COR file.
		*/		
        uint64_t getSteps(){
            uint64_t result=0;
            for(uint64_t i=0;i<validFrames;i++){
				if((fID[i]).size==frameSize){
                    result=*((uint32_t*) (fID[i].pos+4));
				}
			}
			return result*1024;
		}
    };

struct corData
{
	uint64_t tau;
	uint64_t raw;
	double val;
};

struct correlationInfoBlock{
    uint64_t width;
    uint64_t height;
    uint64_t steps;
    uint64_t blocks;
    uint64_t lags;
	bool isNewFileFormat;
	bool doCrosscorrelation;
	bool ignoreErrors;
};

	class corFileReader{
	private:
        uint64_t width;
        uint64_t height;
        uint64_t steps;
        uint64_t blocks;
        uint64_t lags;
		bool isNewFileFormat;
		bool doCrosscorrelation;
		bool ignoreErrors;
		FILE *filePointer;
		corData **corValues;
        uint64_t *global;
        uint64_t *local;
	public:
        corFileReader(const char* filename,uint64_t width, uint64_t height, uint64_t steps):
			width(width),
			height(height),
			steps(steps),
            blocks(14),
			lags(8),
			isNewFileFormat(false),
			doCrosscorrelation(false),
			ignoreErrors(false),
			filePointer(NULL),
            corValues(NULL){
			
            filePointer=fopen(filename,"rb");
            corValues = (corData**)malloc(width*height*sizeof(struct corData*));
            for(uint64_t i=0;i<width*height;i++){
                corValues[i]=(corData*)malloc((blocks*lags)*sizeof(struct corData));
            }
            global = (uint64_t*)malloc(width*height*sizeof(uint64_t));
            local =  (uint64_t*)malloc(width*height*sizeof(uint64_t));
        }
		
		~corFileReader(){
            if(corValues!=NULL){
                for(uint64_t i=0;i<width*height;i++)
                    free(corValues[i]);
                free(corValues);
			}
			
			fclose(filePointer);
		}
		
        inline double getVal(uint64_t index,uint64_t lag){return corValues[index][lag].val;}
        inline uint64_t getRaw(uint64_t index,uint64_t lag){return corValues[index][lag].raw;}
        inline uint64_t getTau(uint64_t index,uint64_t lag){return corValues[index][lag].tau;}
		

		void setIsNewFileFormat(bool value=false){isNewFileFormat=value;}
		void setDoCrosscorrelation(bool value=false){doCrosscorrelation=value;}
		void setIgnoreErrors(bool value=false){ignoreErrors=value;}
        uint64_t getTotalLagCount(){return blocks*lags;}
        corData** getCorData(){return corValues;}
        uint64_t getGlobal(unsigned int i){return global[i];}
        uint64_t getlocal(unsigned int i){return global[i];}



		/**
		* @brief Reads and normalizes HW correlator dump
		* @param file filename of dump
		* @param retData results are stored here
		* @param info correlation information block
		* @param verbose verbosity of operation, currently not used
		* @return nothing
		*
		* This function reads raw correlation data from FPGA2 on RH2 board. 
		* The file has to be aligned correctly and must be error free, use frameAnalyzer to do this.
		* When an overflow is detected, values are corrected accordingly. 
		* All values are finally normalized, see paper for details. 
		* TODO: Use normalization function from correlator_multitau.h.
		*/
        void processFrames(unsigned int /*verbose*/=0){
			std::cerr << "Normalizing correlator dump data..." << std::endl;
            uint64_t hwc_line_size=2*4+32*2*128*4+2;
			unsigned char *hwc_line=(unsigned char*)malloc(hwc_line_size);
            int64_t len;
			char *map=(char*)calloc(height,sizeof(char));
			unsigned char *used_map=(unsigned char*)calloc(128,1);
            uint64_t idLast=steps/1024;
			unsigned char *idLastMap=(unsigned char*)calloc(height,1);
			
			uint64_t **hwc_dump = (uint64_t **)malloc((height)*32 * sizeof(uint64_t*));
            for(uint64_t i=0;i<(height)*32;i++){
				hwc_dump[i]=(uint64_t*)calloc(128,sizeof(uint64_t)*2);
			}
            uint64_t idOld=0;
			do{
				len=fread(hwc_line,hwc_line_size,1,filePointer);
				if((((uint32_t*)hwc_line)[0]&0xFFU)!=0xFFU){
					std::cerr << color(BRIGHT,RED,BLACK) << "SOF not found!" << color(RESET,WHITE,BLACK) << std::endl;
				}
				uint32_t id=((uint32_t*)hwc_line)[0]>>8;
				id&=0x00FFFFFFU;
				if(id<idOld){
					std::cerr << color(BRIGHT,RED,BLACK) << 
						"ID mismatch! Probably your correlator data file is not sorted correctly." << std::endl << 
						"Skipping one frame. Data integrity might be lost!" << std::endl <<
						"Frame id=0x" << std::hex << id << ", id(old)=0x" << idOld << std::dec << color(RESET,WHITE,BLACK) << std::endl;
					continue;
				}
				uint32_t *src=(uint32_t*)&hwc_line[8];
				uint32_t ticks=*((uint32_t*)&hwc_line[4]);
				fprintf(stderr,"(%8.8x)\n",ticks);
				if(idLast==((uint32_t*)hwc_line)[1]){
					//std::cerr << id << " " << id%(height) << ":" << ((uint32_t*)hwc_line)[1] << std::endl;
					idLastMap[id%(height)]=1;
				}
				for(int x=0;x<32;x++){
					uint32_t *dst=(uint32_t*)hwc_dump[(id%(height))*32+x];
					for(int i=0;i<128;i++){
						if(src[i]!=0){
							used_map[i]=1;
						}
					}
					for(int i=0;i<128;i++){
						if(((id%height)==22)&&(x==0)&&(i==0)){
							std::cerr << *src << " (0x" << std::hex << id << std::dec << ")" << std::endl;
						}
						if((*dst>*src)&&(i<112)){
							std::cerr << color(RESET,YELLOW,BLACK);
							std::cerr << "Possible overflow detected @ (" << id%height << "," << x << ")." << i << " d=" << *dst << "> s=" << *src << " (" << std::hex << ((id<<8) | 0xff) << std::dec << ")";
							std::cerr << color(RESET,WHITE,BLACK) << std::endl;
							dst[1]++;
						}
						*dst=*src;
						dst+=2;
						src++;
					}
					src+=128; //skip delay registers
				}
				idOld=id;
			}while(len==1);
			
			for(int i=0;i<128;i++){
				if(used_map[i]==0){
					std::cerr << " .";
				}else{
					std::cerr << " x";
				}
			}
			std::cerr << std::endl;
			for(int i=0;i<128;i++){
						std::cerr << std::hex << std::setw(2) << i;
			}
			std::cerr << std::dec << std::endl;
			
            for(uint64_t correlator=0;correlator<(height)*32;correlator++){
				uint64_t *hwc_value=hwc_dump[correlator];
				unsigned int tau=0;
				uint64_t count_global=hwc_dump[correlator][126];
				uint64_t count_local=hwc_dump[correlator][127];
                global[correlator]=count_global;
                local[correlator]=count_local;
                for(unsigned int block=0;block<blocks;block++){
					for(unsigned int lag=0;lag<lags;lag++){
						uint64_t dword=*hwc_value;
						hwc_value++;
						uint64_t value=dword;
						//G, taken from correlator_multitau.h, should use buitl-in normalization function
						double result=1.0;
						if(steps>tau){
							double correction_factor=(double(steps-tau))/(double)steps;
							result=((double)value*(double)steps)/((double)(1<<block)*correction_factor*(double)(count_global*count_local));
						}
                        corValues[correlator][block*lags+lag].tau=tau*10;//*10E-6s
                        corValues[correlator][block*lags+lag].raw=value;
                        corValues[correlator][block*lags+lag].val=result;
						tau+=(1<<block);
					}
				}
			}
			
			for(unsigned int i=0;i<height;i++){
				if(idLastMap[i]==0){
					textcolor(stderr,BRIGHT,RED,BLACK);
					fprintf(stderr,"Missing HW correlator result: %i!\n",i);
				}
			}
			textcolor(stderr,RESET,WHITE,BLACK);
		}
    };

    /**
    * @brief reads and correlates raw data
    * @param file filename of dump
    * @param retData results are stored here
    * @param info correlation information block
    * @param verbose verbosity of operation, currently not used
    * @return nothing
    *
    */
    inline void readRawFile(char *file, struct corData **retData, struct correlationInfoBlock *info, unsigned int /*verbose*/=0) {
        std::cerr << "Correlating raw data... (" << file << ")" << std::endl;
        uint64_t *ticks=(uint64_t *)calloc(1024,sizeof(uint64_t));
        FILE *fp=fopen(file,"rb");
        uint64_t frameSize=4+(info->height)*4+2;
        if(info->isNewFileFormat==true)frameSize+=4;
        std::cerr << color(BRIGHT,YELLOW,BLUE) << "Frame Size: "<< frameSize << color(RESET,WHITE,BLACK) << std::endl;
        uint32_t *buffer=(uint32_t*)calloc(frameSize,1);
        correlatorjb<uint64_t, double> * correlator[info->height*32];
        for(uint64_t i=0;i<info->height*32;i++){
            correlator[i]=new correlatorjb<uint64_t, double>(info->blocks,info->lags,uint64_t(0));
        }
        uint32_t dword=0;
        for(uint64_t i=0;i<info->steps;i++)
        {
            if(i%(info->steps/20)==99){
                std::cerr << (i*100/info->steps) << "% ";
            }
            uint64_t len=fread((char*)buffer,1,frameSize,fp);
            if(len!=frameSize)
            {
                std::cerr << std::endl << "ERROR: Framesize mismatch! " <<  len << std::endl;
                break;
            }
            if((buffer[0]>>8)!=i){
                fprintf(stderr,"ERROR: Missing raw data frame (%i,%llu/%llu)!\n",(buffer[0]>>8),i,info->steps);
                if(info->ignoreErrors==false){
                    exit(1);
                }
            }
            for(uint64_t y=0;y<info->height;y++)
            {
                dword=buffer[y+1];
                if(info->isNewFileFormat==true){
                    dword=buffer[y+2];
                }
                uint32_t value_old=0;
                for(int64_t x=0;x<32;x++)
                {
                    uint32_t value=dword&1;
                    if((x<31)||(info->doCrosscorrelation==false)){
                        correlator[y*32+x]->run(value,value);//auto-correlation
                        ticks[y*32+x]+=value;
                    }else{
                        correlator[y*32+x]->run(value,value_old);//cross-correlation
                        ticks[y*32+x]+=value*value_old;
                    }
                    value_old=value;
                    dword = dword >> 1;
                }
            }
        }
        fclose(fp);
        std::cerr << "100%" <<std::endl;


        for(uint64_t i=0;i<info->height*32;i++)
        {
            uint64_t **array=correlator[i]->get_array();
            double **array2=correlator[i]->get_array_G();
            for(uint64_t x=0;x<(info->blocks*info->lags);x++)
            {
                retData[i][x].tau=array[0][x]*10; //*10E-6s
                retData[i][x].raw=array[1][x];
                retData[i][x].val=array2[1][x];
                //std::cerr << correlator_sw_values[i][x].tau << " " << correlator_sw_values[i][x].raw << " " << correlator_sw_values[i][x].val << std::endl;
            }
        }
    }
    
    /**
    * @brief reads and correlates raw data (test version)
    * @param file filename of dump
    * @param retData results are stored here
    * @param info correlation information block
    * @param verbose verbosity of operation, currently not used
    * @return nothing
    *
		* Correlates raw data from file. Used for testing purpose only.
		* You should invoke this function when you know what you're doing only!
    */
    inline static void readRawFile2(char *file, struct corData **retData, struct correlationInfoBlock *info, unsigned int /*verbose*/=0){
        std::cerr << "Correlating raw data... (" << file << ")" << std::endl;
        //uint64_t *ticks=(uint64_t *)calloc(1024,sizeof(uint64_t));
        FILE *fp=fopen(file,"rb");
        unsigned int frameSize=4+(info->height)*4+2;
        if(info->isNewFileFormat==true)frameSize+=4;
        std::cerr << color(BRIGHT,YELLOW,BLUE) << "Frame Size: "<< frameSize << color(RESET,WHITE,BLACK) << std::endl;
        uint32_t *buffer=(uint32_t*)calloc(frameSize,1);
        correlatorjb<uint64_t, double> * correlator[info->height*32];
        for(uint64_t i=0;i<info->height*32;i++){
            correlator[i]=new correlatorjb<uint64_t, double>(info->blocks,info->lags,uint64_t(0));
						correlator[i]->set_acc_cnt_max(3);
				}
        uint32_t dword=0;
                //uint16_t *line=(uint16_t*)calloc(sizeof(uint16_t),info->width);
                //uint16_t * frameAccumulator=(uint16_t*)calloc(sizeof(uint16_t),info->width*info->height);
        for(unsigned int i=0;i<info->steps*3;i++)
        {
            if(i%((info->steps*3)/20)==99){
                std::cerr << (i*100/(info->steps*3)) << "% ";
            }
            unsigned int len=fread((char*)buffer,1,frameSize,fp);
            if(len!=frameSize)
            {
                std::cerr << std::endl << "ERROR: Framesize mismatch! " <<  len << std::endl;
                break;
            }
            if((buffer[0]>>8)!=i){
                fprintf(stderr,"ERROR: Missing raw data frame (%i,%i/%llu)!\n",(buffer[0]>>8),i,info->steps);
                if(info->ignoreErrors==false){
                    exit(1);
                }
            }
            for(uint64_t y=0;y<info->height;y++)
            {
                dword=buffer[y+1];
                if(info->isNewFileFormat==true){
                    dword=buffer[y+2];
                }
                for(int64_t x=0;x<32;x++)
                {
                    uint8_t value=dword&1;
										correlator[y*32+x]->run_acc(value,value);
                    dword = dword >> 1;
                }
            }
        }
        fclose(fp);
        std::cerr << "100%" <<std::endl;


        for(uint64_t i=0;i<info->height*32;i++)
        {
            uint64_t **array=correlator[i]->get_array();
            double **array2=correlator[i]->get_array_G();
            for(uint64_t x=0;x<(info->blocks*info->lags);x++)
            {
                retData[i][x].tau=array[0][x]*10; //*10E-6s
                retData[i][x].raw=array[1][x];
                retData[i][x].val=array2[1][x];
                //std::cerr << correlator_sw_values[i][x].tau << " " << correlator_sw_values[i][x].raw << " " << correlator_sw_values[i][x].val << std::endl;
            }
        }
    }


}
#endif
