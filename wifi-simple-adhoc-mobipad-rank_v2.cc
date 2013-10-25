/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 The Boeing Company
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

// 
// This script configures two nodes on an 802.11b physical layer, with
// 802.11b NICs in adhoc mode, and by default, sends one packet of 1000 
// (application) bytes to the other node.  The physical layer is configured
// to receive at a fixed RSS (regardless of the distance and transmit
// power); therefore, changing position of the nodes has no effect. 
//
// There are a number of command-line options available to control
// the default behavior.  The list of available command-line options
// can be listed with the following command:
// ./waf --run "wifi-simple-adhoc --help"
//
// For instance, for this configuration, the physical layer will
// stop successfully receiving packets when rss drops below -97 dBm.
// To see this effect, try running:
//
// ./waf --run "wifi-simple-adhoc --rss=-97 --numPackets=20"
// ./waf --run "wifi-simple-adhoc --rss=-98 --numPackets=20"
// ./waf --run "wifi-simple-adhoc --rss=-99 --numPackets=20"
//
// Note that all ns-3 attributes (not just the ones exposed in the below
// script) can be changed at command line; see the documentation.
//
// This script can also be helpful to put the Wifi layer into verbose
// logging mode; this command will turn on all wifi logging:
// 
// ./waf --run "wifi-simple-adhoc --verbose=1"
//
// When you are done, you will notice two pcap trace files in your directory.
// If you have tcpdump installed, you can try this:
//
// tcpdump -r wifi-simple-adhoc-0-0.pcap -nn -tt
//

 
//#include "random.h"  
 
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include "ns3/ns2-mobility-helper.h"
#include <sstream>


#define PI 3.14159265
#define MATH_E 2.71828 
#define TRANSMISSION_RANGE 250.0
#define SourceCoordinate 1500
#define topk 3
#define Howmany 1
#define multicastNumber 8  //7 number of user setting: 8

NS_LOG_COMPONENT_DEFINE ("WifiSimpleAdhoc");

using namespace ns3;

std::string CSVfileName="wifi-simple.csv";
NodeContainer c;

int total_node=101;
double rxBytes=0;
double recount=0;
double   broadcast_delay=0;
int distance_count=0;
std::string ran ="1" ;

//content save
std::string SaveArray[1001][200000];
//int

int countarray[1001];
double Node_Receive_time[1001][2];
int now_before=0;
int countnow_before[1001];
double sending_time[1001]; 

//record the content in each user
double record_receive_time[1001][101];
double record_enter[1001][2000];
double record_leave[1001];
double Delivery_Time[1001][10];
double End_Delivery_Time[1001][10];
double distance_over_advrange=0;
int enter_count=0;
int leaves_number[10];
int leaves_node[1001];

int number_of_messages[10];


int eachNode_of_messages[1001];// for fariness
int timeSlotofMessages[1800]; 



//---multicast
int multicast[1001][1001];
int multi_candadiate[100];
int sort_multi[100];
int sort2_multi[100];

//-----mobipad-----
int hop_value[10];
int hopRecord[1001][10];
int duplicate[1001][10];
int source[1001][1001];
double times;
int hardware_cost[1001];
double difference[1001][10];
int countNeighbor[1001][1][10];
int NeighborList[1001][1001][10];
int Previous_NeighborList[1001][1001][10];
int differenceNeighbor[1001][1][10];
double timeSchedDelay[1001][10];
int Benefit_Interesting[1001][10];
int Threshold_TN[1001];
int Threshold_TN2[1001];
int ListCount[1001][10];
int same[10];
int previous_duplicate[1001][10];
double previous_duplicate_time[1001][10];
int previous_countNeighbor[1001][1][10];
double duplicate_rate[1001][10];
int User_benefit[1001];
double delay_time=0;
double  Delay_T[1001][10];
int stable[1001][10];
int hopV[1001][10];
//ranking
int send_candidate=0;
int sending=0;
int sending_meet[1001][10];


//------------------------------
double delay_time_record[1001][360][10];
double probability_record[1001][360][10];
int delay_time_count[1001][10];
int probability_count[1001][10];


//----flooding-----

double Node_position[1001];
int total_receive_node[10];
int check=0;
//calcuate the delivery time

double entertime[1001][1000][10];
int entercount[1001][10];
double leavestime[1001][1000][10];
int leavecount[1001][10];
int enter_flag[1001][10];
double total_interval[10]={0,0};
int must_in[1001][10];


//preference---------------------
int interestingType[10];
int recordInteresting[10];
int UserInteresting[1001][10];
int preferenceType;
int receiveType;
int receiveORnot[10];
int ADVtype=0;
int non_interesting[1001];

double FirstReceive[1001][10];
int beenReceived[1001][10];
int FReceive_once[1001][10];


//------------------



//----gossiping-------
double ALPHA=0.5;
double BETA=0.9;
double Radius=1000;
double ADVduration=1800;
double RadiusT=0;


//---------------------




int sourcefrom_id=0;
int sourceNodeID=0;
std::vector<std::string> vec;
//----------------------




static void GenerateTraffic (uint32_t pktSize, 
                             uint32_t pktCount, Time pktInterval,int initial_source_node ,std::string initial_message);



