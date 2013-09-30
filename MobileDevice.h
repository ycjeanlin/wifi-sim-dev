#include <vector>
#include <cstdlib>

#define EPSILON1 0.1
#define EPSILON2 0.2
#define Credits 100
#define NUM_OF_INTERESTS 10
#define NUM_OF_USER_INTERESTS 5
#define BUFFER_SIZE 200

using namespace std;
struct Check{
	uint32_t provider;
	int faceValue;
};

class MobileDevice{
	public:
		void updateADCL(int interests[], double pktContactP[]);
		void updateCRCL(uint32_t nodeId, double chkContactP[][2], int ptr);
		int getChkContactP(double chkContactP[][2]);
		void getPktContactP(double pktContactP[]);
		void getNodeInterests(int interests[]);
		void setPktList(const int pktContainer[][5], int ptr);
		void setCheckList(const vector<Check> &chkContainer);
		void recvAdPacket(int pktContainer[][5], vector<Check> &chkContainer);
		int cashChecks(vector<Check> &chkContainer);
		MobileDevice();
		uint32_t nodeId;
		int AdPktContainer[BUFFER_SIZE][5]; 
		int ptrAdContainer;
		vector<Check> checkContainer;
		uint32_t neighborList[7];
		int ptrNeighborList;
	private:
		void setInterests();
		int nodeInterests[NUM_OF_USER_INTERESTS];
		double pktValue[NUM_OF_INTERESTS][4]; //ad contact likelihood
		double checkValue[BUFFER_SIZE][5]; //check reward contact likelihood
		int ptrChkValue;
		int credits;
		int pktList[BUFFER_SIZE][3];
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
	ptrNeighborList = 0;
}

void MobileDevice::setInterests(){
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

void MobileDevice::getNodeInterests(int interests[]){
	for (int i = 0; i < NUM_OF_INTERESTS; i++)
	{
		nodeInterests[i] = interests[i];
	}
}

void MobileDevice::updateADCL(int interests[], double pktContactP[]){
	double directContactP;
	double inDirectContactP;

	for(int i=0;i<NUM_OF_USER_INTERESTS;i++){
		//Calculate the directly contact likelihood
		directContactP = pktValue[interests[i]][0];
		pktValue[interests[i]][0]=(1-EPSILON1)*directContactP+EPSILON1;
	}
		
	for (int i = 0; i < NUM_OF_INTERESTS; i++){
		//Calculate the indirect contact likelihood
		inDirectContactP = pktValue[i][1];
		pktValue[i][1] = (1-EPSILON2)*pktContactP[i]+EPSILON2*inDirectContactP;

		//Update ADCL
		pktValue[i][2] = 1 - (1-pktValue[i][0])*(1-pktValue[i][1]);
	}
}

void MobileDevice::updateCRCL(uint32_t connectNode, double chkContactP[][2], int ptr){
	double directContactP;
	double inDirectContactP;
	bool found = false;	
	int nodeIdx;
	int usedSize;

    //Calculate the directly contact likelihood
    usedSize = ptrChkValue;
	for(int i =0;i<usedSize;i++){
		if(checkValue[i][0] == connectNode){
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
			checkValue[ptrChkValue][0] = connectNode;
			directContactP = 0;
			checkValue[ptrChkValue][1] = (1-EPSILON1)*directContactP+EPSILON1;
			ptrChkValue++;
		}
	}

	//Calculate the indirect contact likelihood
	usedSize = ptrChkValue;
	for(int i=0;i<ptr;i++){
		found = false;
		for (int j = 0; j < usedSize; j++){
			if(checkValue[j][0] == chkContactP[i][0]){
				found = true;
				nodeIdx = j;
				break;
			}
		}

		if(found){
			inDirectContactP = checkValue[nodeIdx][2];
	 		checkValue[nodeIdx][2] = (1-EPSILON2)*chkContactP[i][1]+EPSILON2*inDirectContactP;
		}else{
			if(ptrChkValue <= BUFFER_SIZE){
				checkValue[ptrChkValue][0] = chkContactP[i][0];
				inDirectContactP = 0;
				checkValue[ptrChkValue][2] = (1-EPSILON2)*chkContactP[i][1]+EPSILON2*inDirectContactP;
				ptrChkValue++;
			}
		}
	}

	for(int i=0;i<ptrChkValue;i++){
		checkValue[i][3] = 1-(1-checkValue[i][1])*(1-checkValue[i][2]);
	}
}

int MobileDevice::getChkContactP(double chkContactP[][2]){
	int ptr = 0;

	for (int i = 0; i < ptrChkValue; i++){
		checkValue[i][0] = chkContactP[ptr][0];
		checkValue[i][1] = chkContactP[ptr][1];
		ptr++;
	}

	return ptr;
}

void MobileDevice::getPktContactP(double pktContactP[]){
	int ptr = 0;

	for (int i = 0; i < NUM_OF_INTERESTS; i++){
		pktValue[i][0] = pktContactP[0];
	}
}

void MobileDevice::setPktList(const int pktContainer[][5], int ptr){
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
	int idx = 0;
	for (uint32_t i = 0; i < chkContainer.size(); i++){
		if(chkContainer[i].provider == nodeId){
			chkList[idx]=i;
			idx++;
		}
	}
	ptrChkList = idx;
}

void MobileDevice::recvAdPacket(int pktContainer[][5], vector<Check> &chkContainer){
	Check chk;
	
	for(int i=0;i<ptrPktList;i++){
		for(int j=0;j<NUM_OF_USER_INTERESTS;j++){

			if(nodeInterests[j] == pktList[i][2]){
				//Receiving the AD packet
				if(ptrAdContainer<BUFFER_SIZE){
					pktContainer[pktList[i][0]][4]--;
					for (int k = 0; k < 4; k++){
						AdPktContainer[ptrAdContainer][k] = pktContainer[pktList[i][0]][k];
					}
					AdPktContainer[ptrAdContainer][4] = 0;
					ptrAdContainer++;
					//sign the check
					chk.provider = pktContainer[pktList[i][0]][1];
					chk.faceValue = pktList[i][1];
					chkContainer.push_back(chk);					
				}
			}
		}
	}
	ptrPktList = 0;
}

int MobileDevice::cashChecks(vector<Check> &chkContainer){
	int cashCredits = 0;
	for(int i=0;i<ptrChkList;i++){
		cashCredits =+ chkContainer[chkList[i]].faceValue;
		chkContainer.erase(chkContainer.begin()+chkList[i]);
	}

	return cashCredits;
}


