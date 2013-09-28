/* Self-Interest-Driven Incentive Scheme in MP2PN
 * Follow the work Self-Interest-Driven Incentives in MP2PN from INFOCON, 2013.
 * Author of the paper: Ting Ning
 * 
 * Programmer: Jean Lin
 * Date: 9/23/2013
 * 
 * Default total interests in the system: 10
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
#include <vector>
#include <string>
#include "time.h"
#include "string.h"

#include "MobileDevice.h"

#define DEBUG 
#define TRANSMISSION_RANGE 10.0
#define NUM_NODES 30


using namespace ns3;
using namespace std;

MobileDevice mDevice[NUM_NODES];

static void SendEchoPkt(Ptr<Node> srcNode, uint32_t pktSize);

//Functions Declaration
static void SendEchoPkt(Ptr<Node> srcNode, uint32_t pktSize){
	stringstream sendMsg;
	stringstream log;

	TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
	Ptr<Socket> source =Socket::CreateSocket(srcNode, tid);
	InetSocketAddress probe = InetSocketAddress(Ipv4Address("255.255.255.255"), 1119);
	source->SetAllowBroadcast(true);
	source->Connect(probe);
	sendMsg<<"probe";
	Ptr<Packet> pkt = Create<Packet>((uint8_t*)sendMsg.str().c_str(), pktSize); 

	source->Send(pkt);

}

void RecvEchoPkt(Ptr<Socket> socket){
	Ptr<Packet> pkt;
	Address from;
	Ipv4Address fromAddrIpv4;
	string data;
	stringstream log;
	stringstream sendMsg;
	uint32_t nodeId = socket->GetNode()->GetId();
	//uint32_t connectNode;

	pkt = socket->RecvFrom(from);
	uint8_t *buffer = new uint8_t[pkt->GetSize()];
	pkt->CopyData(buffer, pkt->GetSize());
	data = string((char*)buffer);
	if(data=="probe"){
		InetSocketAddress remote = InetSocketAddress::ConvertFrom(from);
		fromAddrIpv4 = remote.GetIpv4();
		InetSocketAddress echo = InetSocketAddress(fromAddrIpv4,1119);
		socket->Connect(echo);

		sendMsg.flush();
		sendMsg<<nodeId;
		pkt = Create<Packet>((uint8_t*)sendMsg.str().c_str(),1000);
		socket->Send(pkt);
#ifdef DEBUG		
		log<<Simulator::Now().GetSeconds()<<" Node["<<nodeId<<"] sends Echo Packet to "<<fromAddrIpv4;
		NS_LOG_UNCOND(log.str());
#endif
	}else{
		//int interests1[5];
		//int interests2[5];

#ifdef	DEBUG	
		log<<Simulator::Now()<<" Node["<<nodeId<<"] receives a echo packet from Node["<<data<<"]";
		NS_LOG_UNCOND(log.str());
#endif
		//Connect to a node
		// connectNode = atoi(data.c_str());
		// mDevice[connectNode].getNodeInterests(interests2);
		// mDevice[nodeId].updateADCL()
		
	}
	
}


int main(int argc, char *argv[]){
	string phyMode("DsssRate1Mbps");
	uint32_t numNodes = NUM_NODES;
	uint32_t pktSize = 1000;//bytes
	uint32_t stopTime = 30;
	uint32_t initSrcNode = 0;
	bool enTracing = false;

	CommandLine cmd;
	cmd.AddValue("stopTime", "the time when the simulation ended(s)", stopTime);
	cmd.AddValue("enTracing", "enable Tracing", enTracing);

	cmd.Parse(argc, argv);	

	Time pktInterval = Seconds(interval);

	NodeContainer wifiNodes;
	wifiNodes.Create(numNodes);

	//give each node a mobile device
	for (int i = 0; i < NUM_NODES; ++i){
		mDevice[i].nodeId = i;
	}

	//initialize some ad packets
	for (int i = 0; i < 20; ++i){
		mDevice[i].AdPktContainer[0][0] = i;
		mDevice[i].AdPktContainer[0][1] = mDevice[i].nodeId;
		mDevice[i].AdPktContainer[0][2] = 1;
		mDevice[i].AdPktContainer[0][3] = i%10;
		mDevice[i].AdPktContainer[0][4] = 200;
		mDevice[i].ptrAdContainer++;

	}

	// The below set of helpers will help us to put together the wifi NICs we want
	WifiHelper wifi;

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
		echoSink->SetRecvCallback(MakeCallback(&SendEchoPkt));
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

	Simulator::Schedule(Seconds(0.0), &SendEchoPkt,wifiNodes.Get(initSrcNode), pktSize);

	NS_LOG_INFO("Run Simulation.");
	
	AnimationInterface anim("round2.xml");

	
	Simulator::Stop(Seconds(stopTime));
	Simulator::Run();
	Simulator::Destroy();
	
	anim.StopAnimation();

	return 0;
}