int DataRanking(int user, int ADVtype)
{
int hopRecord2[10];
int reco=0;
int ranking_standard=0;

std::cout <<"user : "<<user<<"interesting : "<<UserInteresting[user][0]<<";"<<UserInteresting[user][1]<<std::endl;
 for(int record=1;record<11;record++)
         {
         //std::cout <<"value: "<<sort_multi[ca]<<std::endl;
         if(hopRecord[user][record]!=0 && record!=UserInteresting[user][0] && record!=UserInteresting[user][1]){
           hopRecord2[reco]=hopRecord[user][record];
           
           //std::cout <<"Type : "<<record<<"value: "<<hopRecord[user][record]<<std::endl;

           reco++; 
            }
         }
if(reco>=3){
//int x_end=sizeof(hopRecord2)/sizeof(int);
         std::sort(hopRecord2,hopRecord2+reco);  
      // for(int rr=0;rr<=reco-1;rr++)
      //    { std::cout <<"hop2: "<<hopRecord2[rr]<<std::endl;}

   if(hopRecord[user][UserInteresting[user][0]]!=0 && hopRecord[user][UserInteresting[user][1]]!=0)
      {
        sending=sending+2;
        send_candidate=topk-sending;     
      }
   else if( hopRecord[user][UserInteresting[user][0]]==0 && hopRecord[user][UserInteresting[user][1]]!=0)   
      {sending=sending+1;
       send_candidate=topk-sending;}
   else if( hopRecord[user][UserInteresting[user][0]]!=0 && hopRecord[user][UserInteresting[user][1]]==0)   
      {sending=sending+1;
       send_candidate=topk-sending;}
   else
       {sending=0;
        send_candidate=topk-sending;
        }


sending_meet[user][UserInteresting[user][0]]=1;
sending_meet[user][UserInteresting[user][1]]=1;

      //std::cout <<"send_candidate: "<<send_candidate<<"hop_value_sort : "<<hopRecord2[send_candidate-1]<<std::endl;

ranking_standard=hopRecord2[send_candidate-1];

}

else
{ranking_standard=360;}

sending=0;


return ranking_standard;
}


void Userpreference()
{

srand(time(NULL));
 for(int userp=1 ;userp<total_node;userp++)
   {
    UserInteresting[userp][0]=rand()%10+1;
    UserInteresting[userp][1]=rand()%10+1;
 
   for(int rep=1; rep<11;rep++){
    if(UserInteresting[userp][0]==UserInteresting[userp][1])
       {
           UserInteresting[userp][1]=rand()%10+1;
       }
    if(UserInteresting[userp][0]!=UserInteresting[userp][1])
             break;
    }
    
 // std::cout<<"GGG: "<<UserInteresting[userp][0]<<"HHH: "<<UserInteresting[userp][1]<<std::endl;


    interestingType[UserInteresting[userp][0]]++;
    interestingType[UserInteresting[userp][1]]++;
    }



}


void InterestingMatch(int node_id, int receiveType1)
{ 

 //receiveType1=receiveType1%11;

        if(receiveType1==UserInteresting[node_id][0] || receiveType1==UserInteresting[node_id][1] ){
           recordInteresting[receiveType1]++;
           Benefit_Interesting[node_id][receiveType1]++;
           User_benefit[node_id]=User_benefit[node_id]+2;
                   
                //Threshold_TN[node_id]=1000;

           Threshold_TN2[node_id]=ceil(  (1-1/pow(User_benefit[node_id],1.5)) *70  );//*((1800-FirstReceive[node_id][receiveType])/1800);  //*100=>*k
               if(Threshold_TN[node_id]<Threshold_TN2[node_id])
                  Threshold_TN[node_id]=Threshold_TN2[node_id]; 
               else     
                  Threshold_TN[node_id]=Threshold_TN[node_id]; 
          }
         
       else
         {       //Threshold_TN[node_id]=1000;

                non_interesting[node_id]=non_interesting[node_id]+2;;
                Threshold_TN2[node_id]=ceil(  (1-1/pow(non_interesting[node_id],1.5)) *  35 );//*((1800-FirstReceive[node_id][receiveType])/1800);
                if(Threshold_TN[node_id]<Threshold_TN2[node_id])
                  Threshold_TN[node_id]=Threshold_TN2[node_id]; 
                else     
                  Threshold_TN[node_id]=Threshold_TN[node_id]; 
  
         }
                  
                 // std::cout <<"Threshold!!   "<< Threshold_TN[node_id]<<std::endl;
   
}


double CompareDifference(int user, int neighbor[1001][1001][10], int p_duplicate[1001][10], int ad_type, double p_time[1001][10], int Now_numbers ,int P_numbers)
{
int Thesame=0;
  
   for(int LG=0; LG<Now_numbers;LG++)
      {std::cout <<"UserID: "<<user<<"Now neighbors!!   "<< neighbor[user][LG][ad_type]<<std::endl;}

   for(int LG=0; LG<P_numbers;LG++)
      {std::cout <<"UserID: "<<user<<"Previous neighbors!!   "<<  Previous_NeighborList[user][LG][ad_type]<<std::endl;}
 
    for(int LG1=0; LG1<P_numbers;LG1++) {
        for(int LG2=0; LG2<Now_numbers;LG2++) {
            if(Previous_NeighborList[user][LG1][ad_type]==neighbor[user][LG2][ad_type])         
                  {  
                    Thesame++;
                    break;
                  }
            
         }
    }
     std::cout<<"the same"<<Thesame<<"Now_numbers"<<Now_numbers<<"P_numbers"<<P_numbers<<std::endl;
     differenceNeighbor[user][1][ad_type]=Now_numbers-Thesame; 




    if(Now_numbers!=0)
     difference[user][ad_type]=(double)(differenceNeighbor[user][1][ad_type])/(double)(Now_numbers);

    if(difference[user][ad_type]==0)
    { stable[user][ad_type]++; }
    else
    {// if(stable[user]>0)
      // stable[user]--; 
     // else
       stable[user][ad_type]=0;
    }


    if((Simulator::Now ().GetSeconds ()-p_time[user][ad_type])!=0)
   // std::cout<<"duplicate[user][ad_type]"<<duplicate[user][ad_type]<<"p_duplicate[user][ad_type]"<<p_duplicate[user][ad_type]<<std::endl;
    duplicate_rate[user][ad_type]=(duplicate[user][ad_type]-p_duplicate[user][ad_type])/(Simulator::Now ().GetSeconds ()-p_time[user][ad_type]);
    
    //stabel or not stable
       if(difference[user][ad_type]<0.5 && difference[user][ad_type]!=0) //stable  neighbor almost the same  (time long)
       {std::cout<<"1111"<<std::endl;
        delay_time=5*((1+duplicate_rate[user][ad_type])/(double)(difference[user][ad_type]));
        }
       else if(difference[user][ad_type]==0 && Now_numbers!=0) 
       {std::cout<<"2222"<<"stable the same: "<<stable[user][ad_type]<<"duplicate rate: "<<duplicate_rate[user][ad_type]<<std::endl;
        delay_time=5*((1*(stable[user][ad_type]+1)+duplicate_rate[user][ad_type])/0.1);
        }
       else if(difference[user][ad_type]==0 && Now_numbers==0)   
        {std::cout<<"3333"<<std::endl;
         delay_time=5*(1+duplicate_rate[user][ad_type]);
         }
       else  //unstable
       {std::cout<<"4444"<<"difference :"<<difference[user][ad_type]<<"duplicate rate: "<<duplicate_rate[user][ad_type]<<std::endl;
        delay_time=5*((1+duplicate_rate[user][ad_type])/(double)difference[user][ad_type]) ;}
     

    // std::cout <<"UserID: "<<user<<"Difference numbers: "<<differenceNeighbor[user][1]<<"Differemce Ratio: "<<difference[user]<<std::endl;
    // std::cout <<"UserID: "<<user<<"Duplicate ratio:  "<<duplicate_rate<<std::endl;
   //  std::cout <<"UserID: "<<user<<"Previous: "<<p_duplicate[user][ad_type]<<"Now: "<<duplicate[user][ad_type]<<std::endl;
   //  std::cout <<"UserID: "<<user<<"Previous Time: "<<p_time[user]<<"Now: "<<Simulator::Now ().GetSeconds ()<<std::endl;
       //std::cout <<"UserID: "<<user<<"Delay Time: "<<delay_time<<"ADVTYPE: "<<ad_type<<std::endl;

/*


  double delay_time=5*(duplicate_rate+difference);*/
  
 //delay_time=5;
delay_time_record[user][delay_time_count[user][ad_type]][ad_type]=delay_time;
delay_time_count[user][ad_type]++;




  return delay_time;
}

