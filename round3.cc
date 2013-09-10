/*Name: Jean Lin
 *This is the third trial of ns3 mobility.
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

using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE("wifiRound3");



int main(int argc, char *argv[]){
	string phyMode("DsssRate1Mbps");
	uint32_t numNodes = 10;
	uint32_t numPkt = 1;//bytes
	uint32_t stopTime = 30;
	uint32_t interval = 5;
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
	    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
	    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
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
	Ipv4InterfaceContainer wifiInterface = ipv4.Assign(devices);


	 UdpEchoClientHelper echoClient("",1119);
	 echoClient.SetAttribute("MaxPackets", UintegerValue(7));
	 echoClient.SetAttribute("Interval", TimeValue(pktInterval));
	 echoClient.SetAttribute("PacketSize", UintegerValue(1024));

	 ApplicationContainer clientApps = echoClient.Install(wifiNodes.Get(0));
	 clientApps.Start(Seconds(2.0));
	 clientApps.Stop(Seconds(stopTime));
	 
	 //Install UdpEchoSever App
	 UdpEchoServerHelper echoServer(1119);
	 ApplicationContainer serverApps;
	 for(uint32_t i=1;i<numNodes;i++){
	 		serverApps.Add(echoServer.Install(wifiNodes.Get(i)));
	 }
	 serverApps.Start(Seconds(1.0));
	 serverApps.Stop(Seconds(stopTime));


	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
	double x=0.0;

	for(uint32_t i=0;i<numNodes;++i){
		positionAlloc->Add(Vector(x,x,0.0));
		x += 1.0;
	}
	mobility.SetPositionAllocator(positionAlloc);
	mobility.SetMobilityModel("ns3::RandomDirection2dMobilityModel",
				  "Bounds", RectangleValue(Rectangle(0,30,0,30)),
				  "Speed", RandomVariableValue(ConstantVariable(2)),
				  "Pause", RandomVariableValue(ConstantVariable(0.2)));

	mobility.Install(wifiNodes);

	NS_LOG_INFO("Run Simulation.");
	
	AnimationInterface anim("round3.xml");

	
	Simulator::Stop(Seconds(stopTime));
	Simulator::Run();
	Simulator::Destroy();
	
	anim.StopAnimation();

	return 0;
}

