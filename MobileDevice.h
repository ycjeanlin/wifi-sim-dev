#include <Vector>
#include <cstdlib>

#define EPSILON1 0.1
#define EPSILON2 0.2
#define Credits 100
#define NUM_OF_INTERESTS 10
#define NUM_OF_USER_INTERESTS 5
#define BUFFER_SIZE 200


struct Check{
	uint32_t provider;
	uint32_t faceValue;
};

class MobileDevice{
	public:
		void updateADCL(uint32_t interests[], double pktValtemp[][]);
		void updateCRCL(uint32_t nodeId, double contactP[], int ptr);
		int getCheckContactP(double contactP[][]);
		void setPktList(const uint32_t pktContainer[][], int ptr);
		void setCheckList(const vector<Check> &chkContainer);
		void recvAdPacket(uint32_t pktContainer[][], vector<Check> &chkContainer);
		uint32_t cashChecks(vector<Check> &chkContainer);
		MobileDevice();
		uint32_t nodeId;
		uint32_t AdPktContainer[BUFFER_SIZE][5]; 
		int ptrAdContainer;
		vector<Check> checkContainer;	
	private:
		void setInterests();
		int nodeInterests[NUM_OF_USER_INTERESTS];
		double pktValue[NUM_OF_INTERESTS][4]; //ad contact likelihood
		double checkValue[BUFFER_SIZE][5]; //check reward contact likelihood
		int ptrChkValue;
		uint32_t credits;
		uint32_t pktList[BUFFER_SIZE][3];
		int ptrPktList;
		int chkList[BUFFER_SIZE]; //for cash the checks of the node
		int ptrChkList;
};

MobileDevice::MobileDevice(){
	credits = 100;
	setInterests();
	ptrAdContainer = 0;
	ptrChkValue = 0;
	ptrChkList = 0;
	ptrPktList = 0;
}

void setInterests(){
	int num;
	int i=0;
	srand(time(NULL));

	num = rand()%10;
	nodeInterests[i] = num;
	i++;
	while(i<=5){
		bool contained = false;

		num=rand()%10;
		for(int j=0;j<i;j++){
			if(num==nodeInterests[j]){
				contained = true;
			}
		}

		if(!contained ){
			nodeInterests[i]=num;
			i++;
		}
	}

}

void MobileDevice::updateADCL(uint32_t interests[], double pktValtemp[][]){
	double directContactP;
	double inDirectContactP;

	for(int i=0;i<NUM_OF_USER_INTERESTS;i++){
		//Calculate the directly contact likelihood
		DirectContactP = pktValue[interests[i]][0];
		pktValue[interests[i]][0]=(1-EPSILON1)*directContactP+EPSILON1;
	}
		
	for (int i = 0; i < NUM_OF_INTERESTS; i++){
		//Calculate the indirect contact likelihood
		inDirectContactP = pktValue[i][1];
		pktValue[i][1] = (1-EPSILON2)*pktValtemp[i][0]+EPSILON2*inDirectContactP;

		//Update ADCL
		pktValue[i][2] = 1 - (1-pktValue[i][0])*(1-pktValue[i][1]);
	}
}

void MobileDevice::updateCRCL(uint32_t nodeId, double contactP[][], int ptr){
	double directContactP;
	double inDirectContactP;
	bool found = false;	
	int nodeIdx;

    //Calculate the directly contact likelihood
    usedSize = ptrChkValue;
	for(int i =0;i<usedSize;i++){
		if(checkValue[i][0] == nodeId){
			found = true;
			nodeIdx=i;		
			break;
		}
	}

	if(found){
		directContactP = checkValue[nodeIdx][1];
		checkValue[nodeIdx][1] = (1-EPSILON1)*directContactP+EPSILON1;
	}else{
		if(ptrChkValue <= BUFFER_SIZE){
			chkValue[ptrChkValue][0] = nodeId;
			directContactP = checkValue[ptrChkValue][1];
			checkValue[ptrChkValue][1] = (1-EPSILON1)*directContactP+EPSILON1;
			ptrChkValue++;
		}
	}

	//Calculate the indirect contact likelihood
	usedSize = ptrChkValue;
	for(int i=0;i<ptr;i++){
		found = false;
		for (int j = 0; j < usedSize; j++){
			if(checkValue[j][0] == contactP[i][0]){
				found = true;
				nodeIdx = j;
				break;
			}
		}

		if(found){
			inDirectContactP = checkValue[nodeIdx][2];
	 		checkValue[nodeIdx][2] = (1-EPSILON2)*contactP[i][1]+EPSILON2*inDirectContactP;
		}else{
			if(ptrChkValue <= BUFFER_SIZE){
				chkValue[ptrChkValue][0] = i;
				inDirectContactP = 0;
				checkValue[ptrChkValue][2] = (1-EPSILON2)*contactP[i][1]+EPSILON2*inDirectContactP;
				ptrChkValue++;
			}
		}
	}

	for(int i=0;i<ptrChkValue;i++){
		checkValue[i][3] = 1-(1-checkValue[i][1])*(1-checkValue[i][2]);
	}
}

int MobileDevice::getCheckContactP(double contactP[][]){
	int ptr = 0;

	for (int i = 0; i < ptrChkValue; i++){
		checkValue[i][0] = contactP[ptr][0];
		checkValue[i][1] = contactP[ptr][1];
		ptr++;
	}

	return ptr;
}

void MobileDevice::setPktList(const uint32_t pktContainer[][], int ptr){
	bool found;
	int idx = 0;

	for (int i = 0; i < ptr; i++){
		if(pktContainer[i][4] != 0){
			found = false;
			for(int j = 0;j<ptrAdContainer;j++){
				if(AdPktContainer[j][0] == pktContainer[i][0]){
					found = true;
					break;
				}
			}

			if(!found){
				pktList[idx][0]=i;
				pktList[idx][1]=pktContainer[i][2];
				pktList[idx][2]=pktContainer[i][3];
				idx++;
			}
		}
	}
}

void MobileDevice::setCheckList(const vector<Check> &chkContainer){
	int idx;
	for (int i = 0; i < chkContainer.size(); i++){
		if(chkContainer[i].provider == nodeId){
			chkList[idx]=i;
			idx++
		}
	}
	ptrChkList = idx;
}

void MobileDevice::recvAdPacket(uint32_t pktContainer[][], vector<Check> &chkContainer){
	for(int i=0;i<ptrPktList;i++){
		for(int j=0;j<NUM_OF_USER_INTERESTS;j++){

			if(nodeInterests[j] == pktList[i][2]){
				//Receiving the AD packet
				if(ptrAdContainer<BUFFER_SIZE){
					pktContainer[pktList[i][0]][4]--;
					for (int k = 0; k < 4; k++){
						AdPktContainer[ptrAdContainer][k] = pktContainer[pktList[i][0]][k];
					}
					AdPktContainer[ptrAdContainer][k] = 0;
					ptrAdContainer++;
					//sign the check
					chkContainer.push_back(Check(pktContainer[pktList[i][0]][1], pktList[i][1]));					
				}
			}
		}
	}
	ptrPktList = 0;
}

uint32_t cashChecks(vector<Check> &chkContainer){
	uint32_t cashCredits = 0;
	for(int i=0;i<ptrChkList;i++){
		cashCredits =+ chkContainer[chkList[i]].faceValue;
		chkContainer.erase(chkList[i]);
	}

	return cashCredits;
}