void multicheck(int candidate_count,int source2)
{       srand(time(NULL));
      
       if(candidate_count>=multicastNumber){ 
         for(int ca=0;ca<candidate_count;ca++)
         {
         //std::cout <<"candidate: "<<multi_candadiate[ca]<<std::endl;
         sort_multi[ca]=rand();
         //std::cout <<"value: "<<sort_multi[ca]<<std::endl;
         sort2_multi[ca]=sort_multi[ca];
         }
         std::sort(sort2_multi,sort2_multi+candidate_count);
         
         for(int ca=0;ca<candidate_count;ca++){
         //std::cout <<"value: "<<sort2_multi[ca]<<"Eighth: "<<sort2_multi[candidate_count-8]<<std::endl;

            if (sort_multi[ca]>sort2_multi[candidate_count-multicastNumber])
                 {//std::cout <<"source2:"<<source2<<"Node who : "<<multi_candadiate[ca]<<std::endl;
                  multicast[source2][multi_candadiate[ca]]=1;}
            else{multicast[source2][multi_candadiate[ca]]=0;}
         }
                   
       }
       
       else{
        for(int ca2=0;ca2<multicastNumber-1;ca2++)
        {
         multicast[source2][multi_candadiate[ca2]]=1;
         }
       }
}

void StringSplit(std::string s,char splitchar,std::vector<std::string>& vec)
{
//clear the vector
if(vec.size()>0) vec.clear();
int length = s.length();
int start=0;
int is = 0;
//check if the first char is split
        if(s[0] == splitchar){
                start = 1;
                is=1;
        }
        for(int i=is;i<length;i++){
                if(s[i] == splitchar){
                        vec.push_back(s.substr(start,i - start));
                        start = i+1;
                }else if(i == length-1){
                        vec.push_back(s.substr(start,i+1 - start));
                        }
        }
}

void datadrop( int node_id)
{
      Ptr<MobilityModel> mobility = c.Get(node_id)->GetObject<MobilityModel> ();    
      Vector pos = mobility->GetPosition(); 
      double o1range=sqrt(pow((SourceCoordinate-pos.x),2)+pow((SourceCoordinate-pos.y),2));
      std::cout <<"How far:"<<o1range<<std::endl;
      for(int drop=6;drop>=0;drop--){
      SaveArray[node_id][drop]="null";}
      

      countarray[node_id]=0;
      for(int type=1;type<6;type++)
      {FReceive_once[node_id][type]=0;}
        
}


void 
dropcheck()
{
for(int type=1;type<6;type++){
      for(int enter2=1;enter2<total_node;enter2++){
           Ptr<MobilityModel> mobility = c.Get(enter2)->GetObject<MobilityModel> ();    
           Vector pos = mobility->GetPosition(); 
           double o2range=sqrt(pow((SourceCoordinate-pos.x),2)+pow((SourceCoordinate-pos.y),2));
                 //std::cout <<"o2range"<<o2range<<" Reveive or not"<<FReceive_once[enter2]<<std::endl;
         
             if(FReceive_once[enter2][type]>0 && o2range >1000)
               {
                 datadrop(enter2);
               }
             else
              {//std::cout <<"DIS in"<<enter2<<std::endl;
              }
        }

}

  Simulator::Schedule(Seconds (5.0), &dropcheck);

}

