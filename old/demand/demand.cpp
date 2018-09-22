#include <iostream>
#include <fstream>
#include <conio.h>
#include <new>
#include <string>
#include <stdlib.h>
#include <cctype>
#include <stdio.h>
using namespace std;
	int n=0;
	int *pr;
	int *tempb;
	int last=0;
	int lastb=-1;
	double ko;
	char _str[255];
	double koef=0.5;
	bool tr=false;
	bool bl=false;
	string s_="";
	string str="";
	char d;	
inline void add(int num){
	last++;
	pr[last]=num;
}

inline void put_num(int num){
 pr[last]=num;
}

inline int get_last(){
	return pr[last];
}

inline void swap(int &i,int &j){
	int t;
	t=i; i=j; j=t;
}


void null_tempb(int num=n){
	for(int j=0;j<num;j++) 
		tempb[j]=0;
	lastb=0;
}
void null_array(int num=n){
	for(int j=0;j<num;j++) 
		pr[j]=0;
	last=0;
}
void delete_last(){
	if (last>0){
	pr[last]=0;
	last--;
	}
}
void Get_all();
string Stringline();
double Get_Priority();
void StrToTempb();
bool is_in(int key);
bool is_all_in();
void writeF();
void Sort_Operations();



int main(){
  remove("results.txt");
  ifstream input("products.txt");
  if(!input){
	  cout<<"File products.txt opening error..."<<endl;
	  d=_getch();
	  exit(-1);
  }
  while(input){
	  input.getline(_str,255);
   	  n++;
	 }
  input.close();
  cout<<"Was registered "<<--n<<" products..."<<endl;
  try {
	  pr=new int[n];
  }
  catch (bad_alloc xa){
	  cout<<"Memory Error."<<endl;
	  d=_getch();
	  exit(-1);
  }

 try {
	  tempb=new int[n];
  }
  catch (bad_alloc xa){
	  cout<<"Memory Error."<<endl;
	  d=_getch();
	  exit(-1);
  }

  cout<<"Memory was given..."<<endl;

  cout<<"Enter the lowest priority (0.5 or 50 for example): ";
  cin>>koef;
  if (koef>1.0)
	  koef=koef/100;
 null_array();
 null_tempb();
 Sort_Operations();
 register int i=1;
 while(i<n){
	 bl=false;
	 if(last<n && last>0)
	 null_array(last);
	 else
		 null_array();
	 put_num(i);
	 add(i+1);
	 Get_all();
	 if(bl==true)
		 i++;
	 i++;
 }
 cout<<"Done..."<<endl;
 cout<<"Do you want to see results ? (Y/N): ";
 d=_getch();
 cout<<endl;
 if(d=='y'||d=='Y'){
  ifstream in("results.txt");
   if(!in){
		cout<<"File results.txt opening error..."<<endl;
		d=_getch();
		exit(-1);
	}
   while(!in.eof()){
	   in.getline(_str,255);
	   str=Stringline();
	   cout<<str<<endl;
   }
   in.close();
   cout<<"Enter Any key to continue...";
   d=_getch();
 }
 else
 {
	 cout<<endl<<"Results're in file results.txt in same directory."<<endl<<"Enter Any key to continue...";
	 d=_getch();
 }
 delete [] pr;
 delete [] tempb;
 	return 0;
}

