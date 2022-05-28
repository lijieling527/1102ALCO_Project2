#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include <cmath>
#include<algorithm>
#include <cstdlib>
#include <iomanip>
using namespace std;


struct predict
{
	string state_table[8] = { "SN","SN","SN" ,"SN", "SN","SN","SN" ,"SN"};
	char table[8] = { 'N','N','N','N', 'N','N','N','N' };
	int bit[3] = { 0,0,0 };
	int missprediction = 0;
};

struct reg
{
	string regname;
	int regvalue;
};

vector<string>PC; //0x000
vector<string>type; //li,addi,beq
vector<string>inst; //R3,0
vector<string>label; //End
vector<reg> regbuffer;
vector <predict> predictor;

int outcome = 0; //0代表 Not taken，1代表Taken
vector<int>PCdex;
string table[4] = { "SN","WN","WT","ST" };

int Base16To10(string pc)
{
	int i = pc.size() - 1;
	int j = 0;
	int newPC = 0;
	while (pc[i] != 'x')
	{
		switch (pc[i]) {
		case 'A':
			newPC += pow(16, j) * 10;
			break;
		case 'B':
			newPC += pow(16, j) * 11;
			break;
		case 'C':
			newPC += pow(16, j) * 12;
			break;
		case 'D':
			newPC += pow(16, j) * 13;
			break;
		case 'E':
			newPC += pow(16, j) * 14;
			break;
		case 'F':
			newPC += pow(16, j) * 15;
			break;
		default:
			int num = pc[i] - '0';
			newPC += pow(16, j) * num;
			break;
		}
		i--;
		j++;
	}
	return newPC;
}


void li(int num)
{
	int end = inst[num].find(',');
	string r = inst[num].substr(0, end);
	reg temp;
	temp.regname = r;
	string imm = inst[num].substr(end + 1, inst[num].size());
	temp.regvalue = stoi(imm);
	regbuffer.push_back(temp);
}
void addi(int num)
{
	int end = inst[num].find(',');
	int immdeate = inst[num].find(',', end + 1);
	string temp = inst[num].substr(immdeate + 1, inst[num].size() - (immdeate + 1));
	string r = inst[num].substr(0, end);
	for (int i = 0; i < regbuffer.size(); i++)
		if (regbuffer[i].regname == r)
			regbuffer[i].regvalue += stoi(temp);
}
int beq(int num)
{
	int n = inst[num].find(',');
	string reg1 = inst[num].substr(0, n);
	int last = inst[num].find(',', n + 1);
	string reg2 = inst[num].substr(n + 1, last - (n + 1));
	string l = inst[num].substr(last + 1, inst[num].size() - 1);
	if (reg1 == reg2)//一樣就taken
	{
		for (int i = 0; i < label.size(); i++)
		{
			if (label[i] == l)//找label
			{
				if (i + 1 == PCdex.size())//end
				{
					outcome = 1;
					return -1;
				}
				outcome = 1;
				return PCdex[i + 1];
			}
		}
	}
	int reg1num = 0, reg2num = 0;
	for (int i = 0; i < regbuffer.size(); i++)
	{
		if (regbuffer[i].regname == reg1)
			reg1num = i;
		if (regbuffer[i].regname == reg2)
			reg2num = i;
	}
	if (regbuffer[reg1num].regvalue != regbuffer[reg2num].regvalue)
	{
		outcome = 0;
		return PCdex[num] + 4;
	}
	for (int i = 0; i < label.size(); i++)
	{
		if (label[i] == l)
		{
			if (i + 1 == PCdex.size())
			{
				outcome = 1;
				return -1;
			}
			outcome = 1;
			return PCdex[i + 1];
		}
	}
}

