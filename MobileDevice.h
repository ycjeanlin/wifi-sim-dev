#define EPSILON1 0.1
#define EPSILON2 0.2
#define Credits 100
#define NUM_OF_INTERESTS 10
#define PKT_BUFFER_SIZE 200

struct Check{
	uint32_t provider;
	bool sign;
	uint32_t faceValue;
};

class MobileDevice{
	public:
		void NodeInterets(uint32_t interests[]);
		uint32_t nodeId;
		uint32_t AdPktContainer[PKT_BUFFER_SIZE][5]; 
		Vector<Check> checkContainer;	
	private:
		uint32_t nodeInterests[5];
		double pktValue[NUM_OF_INTERESTS][3]; //ad contact likelihood
		double checkValue[NUM_OF_INTERESTS][3]; //check reward contact likelihood
		uint32_t credits;
};
