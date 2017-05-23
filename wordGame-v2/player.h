#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;
const int EXP_PER_PASS = 100;
const int MAX_LEVEL = 100;
enum Mode { CHALLENGE, DESIGN, FAIL };
enum ATTR {LEVEL,PASS,EXP,WORD};
//闯关者和出题者共有的抽象基类
class player {
public:
	player():level(0),clgInfo(nullptr),dsInfo(nullptr){}
	player(const player& p);
	player& operator=(const player& p);
	player(string n, string pw,int l=0) :name(n), password(pw),level(l), clgInfo(nullptr), dsInfo(nullptr) {}
	virtual ~player()
	{

	}

	int getLevel() const { return level; }
	string getName () const { return name; }
	string getPw() const { return password; }
	virtual int getPass() const { return 0; }
	virtual int getExp() const { return 0; }
	virtual int getWord() const { return 0; }
	void getAllUser(map<string, player*> *pclg, map<string, player*> *pds) { clgInfo = pclg, dsInfo = pds; }
	virtual bool rank(string n,Mode m) = 0;
	virtual bool showInfo() = 0;
	//virtual void levelInc(int exp) = 0;
	virtual void reRank() = 0;
	virtual void refreshInfo(int d) = 0;
	virtual int getAttr(ATTR a);
	virtual bool solve(string word) { return true; }
	virtual string design(vector<string> &vocabulary) { return string(); }
	static void allRankInit();
	static void saveAllRank();
protected:
	string name;
	string password;
	void attrReRank(vector<string> &ranking,int attr,Mode m,ATTR a);
	map<string, player*> *clgInfo;
	map<string, player*> *dsInfo;
	int level;
	static vector<string> cLevelRanking;
	static vector<string> dLevelRanking;
	static vector<string> expRanking;
	static vector<string> passRanking;
	static vector<string> wordRanking;
	static void rankInit(vector<string> &ranking, string file);
	static void saveRank(vector<string> ranking, string file);
};

class challenger:public player
{
public:
	challenger():pass(0), exp(0) {}
	challenger(const challenger &c);
	challenger& operator=(challenger &c);
	challenger(string n,string pw, int l=0,int p=0,int e=0):pass(p),exp(e),player(n,pw,l){}
	virtual ~challenger();

	int getPass() const { return pass; }
	int getExp() const { return exp; }
	bool solve(string word);
	bool rank(string n,Mode m);
	bool showInfo();
	void reRank();
	//void levelInc(int dExp);
	void refreshInfo(int d);
	static void addChallenger(string n);
	static void levelExpInit();
private:
	
	int exp;
	int pass;

	static vector<int> levelExp;
	
};


class designer : public player
{
public:
	designer();
	designer(const designer &d);
	designer& operator=(const designer &d);
	designer(string n, string pw, int l = 0, int w = 0) :word(w), player(n, pw, l) {  }
	~designer();
	static void levelWordInit();
	string design(vector<string> &vocabulary) { return string(); }
	bool rank(string n, Mode m);

	void refreshInfo(int d);
	bool showInfo();
	void reRank();

	int getWord() const { return word; }
	static void addDesigner(string n);
private:
	int word;
	static vector<int> levelWord;
	
};

inline
designer::designer()
{
}

inline designer::designer(const designer & d):player(d),word(d.word)
{

}

inline designer & designer::operator=(const designer & d)
{
	player::operator=(d);
	word = d.word;
	return *this;
}

inline
designer::~designer()
{
}

inline void designer::levelWordInit()
{
	levelWord.push_back(0);
	for (int i = 1; i < 100; i++)
	{
		levelWord.push_back(i+levelWord[i-1]);
	}
}