double forwardProbability(int node_id,int ADVtype)
{

double G1probability;


if(hopRecord[node_id][ADVtype]!=0)
G1probability=1-(pow(BETA,1/(hopRecord[node_id][ADVtype]/(700.0))));//*(Simulator::Now ().GetSeconds ()/1800);

else if(node_id==0 && Simulator::Now ().GetSeconds ()>100)
{G1probability=0;}

else
G1probability=1;

std::cout <<"G1probability: "<<G1probability<<std::endl;
/*


Ptr<MobilityModel> mobility2 = c.Get(node_id)->GetObject<MobilityModel> ();    
Vector pos2 = mobility2->GetPosition(); 
Gdistance=sqrt(pow((2500-pos2.x),2)+pow((2500-pos2.y),2));
age=Simulator::Now ().GetSeconds ()-0;

        if(age<= ADVduration) 	
		{
                RadiusT = (1-pow(BETA,ADVduration-age))*1000;
                // std::cout <<"RadiusT: " << RadiusT<<std::endl; 
                 }
	else
		{RadiusT = 0.0;}
	
	if(RadiusT < 0.001)
	{
		return 0.0;
	}
		
	if(Gdistance <= RadiusT)
		{Gprobability = 1 + pow(ALPHA, RadiusT+1) - pow(ALPHA, RadiusT+1-Gdistance);}
	else
		{Gprobability = (1 + pow(ALPHA, RadiusT+1) - ALPHA)*pow(ALPHA, Gdistance - RadiusT);}

*/

/*
if(node_id==0 && Simulator::Now ().GetSeconds ()>100)
{G1probability=0;}
else
G1probability=1;*/

G1probability=0.5;

probability_record[node_id][probability_count[node_id][ADVtype]][ADVtype]=G1probability;
probability_count[node_id][ADVtype]++;

return G1probability;
}

void 
EnterLeave()
{

for(int type=1;type<Howmany+1;type++){
  for(int enter=1;enter<total_node;enter++)
   {  
      Ptr<MobilityModel> mobility = c.Get(enter)->GetObject<MobilityModel> ();    
      Vector pos = mobility->GetPosition(); 
      double range=sqrt(pow((SourceCoordinate-pos.x),2)+pow((SourceCoordinate-pos.y),2));
      record_enter[enter][enter_count]=range;
  
      if(range<=1000 && enter_flag[enter][type]==0){
       must_in[enter][type]=1; 
       }      


      if(range<=1000 && enter_flag[enter][type]==0 && beenReceived[enter][type]==0){ 
      enter_flag[enter][type]=1;
      entertime[enter][entercount[enter][type]][type]=Simulator::Now ().GetSeconds ();
      entercount[enter][type]++;     
       }
     
      
     if(enter_count>1){
        if(record_enter[enter][enter_count]>1000  && record_enter[enter][enter_count-1]<=1000 && beenReceived[enter][type]==0)
         { 
           enter_flag[enter][type]=0;
           must_in[enter][type]=0;
           leavestime[enter][leavecount[enter][type]][type]=Simulator::Now ().GetSeconds ();
           
           leavecount[enter][type]++;
           leaves_number[type]++;
           std::cout<<"Now:"<<record_enter[enter][enter_count]<<" Before "<<record_enter[enter][enter_count-1]<<std::endl; 
           std::cout<<"Type: "<<type<<"Leaves node:"<<(enter+1)<<" Total leaves "<<leaves_number[1]<<std::endl; 
         }  
   
      } 
   }
}


 enter_count++;
 

 if(Simulator::Now ().GetSeconds ()==1799){

for(int type=1;type<Howmany+1;type++){
 std::cout<<"number_of_messages"<<number_of_messages[type]<<std::endl;
     for(int output=1;output<total_node;output++)
          { if(beenReceived[output][type]>0 && FirstReceive[output][type]!=0){
            total_interval[type]=(FirstReceive[output][type]-entertime[output][entercount[output][type]-1][type])+total_interval[type];
           // std::cout<<"First : "<<FirstReceive[output][type]<<"entertime: "<<entertime[output][entercount[output][type]-1][type]<<"total:"<<total_interval[type]<<std::endl;
             }
            for(int output1=0 ;output1<entercount[output][type];output1++)
            {std::cout<<output<<" Enter time:"<<" "<<entertime[output][output1][type]<<std::endl;}  
            for(int output2=0 ;output2<leavecount[output][type];output2++)
            {std::cout<<output<<" Leaves time:"<<" "<<leavestime[output][output2][type]<<std::endl;}
            
          } 

 std::cout<<" Total delivery time:"<<" "<<total_interval[type]<<std::endl;
}     
    


for(int type=1;type<Howmany+1;type++){  
std::cout<<" ------------------------------Type "<<type<<"------------------------------"<<std::endl;  
      std::cout<<"Interesting Type: "<<type<<" : "<<interestingType[type]<<"Receiving Number : "<<recordInteresting[type]<<std::endl;


      for(int rece=1; rece <total_node; rece++)
           { //if(type==1 || type==4)
             std::cout<<"Receive time:"<<rece<<" "<<FirstReceive[rece][type]<<std::endl;}

   

}     

     for(int rece2=1; rece2 <total_node; rece2++)
           {std::cout<<rece2<<": "<<eachNode_of_messages[rece2]<<"=> "<<Threshold_TN[rece2]<<std::endl; }
     std::cout<<"----------------------------------------------------------------------------------"<<std::endl;
     for(int rece3=0; rece3 <1801; rece3++)
           {
            std::cout<<rece3<<": "<<timeSlotofMessages[rece3]<<std::endl;}
 
 

//----------------delay time------------------ 
    for(int type=1;type<Howmany+1;type++){ 
         for(int rece=1; rece <total_node; rece++){ 
          for(int rece2=0; rece2 <360; rece2++){
            std::cout<<"Delay user:"<<rece<<"=> "<<delay_time_record[rece][rece2][type]<<std::endl;
            if(delay_time_record[rece][rece2+1][type]==0)
              break;

             }
         }
         for(int rece3=1; rece3 <total_node; rece3++){ 
          for(int rece4=0; rece4 <360; rece4++){
            std::cout<<"Probability user:"<<rece3<<"=> "<<probability_record[rece3][rece4][type]<<std::endl;
             if(probability_record[rece3][rece4+1][type]==0)
              break;

             }
         }

}



 }



 Simulator::Schedule(Seconds (1.0), &EnterLeave);
}

