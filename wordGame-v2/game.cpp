#include "game.h"
#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <ctime>
using namespace std;

game::game():currentPlayer(nullptr),mode(FAIL),ConnectSocket(INVALID_SOCKET)
{
	loadChallenger();
	
	loadDesigner();

	loadVocabulary();
	player::allRankInit();
	challenger::levelExpInit();
	designer::levelWordInit();
}

game::~game()
{
	player::saveAllRank();
	saveChallenger();
	saveDesigner();
	saveVocabulary();
	auto cbegin = ChallengerInfo.begin(), cend = ChallengerInfo.end();
	while (cbegin != cend) { delete cbegin->second; cbegin++; }
	auto dbegin = designerInfo.begin(), dend = designerInfo.end();
	while (dbegin != dend) { delete dbegin->second; dbegin++; }
}

void game::run()
{
	connectSucceed == ConnectServer();
	cout << "*************************************************************" << endl;
	cout << "*                                                           *" << endl;
	cout << "*                     单 词 消 除 游 戏                     *" << endl;
	cout << "*                                                           *" << endl;
	cout << "*************************************************************" << endl;
	cout << endl;
	
	int option=userIn();
	while (option != QUIT)
	{
		if (mode == CHALLENGE) option = challengeUi();
		else if (mode == DESIGN) option = designUi();

		if (option == LOGOUT) option = userIn();
	}
	disconnect(connectSucceed);
}

Mode game::Login()
{
	string name, pw;
	string type;
	map<string, player*> *user = nullptr;
	bool log = false,valid=false;

	cout << "选择登录类型(0.闯关\t1.出题\t2.返回):";
	cin >> type;
	while (type != "0"&&type != "1"&&type != "2")
	{
		cout << "选择登录类型(0.闯关\t1.出题\t2.返回):";
		cin >> type;
	}

	//------------------------------------------
	send(ConnectSocket, type.c_str(), type.size(),0);
	//recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
	//type = string(recvbuf);
	//--------------------------------------------

	if (type == "2") return FAIL;
	else if (stoi(type) == CHALLENGE) user = &ChallengerInfo;
	else user = &designerInfo;
	cout << "请输入用户名(输入\"r\"返回)：";
	cin >> name;
	send(ConnectSocket, name.c_str(), name.size(), 0);
	while (name!="r")
	{
		/*auto target = (*user).find(name);
		if (target != (*user).end())
		{
			cout << "请输入密码：";
			cin >> pw;
			while (pw!="r"&&pw != target->second->getPw())
			{
				cout << "密码错误！" << endl;
				cout << "请输入密码(输入\"r\"返回)：";
				cin >> pw;
			}
			if (pw == "r") name = "r";
			else {
				currentPlayer = target->second;
				currentPlayer->getAllUser(&ChallengerInfo,&designerInfo);
				return Mode(stoi(type));
			}
		}
		else
		{
			cout << "找不到该用户！" << endl;
			cout << "请输入用户名(输入\"r\"返回)：";
			cin >> name;
		}*/
		recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		valid = static_cast<bool>(recvbuf);
		if (valid)
		{
			cout << "请输入密码：";
			cin >> pw;
			send(ConnectSocket, pw.c_str(), pw.size(), 0);
			while (pw != "r")
			{
				recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
				log = static_cast<bool>(recvbuf);
				if (log) return Mode(stoi(type));
				else
				{
					cout << "密码错误！" << endl;
					cout << "请输入密码(输入\"r\"返回)：";
					cin >> pw;
					send(ConnectSocket, pw.c_str(), pw.size(), 0);
					log = static_cast<bool>(recvbuf);
				}
			}
		}
		else
		{
			cout << "找不到该用户！" << endl;
			cout << "请输入用户名(输入\"r\"返回)：";
			cin >> name;
			send(ConnectSocket, name.c_str(), name.size(), 0);
			valid = static_cast<bool>(recvbuf);
		}
	}
	return FAIL;
}