void Get_all(){
	int tm=0;
	ko=Get_Priority();
	if (ko>=koef){
		writeF();
	if (get_last()+1<=n){
			add(get_last()+1);
			Get_all();
			return;
		}    //now if last number is n-1::::::::::::::::::
	else {
		delete_last();
		if (get_last()==n-1){
			put_num(n);
				Get_all();
			return;
			}
			else
		if (get_last()<n-1)
		{
			tm=get_last()+1;
			   put_num(tm);
			   bl=true;
			   Get_all();
			   return;
		}

	     return;
	  }
	}         //now if ko<koef:::::::::::::::::
	else {
		if (get_last()<n){
			tm=get_last()+1;
			put_num(tm);
			Get_all();
			return;
		}                       // dali, jaksho get_last()==n:::::::::::::::::::
		else {
			delete_last();
			if (get_last()==n-1){
				put_num(n);
				Get_all();
			return;
			}
			else
		if (get_last()<n-1){
			tm=get_last()+1;
			  put_num(tm);
			   bl=true;
			if(!(last>0))
			  add(tm+1);
			   Get_all();
			   return;
		   }
		else
			return;
		}
		return;
	}            
}
double Get_Priority(){
	int counter=0;
	int ad=0;
	ifstream input("sorted_operations.txt");
	while(!input.eof()){
	    input.getline(_str,255);
		str=Stringline();
		ad++;
		StrToTempb();
		if(tr){
		if(is_all_in())
		  counter++;
		}
		else
		  break;
	}
	input.close();
	if(ad>1){
	ad--;
	return  (double) counter/ad;
	}
	else
		return 0;
}
void StrToTempb(){
	if(isprint(str[0])){
	str+=" ";
	tr=true;
	}
	else {
		tr=false;
		if(lastb>0 && lastb<n)
		null_tempb(lastb);
		return;
	}
   if(lastb>0 && lastb<n)
		null_tempb(lastb);
   else
	   null_tempb();
	string temp_str="";
	for(int i=0;i<str.length();i++){
		if(str[i]!=' ')
			temp_str+=str[i];
		else
		{
			tempb[lastb]=atoi(temp_str.data());
			lastb++;
			temp_str="";
		}
	}
	lastb--;
}

bool is_in(int key){
	int up=lastb;
	int low=0;
	int i=(low+up)/2;
	while (low<=up){
		if(tempb[i]==key)
			return true;
		else
			if(tempb[i]>key)
				up=i-1;
			else
				low=i+1;
		i=(low+up)/2;
	}
	if (low<=up)
		return true;
	else
		return false;
}
bool is_all_in(){
	bool bol=true;
	if(get_last()==0)
		return false;
	for(int i=0;i<=last;i++)
		if(is_in(pr[i])==false)
			bol=false; 
		return bol;
}
void writeF(){
	ofstream out("results.txt",ios::out|ios::app);
	ifstream in("products.txt");
	if(!out){
		cout<<"File results.txt opening error..."<<endl;
		char ch=_getch();
		exit(0);
	}
    if(!in){
		cout<<"File products.txt opening error..."<<endl;
		char ch1=_getch();
		exit(0);
	}
	if(last==0){
		in.close();
		out.close();
		return;
	}
	for(int j=0;j<=last;j++){
		for(int p=1;p<=pr[j];p++)
		 in.getline(_str,255);
		 s_=Stringline();
		 in.seekg(0,ios::beg);
		 out<<s_<<" ";
	}
	out<<" ( "<<ko*100<<"% )"<<endl;
	in.close();
	out.close();
}



void Sort_Operations(){
	ofstream out("sorted_operations.txt");
	ifstream in("buy-things.txt");
    if(!out){
		cout<<"File sorted_operations.txt opening error..."<<endl;
		char ch=_getch();
		exit(0);
	}
    if(!in){
		cout<<"File buy-things.txt opening error..."<<endl;
		char ch1=_getch();
		exit(0);
	}
    
	while(in){
		in.getline(_str,255);
        str=Stringline(); 
		StrToTempb();
		if(tr){
		int low;
		for(int i=0;i<lastb;i++){
			low=i;
			for(int j=i+1;j<lastb+1;j++)
				if(tempb[j]<tempb[low])
					low=j;
			if(i!=low)
				swap(tempb[i],tempb[low]);
		}
		for(int i=0;i<=lastb;i++){
			out<<tempb[i]<<" ";
          }
		out<<endl;
		}
	}
	cout<<"Customers' operations're sorted..."<<endl;
	in.close();
	out.close();
}


string Stringline(){
	string st="";
	int i=0;
	while(!(isprint(_str[i])==0)){
		st+=_str[i];
		i++;
	}
	return st;
}