void
GetPosition (Ptr<Node> node,int giposition) 
{  
  
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> (); 
  Vector pos = mobility->GetPosition(); 
  int source_node = giposition;
  //std::cout << Simulator::Now ().GetSeconds ()<<" second:Node" << source_node << " " << pos.x << " " << pos.y << std::endl; 

  sending_time[source_node]=(Simulator::Now ().GetSeconds ());
  double SourceX=pos.x;
  double SourceY=pos.y;
  distance_count=0;
  

//distance between central
  Node_position[source_node]=sqrt(pow((SourceCoordinate-pos.x),2)+pow((SourceCoordinate-pos.y),2));


 //calcuate distance between neighbor and source
  for(int neighborcount=0;neighborcount<total_node;neighborcount++)
   {  
  
    if(neighborcount!=giposition)//itself
     {
      Ptr<MobilityModel> mobility = c.Get(neighborcount)->GetObject<MobilityModel> (); 
//     Vector myvelocity = mobility->GetVelocity();
     Vector pos3 = mobility->GetPosition();
//     double Node_velocity=sqrt(pow(myvelocity.x,2)+pow(myvelocity.y,2));

//  double destX=pos3.x+myvelocity.x*5;
//  double destY=pos3.y+myvelocity.y*5; 
//  std::cout <<"Node"<<neighborcount<< "X:"<< pos3.x << "Node" <<neighborcount <<"Y:"<< pos3.y <<" Velocity: "<<Node_velocity <<std::endl;
//  std::cout <<"DEST: "<<neighborcount<< "X:"<< destX << "Node" <<neighborcount <<"Y:"<< destY <<std::endl;
    
      double distance=sqrt(pow((SourceX-pos3.x),2)+pow((SourceY-pos3.y),2));
       //if transmission range< X       
 
          if(distance<=TRANSMISSION_RANGE){
          multi_candadiate[distance_count]=neighborcount; 
         
  
          distance_count++;           
          //std::cout <<"Distance:"<<distance<<"Node:"<<(neighborcount+1)<< "_X:"<< pos.x <<"_Y:"<< pos.y<<"velocity:"<<Node_velocity<<std::endl; 
          
          //std::cout <<"Node"<<neighborcount<< "_X:"<< pos3.x <<" "<<"Node" <<neighborcount <<"_Y:"<< pos3.y <<std::endl; 
          
          /*Ptr<Ipv4> testipv4= c.Get (neighborcount)->GetObject<Ipv4>();
          Ipv4InterfaceAddress iaddr =testipv4->GetAddress(1,0);
          Ipv4Address receiveaddr= iaddr.GetLocal();*/
         // std::cout <<"Node address:"<<neighborcount<<" :"<<receiveaddr<<std::endl;
          }
      }  
    }




 multicheck(distance_count,source_node); 


/*
   
  //get node address save to array
  Ptr<Ipv4> testipv4= c.Get (socket->GetNode ()->GetId ())->GetObject<Ipv4>();
  Ipv4InterfaceAddress iaddr =testipv4->GetAddress(1,0);
  Ipv4Address testaddr= iaddr.GetLocal();
  //std::cout<<"address:"<<testaddr<<std::endl;
*/
} 


