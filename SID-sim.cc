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

#define TRANSMISSION_RANGE 10.0
#define NUM_NODES 10

#include "MobileDevice.h"

using namespace ns3;
using namespace std;

//Functions Declaration

