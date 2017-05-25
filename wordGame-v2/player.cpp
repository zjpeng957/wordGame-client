#include "player.h"
#include <iostream>
#include <fstream>
using namespace std;

vector<string> player::cLevelRanking = vector<string>();
vector<string> player::dLevelRanking = vector<string>();
vector<string> player::expRanking = vector<string>();
vector<string> player::passRanking = vector<string>();
vector<string> player::puzzleRanking = vector<string>();
vector<int> challenger::levelExp = vector<int>();
vector<int> designer::levelPuzzle = vector<int>();
player::player(const player & p):name(p.name),password(p.password),level(p.level),clgInfo(p.clgInfo),dsInfo(p.dsInfo)
{

}

player & player::operator=(const player & p)
{
	name = p.name;
	password = p.password;
	level = p.level;
	clgInfo = p.clgInfo;
	dsInfo = p.dsInfo;
	
	return *this;
}


void player::rankInit(vector<string> &ranking,string file)
{
	ifstream in(file);
	
	string name;
	while (in>>name)
	{
		ranking.push_back(name);
	}
	in.close();
}

void player::saveRank(vector<string> ranking, string file)
{
	ofstream out(file);

	for (auto n:ranking)
	{
		out << n << endl;
	}
}

inline
challenger::challenger(const challenger & c):player(c),exp(c.exp),pass(c.pass)
{
}

challenger & challenger::operator=(challenger & c)
{
	player::operator=(c);
	exp = c.exp;
	pass = c.pass;
	return *this;
}

challenger::~challenger()
{
}



bool challenger::solve(string puzzle)
{
	string ans;
	cin >> ans;
	if (ans != puzzle) cout << "����" << endl;
	return ans==puzzle;
}

bool challenger::rank(string n, Mode m)
{
	
	if (m == CHALLENGE)
	{
		int lr = 0, pr = 0, er = 0, i = 0;
		/*for (; i < cLevelRanking.size() && (lr == 0 || pr == 0 || er == 0); i++)
		{
			if (cLevelRanking[i] == n) lr = i + 1;
			if (passRanking[i] == n) pr = i + 1;
			if (expRanking[i] == n) er = i + 1;
		}
		if (lr == 0 || pr == 0 || er == 0) return false;*/
		recv(currentSocket, recvbuf, DEFAULT_BUFLEN, 0);
		sscanf(recvbuf, "%d %d %d", &lr, &pr, &er);
		cout << "����\t" << "�ȼ�\t" << "�ؿ�\t" << "����\t" << endl;
		cout << "����\t" << lr << "\t" << pr << "\t" << er << "\t" << endl;
	}
	else
	{
		int lr = 0, wr=0, i = 0;
		/*for (; i < dLevelRanking.size() && (lr == 0 || wr == 0); i++)
		{
			if (dLevelRanking[i] == n) lr = i + 1;
			if (puzzleRanking[i] == n) wr = i + 1;
		}
		if (lr == 0 || wr == 0) return false;*/
		recv(currentSocket, recvbuf, DEFAULT_BUFLEN, 0);
		sscanf(recvbuf, "%d %d", &lr, &wr);
		cout << "����\t" << "�ȼ�\t" << "������\t" << endl;
		cout << "����\t" << lr << "\t" << wr << "\t" << endl;
	}
	return true;
}

bool challenger::showInfo()
{
	char n[20];
	int l, p, e;
	cout << "����\t" << "�ȼ�\t" << "������\t" << "����" << endl;
	
	recv(currentSocket, recvbuf, DEFAULT_BUFLEN, 0);
	sscanf(recvbuf, "%s %d %d %d", n, &l, &p, &e);
	cout << n << "\t" << l << "\t" << p << "\t" << e << "\t" << endl;

	cout << endl;

	rank(n, CHALLENGE);
	cout << "**************************************************************" << endl;
	return true;
}

void challenger::reRank()
{
	attrReRank(player::cLevelRanking, level,CHALLENGE,LEVEL);
	attrReRank(player::passRanking, pass, CHALLENGE,PASS);
	attrReRank(player::expRanking, exp, CHALLENGE,EXP);
}

void challenger::refreshInfo(int d)
{
	exp += d;
	if (level < MAX_LEVEL&&exp >= levelExp[level + 1]) level++;
	pass++;
}