Mode game::Register()
{
	string name, pw;
	string type;
	bool valid = false, reg = false;
	map<string, player*> *user = nullptr;

	cout << "选择注册类型(0.闯关\t1.出题\t2.返回):";
	cin >> type;
	while (type != "0"&&type != "1"&&type != "2")
	{
		cout << "选择注册类型(0.闯关\t1.出题\t2.返回):";
		cin >> type;
	}
	send(ConnectSocket, type.c_str(), type.size(), 0);
	if (type == "2") return FAIL;
	else if (stoi(type) == CHALLENGE) user = &ChallengerInfo;
	else user = &designerInfo;

	cout << "请输入用户名(输入\"r\"返回)：";
	cin >> name;
	send(ConnectSocket, name.c_str(), name.size(), 0);
	while (name != "r")
	{
		recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		valid = static_cast<bool>(recvbuf);
		/*auto target = (*user).find(name);
		if (target == (*user).end())
		{
			cout << "请输入密码：";
			cin >> pw;
			while (pw.size() == 0)
			{
				cout << "请输入密码(输入\"r\"返回)：";
				cin >> pw;
			}
			if (pw == "r") name = "r";
			else {
				player *new_player = nullptr;
				if (stoi(type) == CHALLENGE)
				{
					new_player = new challenger(name, pw);
					challenger::addChallenger(name);
				}
				else
				{
					new_player = new designer(name, pw);
					designer::addDesigner(name);
				}
				(*user).insert(make_pair(name, new_player));
				currentPlayer = new_player;
				currentPlayer->getAllUser(&ChallengerInfo,&designerInfo);
				return Mode(stoi(type));
			}
		}
		else
		{
			cout << "用户已存在！" << endl;
			cout << "请输入用户名(输入\"r\"返回)：";
			cin >> name;
		}*/
		if (valid)
		{
			cout << "请输入密码：";
			cin >> pw;
			send(ConnectSocket, pw.c_str(), pw.size(), 0);
		}
	}
	return FAIL;
}

int game::userIn()
{
	string option;
	cout << "请选择：0.登录\t1.注册\t2.退出" << endl;
	cin >> option;
	int op = 0;
	while (mode == FAIL&&op != QUIT)
	{
		while (option!="0"&&option != "1"&&option != "2")
		{
			cout << "请输入正确的选项：0.登录\t1.注册\t2.退出" << endl;
			cin >> option;
		}
		if (option == "2") op = QUIT;
		else op = stoi(option);

		switch (op)
		{
		case LOGIN:
			cout << "************************登 录**************************" << endl;
			mode = Login();
			break;
		case REGISTER:
			cout << "*************************注 册*************************" << endl;
			mode = Register();
			break;
		case QUIT:
			break;
		default:
			break;
		}
	}
	return op;
}

int game::challengeUi()
{
	string choice;
	cout << "*************************闯 关 者 界 面*************************" << endl;
	cout << "请选择：0.开始游戏\t1.查看排名\t2.查找用户\t3.注销\t4.退出" << endl;
	cin >> choice;
	while (choice!="3"&&choice!="4")
	{
		if (choice == "0") challenge();
		else if (choice == "1") rank();
		else if (choice == "2") search();

		cout << "请选择：0.开始游戏\t1.查看排名\t2.查找用户\t3.注销\t4.退出" << endl;
		cin >> choice;
	}
	if (choice == "3")
	{
		mode = FAIL;
		return LOGOUT;
	}
	else if (choice == "4") return QUIT;
	cout << "******************************************************" << endl;
}

void game::challenge()
{
	system("cls");

	string currentWord;
	auto begin = vocabulary.begin(), end = vocabulary.end();
	int pass = 0;
	int duration = 10000;
	default_random_engine e(time(0));
	uniform_int_distribution<unsigned> u(0, 100);
	
	string again = "y";
	while (again=="y")
	{
		
		cout << "************************第 " << pass + 1 << " 关************************" << endl;
		currentWord = begin->second[u(e) % begin->second.size()];
		cout << currentWord << endl;
		_sleep(duration);
		system("cls");

		cout << "************************第 " << pass + 1 << " 关************************" << endl;
		cout << "输入答案:" << endl;
		while (currentPlayer->solve(currentWord))
		{
			pass++;
			begin++;
			if (begin == end) break;

			currentPlayer->refreshInfo(((pass + 1) % 5 + 1) * 10);
			currentPlayer->reRank();
			currentWord = begin->second[u(e) % begin->second.size()];
			system("cls");

			cout << "************************第 " << pass + 1 << " 关************************" << endl;
			cout << currentWord << endl;
			if ((pass - 1) % 5 == 0 && duration >= 1000) duration -= 500;
			_sleep(duration);
			system("cls");
			cout << "************************第 " << pass + 1 << " 关************************" << endl;
			cout << "输入答案:" << endl;
		}
		if (begin == end) cout << "恭喜全部通关！" << endl;
		currentPlayer->showInfo();

		cout << "再来一次？y/n" << endl;
		cin >> again;
		while (again!="y"&&again!="n")
		{
			cout << "y/n?" << endl;
			cin >> again;
		}
		pass = 0;
		duration = 0;
		begin = vocabulary.begin();
	}

	cout << "****************************************************" << endl;
}