void ALU(int* num, int* next)
{

	if (type[*num] == "li")
	{
		li(*num);
		outcome = 0;
		*next += 4;
	}
	else if (type[*num] == "addi")
	{
		addi(*num);
		outcome = 0;
		*next += 4;
	}
	else if (type[*num] == "beq")
	{
		*next = beq(*num);
	}

}
void prediction(int outcome, int thispredictor)
{
	char out;
	char pre;

	cout << endl << predictor[thispredictor].bit[0] << predictor[thispredictor].bit[1] << predictor[thispredictor].bit[2] << ' ';

	for (int j = 0; j < 8; j++)
		cout << predictor[thispredictor].state_table[j] << ' ';

	if (outcome == 1)
		out = 'T';

	else
		out = 'N';

	int s = 0;
	string missprediction;
	for (int i = 2, j = 0; i >= 0; i--, j++)
		s += predictor[thispredictor].bit[i] * pow(2, j);
	int state = s;
	int num;
	for (int i = 0; i < 4; i++)
		if (table[i] == predictor[thispredictor].state_table[s])
			num = i;
	if (outcome == 0 && predictor[thispredictor].table[s] == 'N')
	{
		if (predictor[thispredictor].state_table[s] != "SN")
			predictor[thispredictor].state_table[s] = table[num - 1];
		missprediction = "not miss";
		pre = 'N';
	}
	else if (outcome == 0 && predictor[thispredictor].table[s] == 'T')
	{
		if (predictor[thispredictor].state_table[s] == "WT")
			predictor[thispredictor].table[s] = 'N';
		predictor[thispredictor].state_table[s] = table[num - 1];
		missprediction = "miss";
		pre = 'T';
		predictor[thispredictor].missprediction++;
	}
	else if (outcome == 1 && predictor[thispredictor].table[s] == 'T')
	{
		if (predictor[thispredictor].state_table[s] != "ST")
			predictor[thispredictor].state_table[s] = table[num + 1];
		missprediction = "not miss";
		pre = 'T';
	}
	else if (outcome == 1 && predictor[thispredictor].table[s] == 'N')
	{
		if (predictor[thispredictor].state_table[s] != "WN")
			predictor[thispredictor].table[s] = 'T';
		predictor[thispredictor].state_table[s] = table[num + 1];
		missprediction = "miss";
		pre = 'N';
		predictor[thispredictor].missprediction++;
	}
	cout << "entry: " << thispredictor << ' ' << "misprediction: " << predictor[thispredictor].missprediction;
	cout << "    pre: " << pre << "   out:  " << out << "     " << missprediction << endl;
}
void finput(string input)
{
	if (input.find(':') != string::npos)//label, ex: LOOP
	{
		input.erase(remove(input.begin(), input.end(), '\t'), input.end());
		label.push_back(input.substr(0, input.find(':')));
		PC.push_back("");
		type.push_back("");
		inst.push_back("");
	}
	else //no label
	{
		label.push_back("");
		PC.push_back(input.substr(0, 5));
		int x = 5;
		char a = input[x];
		while (a == '\t')
		{
			x += 1;
			a = input[x];
		}
		if (a == 'l')
		{
			type.push_back(input.substr(x, 2));
			x += 3;
		}
		else if (a == 'b')
		{
			type.push_back(input.substr(x, 3));
			x += 4;
		}
		else if (a == 'a')
		{
			type.push_back(input.substr(x, 4));
			x += 5;
		}
		int end = input.find(';');
		string temp = input.substr(x, end - x);
		temp.erase(remove(temp.begin(), temp.end(), ' '), temp.end());
		inst.push_back(temp);
	}
}
int main()
{
	ifstream fin("input.txt");
	string input;
	while (getline(fin, input))
	{
		finput(input);
	}
	for (int i = 0; i < PC.size(); i++)
	{
		int temp = -1;
		PCdex.push_back(temp);
		if (PC[i] != "")
			PCdex[i] = Base16To10(PC[i]);
	}
	int entry_num;
	cout << "Please input entry (entry>0) :" << endl;
	cin >> entry_num;
	while (entry_num <= 0 || (entry_num & (entry_num - 1)))//判斷是否為2的次方數
	{
		cout << "Number of entries is an error!" << endl;
		cout << "Please input entry (entry>0) :" << endl;
		cin >> entry_num;
	}
	for (int i = 0; i < entry_num; i++)
	{
		predict temp;
		predictor.push_back(temp);
	}
	PCdex[PCdex.size() - 1] = -1;
	int next = PCdex[0];//下一個inst pc
	int nextpredictor = -1;
	while (next != -1)
	{
		int num = 0;
		for (; num < PCdex.size(); num++)
		{
			if (PCdex[num] == next)
				break;
		}
		cout << endl << PC[num] << ' ' << type[num] << ' ' << inst[num];
		nextpredictor = (nextpredictor + 1) % entry_num;

		ALU(&num, &next);
		
		prediction(outcome, nextpredictor);
		predictor[nextpredictor].bit[0] = predictor[nextpredictor].bit[1];
		predictor[nextpredictor].bit[1] = predictor[nextpredictor].bit[2];
		predictor[nextpredictor].bit[2] = outcome;
	}
}
