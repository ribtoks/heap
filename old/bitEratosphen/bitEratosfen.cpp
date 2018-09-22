#include <iostream>
#include <conio.h>
#include <iomanip>
#include <string>
using namespace std;

int main()
{
	int n, m, i, bit;
	unsigned char *bytes;
	cin >> n;
	m = n*10;
	try
	{
		bytes = new unsigned char [m];
	}
	catch(bad_alloc xa)
	{
		cout << "bad_alloc...";
		return -1;
	}
	for (i = 0; i < m; i++)
		bytes[i] = 255;
  /*      string s="";
	for(int j=7;j>=0;j--){
		if(!(bytes[i]&(1<<j)))
			s+="1";
		else
			s+="0";
		}
	cout<<s<<" ";*/
	
		
	for (i = 0; i < m; i++)
		for (bit = 7; bit >= 0; bit--)
			if( (bytes[i] | (1<<bit)) == bytes[i])
			{
				if(i == 0)
				for (int newbit = (bit - (2 + 8*i + 7 - bit)); newbit >= 0;newbit--)
					if((2 + 8*i + 7 - newbit)%(2 + 8*i + 7 - bit) == 0)
						bytes[i] = bytes[i] & (~(1 << newbit) );

				for (int j = i + 1; j <= ((m - 1)/(i + 1)); j += ((int)((i - 1)/8) + 1))
					for (int bbit = 7; bbit >= 0; bbit--)
						if((2 + 8*j + 7 - bbit)%(2 + 8*i + 7 - bit) == 0)
							bytes[j] = bytes[j] & (~(1 << bbit) );
			}

/*		 int counter = 0; bit = 7; i = 0;
			 for (i = 0; i < m; i++)
               for (bit = 7; bit >= 0; bit--)
					if((bytes[i] | ( (1 << bit))) == bytes[i])
						if(counter == n)
							goto out;
						else
							counter++;
*/
			
			
 //	all numbers
			int counter = 0; 
			bit = 7; i = 0;
			while (counter <= n  &&  i < m )
			{
				for (bit = 7; bit >= 0; bit--)
					if( (bytes[i] | (1 << bit)) == bytes[i])
					{
						cout << " " << 2 + 8*i + 7 - bit;
						counter++;
					}
					i++;
			}

out: 
//	 cout << 2 + 8*i + 7 - bit;				
     delete [] bytes;
	 _getch();
return 0;
}