void game::rank()
{
	cout << "*************************排 名*************************" << endl;
	cout << "请选择：0.我的排名\t1.他人排名\t2.返回" << endl;
	string choice;
	cin >> choice;
	while (choice!="2")
	{
		if (choice=="0")
		{
			currentPlayer->rank(currentPlayer->getName(),mode);
		}
		else if(choice=="1")
		{
			string name;
			string type;
			cout << "类型(0.闯关者\t1.出题者\t2.返回):";
			cin >> type;
			while (type != "0"&&type != "1"&&type != "2")
			{
				cout << "类型(0.闯关者\t1.出题者\t2.返回):";
				cin >> type;
			}
			if (type != "2")
			{
				cout << "用户名:";
				cin >> name;
				if (!currentPlayer->rank(name, Mode(stoi(type)))) cout << "用户不存在！" << endl;
			}
		}
		cout << "请选择：0.我的排名\t1.他人排名\t2.返回" << endl;
		cin >> choice;
	}
	cout << "******************************************************" << endl;
}

void game::search()
{
	string choice;
	string attr;
	string aChoice;
	cout << "**********************查 找 用 户**********************" << endl;
	cout << "类型(0.闯关者\t1.出题者\t2.返回):";
	cin >> choice;
	while (choice!="2")
	{
		if (choice == "0")
		{
			cout << "属性(0.姓名\t1.等级\t2.闯关数\t3.经验):";
			cin >> aChoice;
			while (aChoice != "0"&&aChoice != "1"&&aChoice != "2"&&aChoice != "3")
			{
				cout << "请输入属性(0.姓名\t1.等级\t2.闯关数\t3.经验):";
				cin >> aChoice;
			}
			switch (stoi(aChoice))
			{
			case 0:
				cout << "姓名:";
				cin >> attr;
				searchChallenger(attr, -1, -1, -1);
				break;
			case 1:
				cout << "等级:";
				cin >> attr;
				searchChallenger(string(), stoi(attr), -1, -1);
				break;
			case 2:
				cout << "闯关数:";
				cin >> attr;
				searchChallenger(string(), -1, stoi(attr), -1);
				break;
			case 3:
				cout << "经验:";
				cin >> attr;
				searchChallenger(string(), -1, -1, stoi(attr));
				break;
			default:
				break;
			}
		}
		else if (choice == "1")
		{
			cout << "属性(0.姓名\t1.等级\t2.出题数):";
			cin >> aChoice;
			while (aChoice != "0"&&aChoice != "1"&&aChoice != "2")
			{
				cout << "请输入属性(0.姓名\t1.等级\t2.出题数):";
				cin >> aChoice;
			}
			switch (stoi(aChoice))
			{
			case 0:
				cout << "姓名:";
				cin >> attr;
				searchDesigner(attr, -1, -1);
				break;
			case 1:
				cout << "等级:";
				cin >> attr;
				searchDesigner(string(), stoi(attr), -1);
				break;
			case 2:
				cout << "出题数:";
				cin >> attr;
				searchDesigner(string(), -1, stoi(attr));
				break;
			default:
				break;
			}
		}
		cout << "类型(0.闯关者\t1.出题者\t2.返回):";
		cin >> choice;
	}
	cout << "******************************************************" << endl;
}

void game::searchChallenger(string name, int level, int exp, int pass)
{
	auto begin = ChallengerInfo.begin(), end = ChallengerInfo.end();
	bool flag = false;
	if (name != string())
	{
		while (begin!=end)
		{
			if (begin->first == name)
			{
				begin->second->showInfo();
				flag = true;
				return;
			}
			else
			{
				begin++;
			}
		}
	}
	else if(level!=-1)
	{
		while (begin!=end)
		{
			if (begin->second->getLevel() == level)
			{
				begin->second->showInfo();
				flag = true;
			}
			begin++;
		}
	}
	else if (pass != -1)
	{
		while (begin != end)
		{
			if (begin->second->getPass() == pass)
			{
				begin->second->showInfo();
				flag = true;
			}
			begin++;
		}
	}
	else if (exp != -1)
	{
		while (begin != end)
		{
			if (begin->second->getExp() == exp)
			{
				begin->second->showInfo();
				flag = true;
			}
			begin++;
		}
	}
	if (!flag) cout << "用户不存在！" << endl;
}

