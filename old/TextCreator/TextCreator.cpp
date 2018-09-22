#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <list>
#include <vector>
#include <iterator>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
using namespace std;

map<list<string>, map<string, double> > Prob;
int k,n;
int y;
const int PrecCoeff = 50;
int random(int n);
void Analize(const string& filename)
{
	cout<<"Analizing author style...";
	string temp_filename;
	map<list<string>,map<string, int> > Freq;
	map<list<string>, int> Count;
	ifstream file(filename.c_str());
	//if (! file) {
	//	cout<<"             File opening error!            "<<endl;
//		cout<<"Enter new name(path) of(to) source fail(*.txt documents only!): ";
//		cin>>temp_filename;
//	Analize(temp_filename);
//	} 
	list<string> Key;
	string temp;

	for(int i = 0; i < k; i++)
	{ file >> temp; Key.push_back(temp); }
	while (file >> temp)
	{
		Count[Key]++;
	Freq[Key][temp]++;
	Key.push_back(temp);
	Key.pop_front();
}
	for (map<list<string>, map<string, int>>::iterator p=Freq.begin();p !=Freq.end();p++)
              for (map<string, int>::iterator q = p->second.begin();q !=p->second.end();q++)
				  Prob[p->first][q->first] = q->second/double(Count[p->first]);
	cout<<"Analized...";

}
string GetRandomWord(const map<string, double>& wordtable)
{
	vector<string> words;
	for (map<string, double>::const_iterator p = wordtable.begin(); p!= wordtable.end(); p++)
		fill_n(back_inserter(words), PrecCoeff*p->second,p->first);
	if (words.empty())
	{cout<<"It is not possible to build current word!";exit(0);}
	return words[random(words.size())];
}
int main(int argc, char* argv[])
{
int state; 
	FILE *in,*out;
	int num;
   ofstream out1("New_Text.txt");
	string filename;
	char flnm[81]; 
	srand( (unsigned)time( NULL ) );

	cout<<"Enter text coherency koeficient (Recomended : 1 , 2 or 3): ";
	cin>>k;
	cout<<"Enter number of words in new (created) text: ";
	cin>>n;
	cout<<"Enter a number of source failes: ";
	cin>>num;
	for(int j=0; j<num; j++){
      cout<<"Enter name(path) of(to) source fail(*.txt documents only!): ";
     cin>>filename;
	 for(int i=0;i<filename.size();i++)
	flnm[i+1]=filename[i+1];
	 flnm[0]=filename[0];

	if((in = fopen(flnm,"rt"))==NULL)
	 { cout<<"File "<<flnm<<" opening error!"<<endl;
//  if(j>0)
	j--;
	continue;
	 }
  state=1;
  cout<<"Ok.\n";
 if(j==0 && state==1) {
	
 if ((out=fopen("Huge_Source.txt","w+t"))==NULL)
	 { cout<<"File opening error!";
	  j-=1;
	 continue;
 }
	 goto qwer;
}


 if ((out=fopen("Huge_Source.txt","a+t"))==NULL)
	 { cout<<"File opening error!";
	  j-=1;
	 continue;
	 }
qwer:
   fprintf(out,"\n");
	 while(!feof(in))
         fputc(fgetc(in),out);
	 fclose(in);
	 fclose(out);
	}
    
	Analize("Huge_Source.txt");

	map<list<string>, map<string, double>>::iterator start = Prob.begin();
	advance(start, random(Prob.size()));
	
	list<string> CurKey = start->first;
	copy(CurKey.begin(),CurKey.end(), ostream_iterator<string>(cout, " "));
    for (unsigned i = 0; i < n-k;i++)	{
	string temp = GetRandomWord(Prob[CurKey]);
	CurKey.push_back(temp);
	CurKey.pop_front();
    out1<<temp<<" ";
	cout<<temp<<" ";
}
out1.close();
return 0;
}
int random(int n){
 return rand()%n;
}
					 

