#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <bitset>
#include <cmath>
using namespace std;

string state[8] = { "SN ","WN1","WN2","WN3","WT1","WT2","WT3","ST " };
string history = "000";
int counter[8] = {0,0,0,0,0,0,0,0};

int main() {
	string TN;
	cout << "please input: ";

	cin >> TN;		// TTTTNNTNTNTNTNTNT

	cout << "\nround"<< "        Predictor state" 
		<< setw(25) << "outcome" << setw(13) 
		<< "prediction" << setw(13) << "       miss / not miss" 
		<< setw(5) << endl;

	for (int i = 0; i < TN.size(); i++)
	{
		cout << setw(2) << i<<"       ";
		int count = 0;
		for (int j = i - 3; j <= i - 1; j++,count++) {
			if (j < 0)
				history[count] = '0';
			else if (TN[j] == 'T')
				history[count] = '1';
			else
				history[count] = '0';
			
		}
		cout << history << " ";

		for (int j = 0; j <= 7; j++) {
			cout << state[counter[j]] << " ";
		}
		cout << setw(5) << TN[i] << setw(11);

		int n = (int(history[0]) - 48) * 4 + (int(history[1]) - 48) * 2 + (int(history[2]) - 48) * 1;

		if (counter[n] == 0 || counter[n] == 1 || counter[n] == 2 || counter[n] == 3) {
			cout << "N";
			if (TN[i] == 'N') {
				cout << setw(15) << "               not miss";
				if (counter[n] > 0)
					counter[n]--;
			}
			else {
				cout << setw(13) << "               miss";
				counter[n]++;
			}
		}
		else {
			cout << "T";
			if (TN[i] == 'T') {
				cout << setw(15) << "               not miss";
				if (counter[n] < 7)
					counter[n]++;
			}
			
		}
		cout << endl;
	}
}