void ReceivePacket (Ptr<Socket> socket)
{
	Ptr<Packet> packet;
	Address from;
	std::string data;
	int node_id=0;
	node_id=(socket->GetNode ()->GetId ())+1;
	// double time_compare ;
	Ptr<Ipv4> testipv4= c.Get (socket->GetNode ()->GetId ())->GetObject<Ipv4>();
	Ipv4InterfaceAddress iaddr =testipv4->GetAddress(1,0);
	Ipv4Address testaddr= iaddr.GetLocal();
	std::string data_compare;  

	while (packet= socket->RecvFrom(from)){  

		//----------save the packet content----------------  
		uint8_t *buffer = new uint8_t [packet->GetSize ()];
		uint32_t content= packet->CopyData (buffer, packet->GetSize ());
		data=std::string((char*)buffer);
		//SaveArray[socket->GetNode ()->GetId ()][countarray[socket->GetNode ()->GetId ()]]=data;
		data_compare=data;
		if(multicast[sourcefrom_id][socket->GetNode ()->GetId ()]==1)
			std::cout<<"Packet content: "<< data<<"  "<< "; byte: "<<content<<" bytes" <<std::endl; 

		StringSplit( data,'-',vec);
		sourcefrom_id=atoi(vec[1].c_str());//get soure node id
		receiveType=atoi(vec[0].c_str());//get adv type
		hopV[sourcefrom_id][receiveType]=atoi(vec[2].c_str());//hop value


		Ptr<MobilityModel> mobility = c.Get(socket->GetNode ()->GetId ())->GetObject<MobilityModel> (); 
		Vector pos5 = mobility->GetPosition(); 
		std::cout << "Position:" <<pos5.x << " ; " << pos5.y << std::endl; 

		if(packet->GetSize()>0)
		{
			if(multicast[sourcefrom_id][socket->GetNode ()->GetId ()]==1){
			std::cout <<"source2:"<<sourcefrom_id<<"Node who :"<<socket->GetNode ()->GetId ()<<"multicast check: "<<multicast[sourcefrom_id][socket->GetNode ()->GetId ()]<<std::endl;

			std::cout<<Simulator::Now ().GetSeconds ()<<" "<<"Seconds ;Address:"<<testaddr<<" "<<";User node: "<< 
			node_id<< " ;Received data from :" << vec[1] <<" Receive ADV type: "<<receiveType<<" address : "<<InetSocketAddress::ConvertFrom (from).GetIpv4()<<std::endl;}
		}


		//COMPARE TIME ( not using)
		Node_Receive_time[socket->GetNode ()->GetId ()][countnow_before[socket->GetNode ()->GetId()]]=Simulator::Now ().GetSeconds ();
		if(countnow_before[socket->GetNode ()->GetId ()]==0)    
		countnow_before[socket->GetNode ()->GetId ()]++;
		else
		countnow_before[socket->GetNode ()->GetId ()]--;

		delete buffer;

	}//end while

   uint32_t packetSize1 = 1000; // bytes
   uint32_t numPackets1 = 500; 
   //uint32_t numPackets1 = 1;   //flooding
   broadcast_delay=broadcast_delay+0.00001;
   Time interPacketInterval1 = Seconds (5);//??
 
   if(hopV[sourcefrom_id][receiveType]>hopRecord[socket->GetNode ()->GetId ()][receiveType]){   
          hopRecord[socket->GetNode ()->GetId ()][receiveType]=hopV[sourcefrom_id][receiveType];
          //std::cout<<"hello!! "<< hopRecord[socket->GetNode ()->GetId ()][receiveType]<<std::endl;
    }


   for(int LG=0 ;LG<=ListCount[socket->GetNode ()->GetId ()][receiveType];LG++){
               if(NeighborList[socket->GetNode ()->GetId ()][LG][receiveType]==sourcefrom_id){
                 same[receiveType]++;                 
            }       
        }  

   if(same[receiveType]==0){
        
         countNeighbor[socket->GetNode ()->GetId ()][1][receiveType]++;
         NeighborList[socket->GetNode ()->GetId ()][ListCount[socket->GetNode ()->GetId ()][receiveType]][receiveType]=sourcefrom_id ;
         ListCount[socket->GetNode ()->GetId ()][receiveType]++;
       
      }
  
       
         same[receiveType]=0;
  
//---gossiping------------------------------------------------------------------------------


Ptr<MobilityModel> mobility = c.Get(socket->GetNode ()->GetId ())->GetObject<MobilityModel> ();    
Vector pos = mobility->GetPosition(); 
double out_distance=sqrt(pow((SourceCoordinate-pos.x),2)+pow((SourceCoordinate-pos.y),2));


if(out_distance<=1000 && must_in[socket->GetNode ()->GetId ()][receiveType]==1){
for(check=0; check < countarray[socket->GetNode ()->GetId ()];check++){
       //std::cout<<"hello data content: "<<SaveArray[socket->GetNode ()->GetId ()][check]<<std::endl;
        if(SaveArray[socket->GetNode ()->GetId ()][check]==vec[0]){
            receiveORnot[receiveType]=1; //check data in cache
            break;}
         }
}
   


//-----------------------mobipad----------------------------------
if(receiveORnot[receiveType]==1 && multicast[sourcefrom_id][socket->GetNode ()->GetId ()]==1){
  
   duplicate[socket->GetNode ()->GetId ()][receiveType]++;
  // std::cout<<"USER: "<< socket->GetNode ()->GetId ()<<"duplicate: "<< duplicate[socket->GetNode ()->GetId ()][receiveType] <<std::endl;

   //calculate the what??

 }

//------------------first time--------------
else{                 





if(out_distance<=1000 && must_in[socket->GetNode ()->GetId ()][receiveType]==1 && multicast[sourcefrom_id][socket->GetNode ()->GetId ()]==1){
                if(receiveORnot[receiveType]==0){
                        

                        if(Simulator::Now ().GetSeconds ()<5)//for centeral node
                        {SaveArray[0][0]=vec[0];
                        beenReceived[0][receiveType]++;
                        FReceive_once[0][receiveType]++;
                        countarray[0]++;
                        }
                        
                        SaveArray[socket->GetNode ()->GetId ()][countarray[socket->GetNode ()->GetId ()]]=vec[0];
                        beenReceived[socket->GetNode ()->GetId ()][receiveType]++;
                        FReceive_once[socket->GetNode ()->GetId ()][receiveType]++;
       

                        Simulator::Schedule( Seconds (5.0+broadcast_delay), &GenerateTraffic, 
                        packetSize1, numPackets1, interPacketInterval1,socket->GetNode ()->GetId (),SaveArray[socket->GetNode ()->GetId ()][countarray[socket->GetNode ()->GetId ()]]);
                        
                        countarray[socket->GetNode ()->GetId ()]++; 
                        
               }     
        }
}



multicast[sourcefrom_id][socket->GetNode ()->GetId ()]=0;
receiveORnot[receiveType]=0;

//-----------------------------------------gossip end----------------------------------------



if(FReceive_once[socket->GetNode ()->GetId ()][receiveType]==1)
  { 
Delivery_Time[socket->GetNode ()->GetId ()][receiveType]=Simulator::Now ().GetSeconds (); 
      if(socket->GetNode ()->GetId ()!=0)
         {End_Delivery_Time[socket->GetNode ()->GetId ()][receiveType]=Delivery_Time[socket->GetNode ()->GetId ()][receiveType]+1800;}
FReceive_once[socket->GetNode ()->GetId ()][receiveType]++;
  }

if(beenReceived[socket->GetNode ()->GetId ()][receiveType]==1){
   //Record the node first receive time
   FirstReceive[socket->GetNode ()->GetId ()][receiveType]=Simulator::Now ().GetSeconds ();
   InterestingMatch(socket->GetNode ()->GetId (),receiveType);
   total_receive_node[receiveType]++;
   std::cout<<"Tpye: "<<receiveType<<"Total numbers of receive node: "<< total_receive_node[receiveType]<<std::endl; 
   beenReceived[socket->GetNode ()->GetId ()][receiveType]++;
   }


double reset=(0.00001*distance_count);
if(broadcast_delay ==reset){
    broadcast_delay=0.00001;
   }  
  
/*
//check array content
if(Simulator::Now ().GetSeconds ()>125){
 for(int s1=0;s1<total_node;s1++)
  { std::cout<<"User"<<s1<<":"<<std::endl;
    for(int s2=0;s2<100;s2++){
    std::cout<<SaveArray[s1][s2]<<"-";}
    std::cout<<std::endl;
    //std::cout<<"number_of_messages"<<number_of_messages<<std::endl;
  }
}*/

  NS_LOG_UNCOND ("Received one packet!");
}

static void GenerateTraffic (uint32_t pktSize, 
                             uint32_t pktCount, Time pktInterval,int initial_source_node,std::string initial_message)
{
//********************************************************************************
  
	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");

	Ptr<Socket> source = Socket::CreateSocket (c.Get (initial_source_node), tid);
	InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), 80);
	source->SetAllowBroadcast (true);
	source->Connect (remote);

