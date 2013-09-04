/*Name: Jean Lin
 *This is the first trial of ns3 mobility.
 *There are 10 nodes in the system and walk randomly.
 *
 *
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/netanim-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/olsr-helper.h"
#include "ns3/csma-module.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "time.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiRound1");

void RecievePAcket(Ptr<Socket> socket){
	NS_LOG_UNCOND("Recieved one packet");
}

static void GenerateTraffic(Ptr<Socket> socket, uint32_t pktSize, uint32_t pktCount,
			    Time pktInterval){

	if(pktCount>0){
		socket->Send(Create<Packet>(pktSize));
		Simulator::Schedule(pktInterval, &GenerateTraffic, socket, pktSize,
				    pktCount-1, pktInterval);
	}else{
		socket->Close();
	}
} 

int main(int argc, char *argv[]){
	uint32_t numNodes = 10;
	uint32_t stopTime = 20;
	uint32_t interval = 5;//seconds
	bool enTracing = false;
	bool verbose = true;

	CommandLine cmd;
	cmd.AddValue("numNodes", "number of nodes",numNodes);
	cmd.AddValue("interval", "the period of generating pakcets", interval);
	cmd.AddValue("enTracing", "enable Tracing", enTracing);
	cmd.AddValue("verbose", "Tell echo application to log if true", verbose);

	cmd.Parse(argc, argv);
	
	if(verbose){
		LogComponentEnable("WifiRound1",LOG_LEVEL_INFO);
	}

	Time pktInterval = Seconds(interval);

	NodeContainer wifiNodes;
	wifiNodes.Create(numNodes);

	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
	double x=0.0;

	for(uint32_t i=0;i<numNodes;++i){
		positionAlloc->Add(Vector(x,x,0.0));
		x += 1.0;
	}
	mobility.SetPositionAllocator(positionAlloc);
	mobility.SetMobilityModel("ns3::RandomDirection2dMobilityModel",
				  "Bounds", RectangleValue(Rectangle(0,20,0,20)),
				  "Speed", RandomVariableValue(ConstantVariable(2)),
				  "Pause", RandomVariableValue(ConstantVariable(0.2)));

	mobility.Install(wifiNodes);

	NS_LOG_INFO("Run Simulation.");
	
	AnimationInterface anim("round1.xml");

	
	Simulator::Stop(Seconds(stopTime));
	Simulator::Run();
	Simulator::Destroy();
	
	anim.StopAnimation();

}

