/*Name: Jean Lin
 *This is the second trial of ns3 mobility.
 *There are 10 nodes in the system and walk randomly.
 *And Node[0] is the source node which send packets to its neighbor.
 *You can use 
 *./waf --run "[PATH}/round3 --PrintHelp"
 *for more information.
 *
 */

//Including libraries
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
#include <sstream>
#include <vector>
#include <string>
#include "time.h"
#include "string.h"

#define TRANSMISSION_RANGE 10.0

bool lock = false;

using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE("WifiRound3");

//Function declaration
static void GenerateTraffic(Ptr<Node> srcNode, uint32_t pktSize, uint32_t numPkt,Time pktInterval, string msg);

void EchoPacket(Ptr<Socket> socket){
	Ptr<Packet> pkt;
	Address from;
	Ipv4Address ipv4_from;
	string msg;
	string data;
	stringstream log;
	stringstream sendMsg;
	int node_id = socket->GetNode()->GetId();
	
	pkt = socket->RecvFrom(from);
	uint8_t *buffer = new uint8_t[pkt->GetSize()];
	pkt->CopyData(buffer, pkt->GetSize());
	data = string((char*)buffer);
	if(data=="probe"){
		NS_LOG_UNCOND("Start Echo");
		
		InetSocketAddress remote = InetSocketAddress::ConvertFrom(from);
		ipv4_from = remote.GetIpv4();
		InetSocketAddress echo = InetSocketAddress(ipv4_from,1119);
		socket->Connect(echo);
		sendMsg<<ipv4_from<<":"<<remote.GetPort();
		pkt = Create<Packet>((uint8_t*) sendMsg.str().c_str(), 1000);
		socket->Send(pkt);
		log <<Simulator::Now().GetSeconds()<<" Node["<<node_id<<"]==> Content: "<<ipv4_from<<":"<<echo.GetPort()<<" sent";
		NS_LOG_UNCOND(log.str());
	}else{
		InetSocketAddress remote = InetSocketAddress::ConvertFrom(from);
		log.flush();
		log <<Simulator::Now().GetSeconds()<<" Node["<<node_id<<"]==> Content: "<<remote.GetIpv4()<<":"<<remote.GetPort()<<" Received";
		NS_LOG_UNCOND(log.str());

	}
}

void ReceivePacket(Ptr<Socket> socket){
	Ptr<Packet> pkt;
	Address from;
	string msg;
	string data;
	stringstream log;
	int node_id = socket->GetNode()->GetId();
	
	pkt = socket->RecvFrom(from);
	uint8_t *buffer = new uint8_t[pkt->GetSize()];
	uint32_t content = pkt->CopyData(buffer, pkt->GetSize());
	data = string((char*)buffer);
	
	log <<Simulator::Now().GetSeconds()<<"Node["<<node_id<<"]==> Content: "<<data<<" pktSize = "<<content<<"bytes";
	NS_LOG_UNCOND(log.str());
}


static void GenerateTraffic(Ptr<Node> srcNode, uint32_t pktSize, uint32_t numPkt,Time pktInterval, string msg){
	TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
	stringstream sendMsg;
	stringstream log;
	Address neighborAddr;
	//int i = 0;
	
	//sender socket setting
	Ptr<Socket> source = Socket::CreateSocket(srcNode, tid);
	//
	InetSocketAddress probe = InetSocketAddress(Ipv4Address("255.255.255.255"), 1119);
	source->SetAllowBroadcast(true);
	source->Connect(probe);

	
	if(numPkt>0){
		sendMsg<<"probe";
		Ptr<Packet> pkt = Create<Packet>((uint8_t*) sendMsg.str().c_str(), pktSize);
		source->Send(pkt);
		
				
		Simulator::Schedule(pktInterval, &GenerateTraffic, srcNode, pktSize, numPkt-1, pktInterval, msg);
	}else{
		cout<<"Source Node: "<<srcNode->GetId()<<" packet transmission ended."<<endl;
		source->Close();
	}
} 