void game::searchDesigner(string name, int level, int word)
{
	auto begin = designerInfo.begin(), end = designerInfo.end();
	bool flag = false;
	if (name != string())
	{
		while (begin != end)
		{
			if (begin->first == name)
			{
				begin->second->showInfo();
				flag = true;
				return;
			}
			else
			{
				begin++;
			}
		}
	}
	else if (level != -1)
	{
		while (begin != end)
		{
			if (begin->second->getLevel() == level)
			{
				begin->second->showInfo();
				flag = true;
			}
			begin++;
		}
	}
	else if (word != -1)
	{
		while (begin != end)
		{
			if (begin->second->getWord() == word)
			{
				begin->second->showInfo();
				flag = true;
			}
			begin++;
		}
	}
	if (!flag) cout << "用户不存在！" << endl;
}

int game::designUi()
{
	string choice;
	cout << "*************************出 题 者 界 面*************************" << endl;
	cout << "请选择：0.出题\t1.查看排名\t2.查找用户\t3.注销\t4.退出" << endl;
	cin >> choice;
	while (choice != "4" && choice != "3")
	{
		if (choice == "0") design();
		else if (choice == "1") rank();
		else if (choice == "2") search();

		cout << "请选择：0.出题\t1.查看排名\t2.查找用户\t3.注销\t4.退出" << endl;
		cin >> choice;
	}
	if (choice == "3")
	{
		mode = FAIL;
		return LOGOUT;
	}
	else if (choice == "4") return QUIT;
}

void game::design()
{
	system("cls");
	string word;
	int flag = 1;
	cout << "*************************出 题*************************" << endl;
	cout << "输入要添加的新词（输入!q停止出题）:";
	cin >> word;
	while (word != "!q")
	{
		auto &list = vocabulary[word.size()];
		if (list.size() == 0)
		{
			list.push_back(word);
		}
		else
		{
			for (int i = 0; i < list.size()&&flag!=0; i++)
			{
				if (list[i] == word) flag = 0;
			}
		}
		if (flag)
		{
			cout << "添加成功！" << endl;
			currentPlayer->refreshInfo(1);
			currentPlayer->reRank();
		}
		else cout << "单词已存在！" << endl;

		cout << "输入要添加的新词（输入!q停止出题）:";
		cin >> word;
	}
	system("cls");
	currentPlayer->showInfo();
}

void game::loadChallenger()
{
	ifstream in("challenger.txt");
	string name, password;
	int level, pass, exp;
	while (in>>name)
	{
		in >> password >> level >> pass >> exp;
		ChallengerInfo[name] = new challenger(name, password, level, pass, exp);
	}
	in.close();
}

void game::loadDesigner()
{
	ifstream in("designer.txt");
	string name, password;
	int level, word;
	while (in>>name)
	{
		in >> password >> level >> word;
		designerInfo[name] = new designer(name, password, level, word);
	}
	in.close();
}

void game::loadVocabulary()
{
	ifstream in("vocabulary.txt");
	string word;
	int l = 0;
	while (in>>word)
	{
		if (word.size() > l) l = word.size();
		vocabulary[l].push_back(word);
	}
}

void game::saveChallenger()
{
	ofstream out("challenger.txt");
	auto begin = ChallengerInfo.begin(), end = ChallengerInfo.end();

	while (begin!=end)
	{
		out << begin->second->getName() << " " << begin->second->getPw() << " " << begin->second->getLevel()<<" "
			<< begin->second->getPass() << " " << begin->second->getExp() << endl;
		begin++;
	}
	out.close();
}

void game::saveDesigner()
{
	ofstream out("designer.txt");
	auto begin = designerInfo.begin(), end = designerInfo.end();

	while (begin != end)
	{
		out << begin->second->getName() << " " << begin->second->getPw() << " " << begin->second->getLevel()<<" "
			<< begin->second->getWord() << endl;
		begin++;
	}
	out.close();
}

void game::saveVocabulary()
{
	ofstream out("vocabulary.txt");
	auto begin = vocabulary.begin(), end = vocabulary.end();
	while (begin!=end)
	{
		for (auto word:begin->second)
		{
			out << word << endl;
		}
		begin++;
	}
	out.close();
}

bool game::ConnectServer()
{
	WSADATA wsaData;
	struct addrinfo *result = nullptr, hints;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "网络服务启动失败！" << endl;
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cout << "无法连接到服务器！" << endl;
		WSACleanup();
		return false;
	}

	for (struct addrinfo *p = result; p != nullptr; p = p->ai_next)
	{
		ConnectSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(ConnectSocket == INVALID_SOCKET)
		{
			cout << "无法连接到服务器！" << endl;
			WSACleanup();
			return false;
		}

		iResult = connect(ConnectSocket, p->ai_addr, (int)p->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "无法连接到服务器！" << endl;
		WSACleanup();
		return false;
	}

	return true;
}

void game::disconnect(bool connecting)
{
	if (!connecting) return;

	closesocket(ConnectSocket);
	WSACleanup();
}
