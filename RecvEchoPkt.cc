void RecvEchoPkt(Ptr<Socket> socket){
	Ptr<Packet> pkt;
	Address from;
	Ipv4Address fromAddrIpv4;
	InetSocketAddress remote;  
	string data;
	stringstream log;
	uint32_t nodeId = socket->GetNode()->GetId();

	pkt = socket->RecvFrom(from);
	uint8_t *buffer = new uint8_t[pkt->GetSize()];
	pkt->CopyData(buffer, pkt->GetSize());
	data = string((char*)buffer);
	if(data="probe"){
		remote = InetSocketAddress::ConvertFrom(from);
		fromAddrIpv4 = remote.GetIpv4();
		InetSocketAddress echo = InetSocketAddress(fromAddrIpv4,1119);
		socket->Connect(echo);
		sendMsg.flush();
		sendMsg<<fromAddrIpv4<<":"<<echo.GetPort();
		pkt = Create<Packet>((uint8_t*)sendMsg.str().c_str(),1000);
		socket->Send(pkt);
		log<<Simulator::Now().GetSeconds()<<" Node["<<nodeId<<"] sends Echo Packet";
		NS_LOG_UNCOND(log.str());
	}else{
		remote = InetSocketAddress::ConvertFrom(from);
		//check the number of the packet the node has
		log<<Simulator::Now()<<" Node["<<nodeId<<"] receives a echo packet from"<<remote.GetIpv4()<<":"<<remote.GetPort();
		//Connect to a node
		/*remote = InetSocketAddress(remote.GetIpv4(),80);
		socket->Connect(remote);
		sendMsg.flush();
		sendMsg<<"Ad Packet";
		pkt = Create<Packet>((uint8_t*)sendMsg.str().c_str(),1000);
		socket->Send(pkt);*/
	}
	
}
