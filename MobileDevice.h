#include <Vector>

#define EPSILON1 0.1
#define EPSILON2 0.2
#define Credits 100
#define NUM_OF_INTERESTS 10
#define NUM_OF_USER_INTERESTS 5
#define BUFFER_SIZE 200


struct Check{
	uint32_t provider;
	bool sign;
	uint32_t faceValue;
};

class MobileDevice{
	public:
		void NodeInterets(uint32_t interests[]);
		void updateADCL(uint32_t interests[], const double pktValue[][]);
		void updateCRCL(uint32_t nodeId, const double contactP[]);
		void setPktList(const uint32_t pktContainer[][]);
		void setCheckList(const vector<Check> &chkContainer);
		uint32_t nodeId;
		uint32_t AdPktContainer[BUFFER_SIZE][5]; 
		Vector<Check> checkContainer;	
	private:
		uint32_t nodeInterests[NUM_OF_USER_INTERESTS];
		double pktValue[NUM_OF_INTERESTS][3]; //ad contact likelihood
		double checkValue[BUFFER_SIZE][3]; //check reward contact likelihood
		uint32_t credits;
		uint32_t pktList[BUFFER_SIZE][5]; 

};

void MobileDevice::updateADCL(uint32_t interests[], const double pktValue[][]){
	double directContactP;
	double inDirectContactP;

	for(int i=0;i<NUM_OF_USER_INTERESTS;i++){
		//Calculate the directly contact likelihood
		DirectContactP = this.pktValue[interests[i]][0];
		this.pktValue[interests[i]][0]=(1-EPSILON1)*directContactP+EPSILON1;
	}
		
	for (int i = 0; i < NUM_OF_INTERESTS; i++){
		//Calculate indirectly contact likelihood
		inDirectContactP = this.pktValue[i][1];
		this.pktValue[i][1] = (1-EPSILON2)*pktValue[i][0]+EPSILON2*inDirectContactP;

		//Update ADCL
		this.pktValue[i][2] = 1 - (1-this.pktValue[i][0])*(1-this.pktValue[i][1]);
	}
		

}
