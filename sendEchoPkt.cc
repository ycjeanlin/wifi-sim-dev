/*Name: Jean Lin
 *Send Echo Packet is for finding the nearest neighbor in the
 *local area.
 *
 *
 */

static void SendEchoPkt(Ptr<Node> srcNode, uint32_t pktSize){
	TypeId tid = typeId::LookupByName("ns3::UdpSocketFactory");
	stringstream sendMsg;
	stringstream log;

	Ptr<Socket> source =Socket::CreateSocket(srcNode, tid);
	InetSocket probe = InetSocketAddress(Ipv4Address("255.255.255.255"), 1119);
	source->SetAllowBroadcast(true);
	source->Connect(probe);
	sendMsg<<"probe";
	Ptr<Packet> pkt = Create<Packet>((uint8_t*)sendMsg.str().c_str(), pktSize); 

	source->send(pkt);

}