void challenger::addChallenger(string n)
{
	player::cLevelRanking.push_back(n);
	player::passRanking.push_back(n);
	player::expRanking.push_back(n);
}

void challenger::levelExpInit()
{
	levelExp.push_back(0);
	for (int i = 1; i < 100; i++)
	{
		levelExp.push_back(i*5 + levelExp[i - 1]);
	}
}

int player::getAttr(ATTR a)
{
	switch (a)
	{
	case LEVEL:return getLevel();
		break;
	case PASS:return getPass();
		break;
	case EXP:return getExp();
		break;
	case PUZZLE:return getPuzzle();
		break;
	default:return 0;
		break;
	}
}

void player::allRankInit()
{
	player::rankInit(cLevelRanking, "cLevel.txt");
	player::rankInit(dLevelRanking, "dLevel.txt");
	player::rankInit(passRanking, "pass.txt");
	player::rankInit(expRanking, "exp.txt");
	player::rankInit(puzzleRanking, "puzzle.txt");
}

void player::saveAllRank()
{
	saveRank(cLevelRanking, "cLevel.txt");
	saveRank(dLevelRanking, "dLevel.txt");
	saveRank(passRanking, "pass.txt");
	saveRank(expRanking, "exp.txt");
	saveRank(puzzleRanking, "puzzle.txt");
}

void player::attrReRank(vector<string>& ranking, int attr,Mode m,ATTR a)
{
	int i = 0;
	map<string, player*> *user = nullptr;
	if (m == CHALLENGE) user = clgInfo;
	else user = dsInfo;
	while (ranking.size() > i&&ranking[i] != name) i++;
	if (i != ranking.size())
	{
		string next;
		while (i>=1)
		{
			next = ranking[i - 1];
			if (attr < (*user)[next]->getAttr(a)) break;
			swap(ranking[i - 1], ranking[i]);
			i--;
		}
	}
}

bool designer::rank(string n, Mode m)
{
	if (m == CHALLENGE)
	{
		int lr = 0, pr = 0, er = 0, i = 0;
		/*for (; i < cLevelRanking.size() && (lr == 0 || pr == 0 || er == 0); i++)
		{
		if (cLevelRanking[i] == n) lr = i + 1;
		if (passRanking[i] == n) pr = i + 1;
		if (expRanking[i] == n) er = i + 1;
		}
		if (lr == 0 || pr == 0 || er == 0) return false;*/
		recv(currentSocket, recvbuf, DEFAULT_BUFLEN, 0);
		sscanf(recvbuf, "%d %d %d", &lr, &pr, &er);
		cout << "����\t" << "�ȼ�\t" << "�ؿ�\t" << "����\t" << endl;
		cout << "����\t" << lr << "\t" << pr << "\t" << er << "\t" << endl;
	}
	else
	{
		int lr = 0, wr = 0, i = 0;
		/*for (; i < dLevelRanking.size() && (lr == 0 || wr == 0); i++)
		{
		if (dLevelRanking[i] == n) lr = i + 1;
		if (puzzleRanking[i] == n) wr = i + 1;
		}
		if (lr == 0 || wr == 0) return false;*/
		recv(currentSocket, recvbuf, DEFAULT_BUFLEN, 0);
		sscanf(recvbuf, "%d %d", &lr, &wr);
		cout << "����\t" << "�ȼ�\t" << "������\t" << endl;
		cout << "����\t" << lr << "\t" << wr << "\t" << endl;
	}
	return true;
}

void designer::refreshInfo(int d)
{
	puzzle += d;
	if (level < MAX_LEVEL&&puzzle >= levelPuzzle[level + 1]) level++;
}

bool designer::showInfo()
{
	char n[20];
	int l, p;
	cout << "����\t" << "�ȼ�\t" << "������" << endl;
	
	recv(currentSocket, recvbuf, DEFAULT_BUFLEN, 0);
	sscanf(recvbuf, "%s %d %d", n, &l, &p);
	cout << n << "\t" << l << "\t" << p << endl;

	cout << endl;

	rank(n, DESIGN);
	cout << "**************************************************************" << endl;
	return true;
}

void designer::reRank()
{
	attrReRank(dLevelRanking, level,DESIGN,LEVEL);
	attrReRank(puzzleRanking, puzzle, DESIGN,PUZZLE);
}


void designer::addDesigner(string n)
{
	player::dLevelRanking.push_back(n);
	player::puzzleRanking.push_back(n);
}
