#include<iostream>
#include "sv.h"

int main(int argc, char * argv[])
{

	if(argc<3)
	{
	cerr<<"Usage: "<<argv[0]<<" foo.mgaps mumTobed.Chr.bed chrom"<<endl;
	exit(EXIT_FAILURE);
	}
	ifstream fin;
	ofstream fout;

	fin.open(argv[1]);
	fout.open(argv[2]);
	
	string str,name,temp,outFileName;
	int ref_st1,ref_st2,ref_end,aln_len,q_st1,q_st2,q_end,ctMum,totAln;
	totAln =0;
	mgapC cluster;
	int count_mum =0;
	map<int,vector<int>> Mref; // the first element would be the start coordinate of a master cluster
	map<int,vector<int>> Qref; //the first element would be the start and the 2nd elements would be master query
	map<int,string> ref_name;
	map<int,string> q_name;
cout<<"Reading mgap alignment.."<<endl;
	while(getline(fin,str))
	{
		if(str[0] == '>')
		{
			//name=str.substr(1,str.find('n')); //remove > from the string
			name = str.substr(1,(str.size()-6));
			//cout<<name<<"\t"<<name.size()<<endl;
			
		}
		ctMum =0; //this is the count of mums within a cluster.reset it here.

		while(str[0] != '#' && str[0] != '>' && !fin.eof())
		{
//cout<<q_st2<<endl;
			if(ctMum ==0)  //the first match.the only match when a cluster has a single match
			{
				temp = xtractcol(str,'\t',1);
				ref_st1 = stoi(temp,nullptr);
				temp = xtractcol(str,'\t',3);
				aln_len = stoi(temp,nullptr);
				temp = xtractcol(str,'\t',2);
				q_st1 = stoi(temp,nullptr);		
				totAln = aln_len;
			}
			else
			{
				temp = xtractcol(str,'\t',1);
                       		ref_st2 = stoi(temp,nullptr);
				temp = xtractcol(str,'\t',3);
				aln_len = stoi(temp,nullptr);
				temp = xtractcol(str,'\t',2);
				q_st2 = stoi(temp,nullptr);
				totAln = aln_len+totAln;
			}	
		ctMum++; //counts total mums inside a cluster		
		if(!fin.eof())
		{
			getline(fin,str); //this will also read the name of the next seq
		}
		}
		if(ctMum == 1) //if there is only one mum in the cluster
		{
			ref_end = ref_st1 + aln_len;
			q_end = q_st1 + aln_len;
		}
		if(ctMum >1) //if there are more than one mum in the cluster
		{
			ref_end = ref_st2 + aln_len;
			q_end = q_st2 + aln_len;
		}
		if(totAln < 100000000)
		{
			fout<<string(argv[3])<<'\t'<<ref_st1<<'\t'<<ref_end<<'\t'<<q_st1<<'\t'<<q_end<<'\t'<<name<<'\t'<<totAln<<'\t'<<(q_end-q_st1)<<endl;
			cluster.refName[count_mum] = string(argv[3]);
			cluster.qName[count_mum] = name;
			cluster.refClust[count_mum].push_back(ref_st1);
			cluster.refClust[count_mum].push_back(ref_end);
			cluster.qClust[count_mum].push_back(q_st1);
			cluster.qClust[count_mum].push_back(q_end);
			cluster.len[count_mum].push_back(totAln);
			cluster.len[count_mum].push_back(q_end-q_st1);
			count_mum++;
		}
		if(str[0] == '>' && !fin.eof()) //this is to extract the name when getline reads a seq names within the second while loop
		{
			//name=str.substr(1); //remove > from the string
			name=str.substr(1,(str.size()-6)); 
	        }

		ref_st1= 0; //resetting them so that they print 0 when an alignment is absent
		ref_end = 0;
		q_st1 = 0;
		q_end = 0;
		totAln = 0;
	}
	fout.close();
	fin.close();
cout<<"Finished converting mgap to bed conversion"<<endl;

	name = string(argv[3]);
cout<<"Extracting abnormal clusters"<<endl;
	comparClust(cluster);
cout<<"Finished recording the abnormal clusters"<<endl;
	outFileName = "SV_report."+ name +".tsv";
//cout<<"Filtering false positives now..."<<endl;
	//removeExactDups(cluster);
	
	//fout<<"Refchr"<<"\t"<<"start"<<"\t"<<"end"<<"\t"<<"DupChr1"<<"\t"<<"start1"<<"\t"<<"end1"<<"\t"<<"DupChr2"<<"\t"<<"start2"<<"\t"<<"end2"<<"\t"<<"Copies"<<endl;
//cout<<"Finished filtering false positives. Now writing the SV in tsv format"<<endl;
	fout.open(outFileName.c_str());
	for(unsigned int k=0;k<cluster.dupCord.size();k++)
	{
		if(cluster.dupCord[k][0] != 0)
		{
			if(cluster.dupName[k][1] == cluster.dupName[k][2])
			{
				fout<<cluster.dupName[k][0]<<"\t"<<cluster.dupCord[k][0]<<"\t"<<cluster.dupCord[k][1]<<"\t"<<cluster.dupName[k][1]<<"\t"<<cluster.dupCord[k][2]<<"\t"<<cluster.dupCord[k][3]<<"\t"<<cluster.dupName[k][2]<<"\t"<<cluster.dupCord[k][4]<<"\t"<<cluster.dupCord[k][5]<<"\t"<<cluster.dupCord[k][6]<<"\t"<<cluster.dupCord[k][7]<<"\t"<<"TD"<<endl;
			}
			if(cluster.dupName[k][1] != cluster.dupName[k][2])
			{
				fout<<cluster.dupName[k][0]<<"\t"<<cluster.dupCord[k][0]<<"\t"<<cluster.dupCord[k][1]<<"\t"<<cluster.dupName[k][1]<<"\t"<<cluster.dupCord[k][2]<<"\t"<<cluster.dupCord[k][3]<<"\t"<<cluster.dupName[k][2]<<"\t"<<cluster.dupCord[k][4]<<"\t"<<cluster.dupCord[k][5]<<"\t"<<cluster.dupCord[k][6]<<"\t"<<cluster.dupCord[k][7]<<"\t"<<"NTD"<<endl;
			}
		}
	}
	fout.close();



return 0;
}