int main(int argc, char *argv[]){
	string phyMode("DsssRate1Mbps");
	uint32_t numNodes = 10;
	uint32_t pktSize = 1000;//bytes
	uint32_t numPkt = 1;//bytes
	uint32_t stopTime = 30;
	uint32_t interval = 5;
	uint32_t initSrcNode = 0;
	string initMsg = "test";
	bool enTracing = false;
	bool verbose = false;

	CommandLine cmd;
	cmd.AddValue("numNodes", "number of nodes",numNodes);
	cmd.AddValue("numPkt", "number of packet",numPkt);
	cmd.AddValue("interval", "the time period of each traffic generation",interval);
	cmd.AddValue("stopTime", "the time when the simulation ended(s)", stopTime);
	cmd.AddValue("enTracing", "enable Tracing", enTracing);
	cmd.AddValue("verbose", "Tell echo application to log if true", verbose);

	cmd.Parse(argc, argv);
	
	Time pktInterval = Seconds(interval);
	if(verbose){
		LogComponentEnable("WifiRound3",LOG_LEVEL_DEBUG);
	}
	NodeContainer wifiNodes;
	wifiNodes.Create(numNodes);

	// The below set of helpers will help us to put together the wifi NICs we want
	WifiHelper wifi;
	if (verbose){
		wifi.EnableLogComponents ();  // Turn on all Wifi logging
	}
	wifi.SetStandard (WIFI_PHY_STANDARD_80211b);

	YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
	// This is one parameter that matters when using FixedRssLossModel
	// set it to zero; otherwise, gain will be added
	wifiPhy.Set ("RxGain", DoubleValue (0) ); 
	// ns-3 supports RadioTap and Prism tracing extensions for 802.11b
	wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 

	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
	// The below FixedRssLossModel will cause the rss to be fixed regardless
	// of the distance between the two stations, and the transmit power
	wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel","MaxRange",DoubleValue (TRANSMISSION_RANGE));
	wifiPhy.SetChannel (wifiChannel.Create ());

	// Add a non-QoS upper mac, and disable rate control
	NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
		                "DataMode",StringValue (phyMode),
		                "ControlMode",StringValue (phyMode));
	// Set it to adhoc mode
	wifiMac.SetType ("ns3::AdhocWifiMac");
	NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, wifiNodes);

	InternetStackHelper internet;
	internet.Install(wifiNodes);

	Ipv4AddressHelper ipv4;
	NS_LOG_INFO("Assign IP Addresses.");
	ipv4.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer i = ipv4.Assign(devices);

	TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
	
	//Assign receive sink to each nodes
	for(uint32_t i=0;i<numNodes;i++){
		Ptr<Socket> echoSink = Socket::CreateSocket(wifiNodes.Get(i), tid);
		Ipv4Address addr = wifiNodes.Get(i)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
		std::cout<<"Ip Address "<<i<<"  = "<<addr<<std::endl;
		InetSocketAddress echoLocal = InetSocketAddress(wifiNodes.Get(i)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal(),1119);
		echoSink->Bind(echoLocal);
		echoSink->SetRecvCallback(MakeCallback(&EchoPacket));
	}

	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
	double x=0.0;

	for(uint32_t i=0;i<numNodes;++i){
		positionAlloc->Add(Vector(x,x,0.0));
		x += 1.0;
	}
	mobility.SetPositionAllocator(positionAlloc);
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

	mobility.Install(wifiNodes);

	
	Simulator::Schedule(Seconds(0.0), &GenerateTraffic,wifiNodes.Get(initSrcNode), pktSize, numPkt, pktInterval, initMsg);

	NS_LOG_INFO("Run Simulation.");
	
	AnimationInterface anim("round2.xml");

	
	Simulator::Stop(Seconds(stopTime));
	Simulator::Run();
	Simulator::Destroy();
	
	anim.StopAnimation();

	return 0;
}