//***********************Random value setting compare to probability****************
	  
	double min = 0.0;
	double max = 1.0;
	Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
	x->SetAttribute ("Min", DoubleValue (min));
	x->SetAttribute ("Max", DoubleValue (max));
	double Random_value = x->GetValue ();


//********************************************************************************
	sourceNodeID=source->GetNode ()->GetId ();
	ADVtype=atoi(initial_message.c_str());

	must_in[0][ADVtype]=1;

//-------------------------------------
	for(int g=0;g<countNeighbor[sourceNodeID][1][ADVtype];g++){
		// std::cout<<"User Neighbor: "<<source->GetNode ()->GetId ()<<"numbers: "<< countNeighbor[source->GetNode ()->GetId ()][1]<<"who: "<<NeighborList[source->GetNode ()->GetId ()][g]<<std::endl;
	}

	Delay_T[sourceNodeID][ADVtype]=CompareDifference(sourceNodeID,NeighborList,previous_duplicate,ADVtype,previous_duplicate_time,countNeighbor[sourceNodeID][1][ADVtype], previous_countNeighbor[source->GetNode ()->GetId ()][1][ADVtype]);
   

	for(int copy=0; copy< countNeighbor[sourceNodeID][1][ADVtype]; copy++){
		Previous_NeighborList[sourceNodeID][copy][ADVtype]=NeighborList[sourceNodeID][copy][ADVtype];
	}

	previous_countNeighbor[sourceNodeID][1][ADVtype]=countNeighbor[sourceNodeID][1][ADVtype];
	previous_duplicate[sourceNodeID][ADVtype]=duplicate[sourceNodeID][ADVtype];
	previous_duplicate_time[sourceNodeID][ADVtype]=Simulator::Now ().GetSeconds ();

//--------------------------------------

	if (pktCount > 0 
		&& Simulator::Now ().GetSeconds () < End_Delivery_Time[sourceNodeID][ADVtype]){ 
		
		GetPosition(c.Get (sourceNodeID),sourceNodeID);;
		//double time_interval=5;
		std::stringstream msg;


		if(sourceNodeID==0){
			msg<<initial_message<<"-"<<sourceNodeID<<"-"<<hopRecord[sourceNodeID][ADVtype];
			//ran++;         
		}else{
			msg<<initial_message<<"-"<<sourceNodeID<<"-"<<hopRecord[sourceNodeID][ADVtype]; 
		}
		
		Ptr<Packet> pkt1 = Create<Packet> ((uint8_t*) msg.str().c_str(),pktSize);

	//-------------gossiping----------------------------------------------

		//-----------ranking----------
		int standard=DataRanking(sourceNodeID,ADVtype);
		std::cout<<"User hop value: "<<hopRecord[sourceNodeID][ADVtype]<<std::endl;
		if(hopRecord[sourceNodeID][ADVtype]<=standard){
			sending_meet[sourceNodeID][ADVtype]=1;
		}

		double Gprobability=forwardProbability(source->GetNode ()->GetId (),ADVtype);
		std::cout<<Simulator::Now ().GetSeconds ()<<" ID: "<<source->GetNode ()->GetId ()<<" "<<Gprobability<<std::endl;
		//int sendornot=getRandom(Gprobability);
		//std::cout<<"Send or not: "<<sendornot<<std::endl;


		Ptr<MobilityModel> mobility2 = c.Get(sourceNodeID)->GetObject<MobilityModel> (); 
		Vector pos3 = mobility2->GetPosition(); 
		double issudist;
		issudist=sqrt(pow((SourceCoordinate-pos3.x),2)+pow((SourceCoordinate-pos3.y),2));

		//std::cout<<"User hop value: "<<hopRecord[source->GetNode ()->GetId ()][ADVtype]<<std::endl;

		if(	Node_position[sourceNodeID]<=1000 
			&& must_in[sourceNodeID][ADVtype]==1 
			&& Gprobability>Random_value 
			&& eachNode_of_messages[sourceNodeID]<Threshold_TN[sourceNodeID] 
			&& sending_meet[sourceNodeID][ADVtype]==1){// in adv.Range

			//&& eachNode_of_messages[source->GetNode ()->GetId ()]<Threshold_TN[source->GetNode ()->GetId ()]
			source->Send(pkt1);
			std::cout<<"Delay Time: "<<Delay_T[sourceNodeID][ADVtype]<<std::endl; 

			hopRecord[sourceNodeID][ADVtype]++;
			// hop_value[ADVtype]++;   

			if(distance_count>=8){    
			//std::cout<<"multicast hello"<<std::endl; 
			}  

			timeSlotofMessages[(int)Simulator::Now ().GetSeconds ()]++;
			eachNode_of_messages[sourceNodeID]++;           
			number_of_messages[ADVtype]++;

			std::cout <<"Node"<<sourceNodeID<< "_X:"<< pos3.x <<"_Y:"<< pos3.y <<"Distance from 0: "<<issudist<<std::endl;

			std::cout<<"Time:"<<Simulator::Now ().GetSeconds ()<<" "<<"Source node id:"<<(sourceNodeID)+1<<" "<<"Sending packet content: "<<(uint8_t*) msg.str().c_str()<<std::endl;
			//std::cout<<"Node: "<<source->GetNode ()->GetId ()<<"Distance: "<<Node_position[source->GetNode ()->GetId ()]<<std::endl;

			Simulator::Schedule (Seconds(Delay_T[sourceNodeID][ADVtype]), &GenerateTraffic, 
			pktSize,pktCount-1, pktInterval,initial_source_node,initial_message);
		}else{   
			std::cout<<"Source: "<<eachNode_of_messages[sourceNodeID]<<"=> "<<Threshold_TN[source->GetNode ()->GetId ()]<<std::endl;

			Simulator::Schedule (Seconds(Delay_T[sourceNodeID][ADVtype]), &GenerateTraffic, 
			pktSize,pktCount-1, pktInterval,initial_source_node,initial_message);
		}
	}else{//-------------------gossip end-----------------------------------------
		source->Close (); 
    }


	for(int LG=0; LG<ListCount[sourceNodeID][ADVtype];LG++){ 
		NeighborList[sourceNodeID][LG][ADVtype]=1001;
	}

	countNeighbor[sourceNodeID][1][ADVtype]=0;
	ListCount[sourceNodeID][ADVtype]=0;  

	sending_meet[sourceNodeID][ADVtype]=0;
	send_candidate=0;
}




void CSVOUTPUT ()
{
	std::ofstream out (CSVfileName.c_str (), std::ios::app);
	//
	out << (Simulator::Now ()).GetSeconds () << "seconds,"
		<< "type1" << ","
		<< "type2" << ","
		<< "type3" << ","
		<< "type4"<< ","
		<< "type5"<< ""
		<< std::endl;

	out.close ();
	Simulator::Schedule (Seconds (2.0), &CSVOUTPUT);
}

int main (int argc, char *argv[])
{
	std::string phyMode ("DsssRate1Mbps");
	double rss = -80;  // -dBm
	uint32_t packetSize = 1000; // bytes 
	uint32_t numPackets = 360;
	double interval =5; // seconds
	bool verbose = false;


	std::string traceFile;
	int    nodeNum;
	double duration;

	// Enable logging from the ns2 helper
	LogComponentEnable ("Ns2MobilityHelper",LOG_LEVEL_DEBUG);

	//NS2
	CommandLine cmd;
	cmd.AddValue ("traceFile", "Ns2 movement trace file", traceFile);
	cmd.AddValue ("nodeNum", "Number of nodes", nodeNum);
	cmd.AddValue ("duration", "Duration of Simulation", duration);
	cmd.Parse (argc,argv);
	if (traceFile.empty () || nodeNum <= 0 || duration <= 0)
	{
		std::cout << "Usage of " << argv[0] << " :\n\n"
		"./waf --run \"ns2-mobility-trace"
		" --traceFile=src/mobility/examples/default.ns_movements"
		" --nodeNum=2 --duration=100.0 --logFile=ns2-mob.log\" \n\n"
		"NOTE: ns2-traces-file could be an absolute or relative path. You could use the file default.ns_movements\n"
		"      included in the same directory of this example file.\n\n"
		"NOTE 2: Number of nodes present in the trace file must match with the command line argument and must\n"
		"        be a positive number. Note that you must know it before to be able to load it.\n\n"
		"NOTE 3: Duration must be a positive number. Note that you must know it before to be able to load it.\n\n";

		return 0;
	}

	// Create Ns2MobilityHelper with the specified trace log file as parameter
	Ns2MobilityHelper ns2 = Ns2MobilityHelper (traceFile);
	//NodeContainer c;
	c.Create (nodeNum);
	ns2.Install ();



	// Convert to time object
	Time interPacketInterval = Seconds (interval);

	// disable fragmentation for frames below 2200 bytes
	Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
	// turn off RTS/CTS for frames below 2200 bytes
	Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
	// Fix non-unicast data rate to be the same as that of unicast
	Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", 
						StringValue (phyMode));

	// The below set of helpers will help us to put together the wifi NICs we want
	WifiHelper wifi;
	if (verbose)
	{
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
	// wifiChannel.AddPropagationLoss ("ns3::FixedRssLossModel","Rss",DoubleValue (-80));
	wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel","MaxRange",DoubleValue (TRANSMISSION_RANGE));//transmission range:(200m)

	wifiPhy.SetChannel (wifiChannel.Create ());

	// Add a non-QoS upper mac, and disable rate control
	NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
								  "DataMode",StringValue (phyMode),
								  "ControlMode",StringValue (phyMode));
	// Set it to adhoc mode
	wifiMac.SetType ("ns3::AdhocWifiMac");
	NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, c);

	InternetStackHelper internet;
	internet.Install (c);

	Ipv4AddressHelper ipv4;
	NS_LOG_INFO ("Assign IP Addresses.");
	ipv4.SetBase ("10.0.0.0", "255.255.240.0");
	Ipv4InterfaceContainer i = ipv4.Assign (devices);

	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");

	int icount;

	for(icount=0; icount<total_node; icount++) 
	{ 
		//std::cout<<"address: "<<i.GetAddress(icount) <<"; "<<std::endl; 
		Ptr<Socket> recvSink = Socket::CreateSocket (c.Get (icount), tid);
		//tid: UdpSocketFactory
		//source come from any IP? NO~ getany return 0.0.0.0
		// InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
		InetSocketAddress local = InetSocketAddress (c.Get (icount)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal(),80);
		recvSink->Bind (local);
		recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));
	}

	for(int LG=0 ;LG<1001;LG++){
		for(int LG2=0 ;LG2<1001;LG2++){
		//Previous_NeighborList[LG][LG2]=1001;
			for(int type=0;type<11;type++)
				NeighborList[LG][LG2][type]=1001;

		}
	}

	Threshold_TN[0]=360;
	Simulator::Schedule( Seconds (0.0), &EnterLeave);
	Simulator::Schedule( Seconds (95.0),&dropcheck);

	int initial_source_node=0;

	std::string initial_message;
	// Output what we are doing
	NS_LOG_UNCOND ("Testing " << numPackets  << " packets sent with receiver rss " << rss );
	//interPacketInterval:Control the packet sending time
	End_Delivery_Time[0][1]=100;

	Userpreference();
	initial_message=ran;
	Simulator::Schedule(   Seconds (0.0), &GenerateTraffic, 
							  packetSize, numPackets, interPacketInterval,initial_source_node,initial_message);

	AnimationInterface anim ("wifi-simple-adhoc.xml");
	//CSVOUTPUT ();
	Simulator::Stop (Seconds (duration));   
	Simulator::Run ();
	Simulator::Destroy ();

	return 0;
}

