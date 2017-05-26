#include "game.h"
#include <conio.h>
#include <iostream>
#include <iomanip>
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
	designer::levelPuzzleInit();
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
	cout << "*                     �� �� �� �� �� Ϸ                     *" << endl;
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
	int log = 0,valid=0;

	cout << "ѡ���¼����(0.���� 1.���� 2.����):";
	cin >> type;
	while (type != "0"&&type != "1"&&type != "2")
	{
		cout << "ѡ���¼����(0.���� 1.���� 2.����):";
		cin >> type;
	}

	
	send(ConnectSocket, type.c_str(), type.size()+1,0);
	//recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
	if (type == "2") return FAIL;
	else if (stoi(type) == CHALLENGE) user = &ChallengerInfo;
	else user = &designerInfo;
	cout << "�������û���(����\"r\"����)��";
	cin >> name;
	send(ConnectSocket, name.c_str(), name.size()+1, 0);
	
	while (name!="r")
	{
		recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		sscanf(recvbuf, "%d", &valid);
		if (valid)
		{
			cout << "���������룺";
			cin >> pw;
			send(ConnectSocket, pw.c_str(), pw.size()+1, 0);
			while (pw != "r")
			{
				recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
				sscanf(recvbuf, "%d", &log);
				
				if (log)
				{
					send(ConnectSocket, "ack", 4, 0);
					recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
					char n[20], p[20];
					int level;
					if (stoi(type) == CHALLENGE)
					{
						int pass, Exp;
						sscanf(recvbuf, "%s %s %d %d %d", n, p, &level, &pass, &Exp);
						currentPlayer = new challenger(n, p, level, pass, Exp);
					}
					else
					{
						int puzzle;
						sscanf(recvbuf, "%s %s %d %d", n, p, &level, &puzzle);
						currentPlayer = new designer(n, p, level, puzzle);
					}
					currentPlayer->setSocket(ConnectSocket);
					return Mode(stoi(type));
				}
				else
				{
					cout << "�������" << endl;
					cout << "����������(����\"r\"����)��";
					cin >> pw;
					send(ConnectSocket, pw.c_str(), pw.size()+1, 0);
				}
			}
		}
		else
		{
			cout << "�Ҳ������û���" << endl;
			cout << "�������û���(����\"r\"����)��";
			cin >> name;
			send(ConnectSocket, name.c_str(), name.size()+1, 0);
			sscanf(recvbuf, "%d", &valid);
		}
	}
	//recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
	return FAIL;
}

Mode game::Register()
{
	string name, pw;
	string type;
	int valid = 0, reg = 0;
	map<string, player*> *user = nullptr;

	cout << "ѡ��ע������(0.���� 1.���� 2.����):";
	cin >> type;
	while (type != "0"&&type != "1"&&type != "2")
	{
		cout << "ѡ��ע������(0.���� 1.���� 2.����):";
		cin >> type;
	}
	send(ConnectSocket, type.c_str(), type.size()+1, 0);
	if (type == "2") return FAIL;
	else if (stoi(type) == CHALLENGE) user = &ChallengerInfo;
	else user = &designerInfo;

	cout << "�������û���(����\"r\"����)��";
	cin >> name;
	send(ConnectSocket, name.c_str(), name.size()+1, 0);
	while (name != "r")
	{
		recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		sscanf(recvbuf, "%d", &valid);
		
		if (valid)
		{
			cout << "���������룺";
			cin >> pw;
			send(ConnectSocket, pw.c_str(), pw.size()+1, 0);
			if (stoi(type) == CHALLENGE)
			{
				currentPlayer = new challenger(name, pw);
			}
			else
			{
				currentPlayer = new designer(name, pw);
			}
			currentPlayer->setSocket(ConnectSocket);
			return Mode(stoi(type));
		}
	}
	return FAIL;
}

int game::userIn()
{
	string option;
	cout << "��ѡ��0.��¼"<<setw(10)<<"1.ע��" << setw(10) <<"2.�˳�" << endl;
	
	cin >> option;
	int op = 0;
	while (mode == FAIL&&op != QUIT)
	{
		while (option!="0"&&option != "1"&&option != "2")
		{
			cout << "��������ȷ��ѡ�0.��¼" << setw(10)<<"1.ע��"<< setw(10)<<"2.�˳�" << endl;
			cin >> option;
		}
		send(ConnectSocket, option.c_str(), option.size()+1, 0);
		if (option == "2") op = QUIT;
		else op = stoi(option);

		switch (op)
		{
		case LOGIN:
			cout << "************************�� ¼**************************" << endl;
			mode = Login();
			break;
		case REGISTER:
			cout << "*************************ע ��*************************" << endl;
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
	int isCha;
	char n[20];
	recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
	sscanf(recvbuf, "%d %s", &isCha,n);
	if (isCha)
	{
		cout <<n<< "���㷢����ս���Ƿ���ܣ�(y/n):" << endl;
		cin >> choice;
		send(ConnectSocket, choice.c_str(), choice.size() + 1, 0);
		if (choice == "y")
		{
			opponent = n;
			fight();
		}
	}
	cout << "*************************�� �� �� �� ��*************************" << endl;
	cout << "��ѡ��0.��ʼ��Ϸ 1.�鿴���� 2.�����û� 3.ע�� 4.�˳�" << endl;
	cin >> choice;
	while (choice!="3"&&choice!="4")
	{
		if (choice == "0")
		{
			send(ConnectSocket, choice.c_str(), choice.size() + 1, 0);
			challenge();
		}
		else if (choice == "1")
		{
			send(ConnectSocket, choice.c_str(), choice.size() + 1, 0);
			rank();
		}
		else if (choice == "2")
		{
			send(ConnectSocket, choice.c_str(), choice.size() + 1, 0);
			search();
		}

		recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		sscanf(recvbuf, "%d %s", &isCha, n);
		if (isCha)
		{
			cout <<n<< "���㷢����ս���Ƿ���ܣ�(y/n):" << endl;
			cin >> choice;
			send(ConnectSocket, choice.c_str(), choice.size() + 1, 0);
			if (choice == "y")
			{
				opponent = n;
				fight();
			}
		}

		cout << "��ѡ��0.��ʼ��Ϸ 1.�鿴���� 2.�����û� 3.ע�� 4.�˳�" << endl;
		cin >> choice;
	}
	send(ConnectSocket, choice.c_str(), choice.size() + 1, 0);
	if (choice == "3")
	{
		send(ConnectSocket, choice.c_str(), choice.size() + 1, 0);
		mode = FAIL;
		return LOGOUT;
	}
	else if (choice == "4") 
	{
		send(ConnectSocket, choice.c_str(), choice.size() + 1, 0);
		return QUIT;
	}
	cout << "******************************************************" << endl;
}

void game::challenge()
{
	system("cls");

	string currentWord;
	
	int pass = 0, solved = 0;
	int duration = 8000;
	
	
	string again = "y";
	while (again=="y")
	{
		
		cout << "************************�� " << pass + 1 << " ��************************" << endl;
		
		recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		currentWord = recvbuf;
		cout << currentWord << endl;
		_sleep(duration);
		system("cls");

		cout << "************************�� " << pass + 1 << " ��************************" << endl;
		cout << "�����:" << endl;
		while (true)
		{
			solved = currentPlayer->solve(currentWord);
			sprintf(recvbuf, "%d", solved);
			send(ConnectSocket, recvbuf, strlen(recvbuf) + 1, 0);
			if (solved == 0) break;
			system("cls");

			pass++;
			recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
			currentWord = recvbuf;
			if (currentWord == "end!") break;
			cout << "************************�� " << pass + 1 << " ��************************" << endl;
			cout << currentWord << endl;
			if ((pass - 1) % 5 == 0 && duration >= 1000) duration -= 600;
			_sleep(duration);
			system("cls");
			cout << "************************�� " << pass + 1 << " ��************************" << endl;
			cout << "�����:" << endl;
		}
		
		if (currentWord == "end!") cout << "��ϲȫ��ͨ�أ�" << endl;
		cout << "����һ�Σ�y/n" << endl;
		cin >> again;
		while (again!="y"&&again!="n")
		{
			cout << "y/n?" << endl;
			cin >> again;
		}
		send(ConnectSocket, again.c_str(), again.size()+1, 0);
		pass = 0;
		duration = 0;
		
	}
	currentPlayer->showInfo(recvbuf);

	cout << "****************************************************" << endl;
}

void game::rank()
{
	recv(ConnectSocket, recvbuf, 2, 0);
	string type;
	int size, l, p, e, pu;
	char n[20];
	cout << "����(0.������ 1.������ 2.����):";
	cin >> type;
	while (type!="0"&&type != "1"&&type != "2")
	{
		cout << "����(0.������ 1.������ 2.����):";
		cin >> type;
	}
	send(ConnectSocket, type.c_str(), type.size() + 1, 0);
	if (type == "0")
	{
		cout << "����" << setw(10)<<"����" << setw(10) << "�ȼ�" << setw(10) << "��߹ؿ�" << setw(10) << "����" << endl;
		recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		sscanf(recvbuf, "%d", &size);
		send(ConnectSocket, "2", 2, 0);
		for (int i = 0; i < size; i++)
		{
			recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
			send(ConnectSocket, "1", 2, 0);
			sscanf(recvbuf, "%s %d %d %d", n, &l, &p, &e);
			cout << i + 1 << setw(10) << n << setw(10) << l << setw(10) << p << setw(10) << e << endl;
		}
	}
	else if (type == "1")
	{
		cout << "����" << setw(10) << "����" << setw(10) << "�ȼ�" << setw(10) << "������" << endl;
		recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		sscanf(recvbuf, "%d", &size);
		for (int i = 0; i < size; i++)
		{
			recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
			send(ConnectSocket, "1", 2, 0);
			sscanf(recvbuf, "%s %d %d", n, &l, &pu);
			cout << i + 1 << setw(10) << n << setw(10) << l << setw(10) << pu<< endl;
		}
	}
	cout << "******************************************************" << endl;
}

void game::search()
{
	string choice;
	string attr;
	string aChoice;
	string rp, name;
	cout << "**********************�� �� �� ��**********************" << endl;
	cout << "����(0.������ 1.������ 2.����):";
	cin >> choice;
	send(ConnectSocket, choice.c_str(), choice.size() + 1, 0);
	while (choice!="2")
	{
		if (choice == "0")
		{
			cout << "����(0.���� 1.�ȼ� 2.������ 3.���� 4.����):";
			cin >> aChoice;
			while (aChoice != "0"&&aChoice != "1"&&aChoice != "2"&&aChoice != "3"&&aChoice != "4")
			{
				cout << "����������(0.���� 1.�ȼ� 2.������ 3.���� 4.����):";
				cin >> aChoice;
			}
			send(ConnectSocket, aChoice.c_str(), aChoice.size() + 1, 0);
			switch (stoi(aChoice))
			{
			case 0:
				cout << "����:";
				cin >> attr;
				send(ConnectSocket, attr.c_str(), attr.size() + 1, 0);
				searchChallenger(attr, -1, -1, -1);
				break;
			case 1:
				cout << "�ȼ�:";
				cin >> attr;
				send(ConnectSocket, attr.c_str(), attr.size() + 1, 0);
				searchChallenger(string(), stoi(attr), -1, -1);
				break;
			case 2:
				cout << "������:";
				cin >> attr;
				send(ConnectSocket, attr.c_str(), attr.size() + 1, 0);
				searchChallenger(string(), -1, stoi(attr), -1);
				break;
			case 3:
				cout << "����:";
				cin >> attr;
				send(ConnectSocket, attr.c_str(), attr.size() + 1, 0);
				searchChallenger(string(), -1, -1, stoi(attr));
				break;
			case 4:
				if (searchChallenger(string(), -1, -1, -1))
				{
					cout << "�Ƿ���ս?(y/n):";
					cin >> rp;
					while (rp!="y"&&rp!="n")
					{
						cin >> rp;
					}
					send(ConnectSocket, rp.c_str(), rp.size() + 1, 0);
					recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
					if (rp == "y")
					{
						cout << "��ս���û���:" << endl;
						cin >> name;
						send(ConnectSocket, name.c_str(), name.size() + 1, 0);
						match(name);
					}
				}
				break;
			default:
				break;
			}
		}
		else if (choice == "1")
		{
			cout << "����(0.���� 1.�ȼ� 2.������):";
			cin >> aChoice;

			while (aChoice != "0"&&aChoice != "1"&&aChoice != "2")
			{
				cout << "����������(0.���� 1.�ȼ� 2.������):";
				cin >> aChoice;
			}
			send(ConnectSocket, aChoice.c_str(), aChoice.size() + 1, 0);
			switch (stoi(aChoice))
			{
			case 0:
				cout << "����:";
				cin >> attr;
				send(ConnectSocket, attr.c_str(), attr.size() + 1, 0);
				searchDesigner(attr, -1, -1);
				break;
			case 1:
				cout << "�ȼ�:";
				cin >> attr;
				send(ConnectSocket, attr.c_str(), attr.size() + 1, 0);
				searchDesigner(string(), stoi(attr), -1);
				break;
			case 2:
				cout << "������:";
				cin >> attr;
				send(ConnectSocket, attr.c_str(), attr.size() + 1, 0);
				searchDesigner(string(), -1, stoi(attr));
				break;
			default:
				break;
			}
		}
		cout << "����(0.������ 1.������ 2.����):";
		cin >> choice;
		send(ConnectSocket, choice.c_str(), choice.size() + 1, 0);
	}
	cout << "******************************************************" << endl;
}

bool game::searchChallenger(string name, int level, int exp, int pass)
{
	int size, l, p, e,r;
	char n[20];
	cout << "�û���"<<setw(10) << "�ȼ�" << setw(10) << "��߹ؿ�" << setw(10) << "����" << setw(10) <<"����" << endl;
	recv(ConnectSocket, recvbuf, 2, 0);
	sscanf(recvbuf, "%d", &size);
	for (int i = 0; i < size; i++)
	{
		recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		sscanf(recvbuf, "%s %d %d %d %d", n, &l, &p, &e,&r);
		send(ConnectSocket, "1", 2, 0);
		cout << n<< setw(10) << l << setw(10) << p << setw(10) << e<< setw(10) <<r << endl;
	}
	if (size == 0)
	{
		cout << "�û������ڣ�" << endl;
		return false;
	}
	else return true;
}

void game::searchDesigner(string name, int level, int wordCount)
{
	int size, l, p,r;
	char n[20];
	cout << "�û���" <<setw(10)<< "�ȼ�" << setw(10) << "������" << setw(10) <<"����"<< endl;
	recv(ConnectSocket, recvbuf, 2, 0);
	sscanf(recvbuf, "%d", &size);
	for (int i = 0; i < size; i++)
	{
		recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		sscanf(recvbuf, "%s %d %d %d", n, &l, &p,&r);
		send(ConnectSocket, "1", 2, 0);
		cout << n << setw(10) << l << setw(10) << p<< setw(10) <<r << endl;
	}
	if (size==0) cout << "�û������ڣ�" << endl;
}

int game::designUi()
{
	string choice;
	cout << "*************************�� �� �� �� ��*************************" << endl;
	cout << "��ѡ��0.���� 1.�鿴���� 2.�����û� 3.ע�� 4.�˳�" << endl;
	cin >> choice;
	send(ConnectSocket, choice.c_str(), choice.size() + 1, 0);
	while (choice != "4" && choice != "3")
	{
		if (choice == "0") design();
		else if (choice == "1") rank();
		else if (choice == "2") search();

		cout << "��ѡ��0.���� 1.�鿴���� 2.�����û� 3.ע�� 4.�˳�" << endl;
		cin >> choice;
		send(ConnectSocket, choice.c_str(), choice.size() + 1, 0);
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
	cout << "*************************�� ��*************************" << endl;
	cout << "����Ҫ��ӵ��´ʣ�����!qֹͣ���⣩:";
	cin >> word;
	send(ConnectSocket, word.c_str(), word.size() + 1, 0);
	while (word != "!q")
	{
		recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		sscanf(recvbuf, "%d", &flag);
		if (flag)
		{
			cout << "��ӳɹ���" << endl;
			//currentPlayer->refreshInfo(1);
			//currentPlayer->reRank();
		}
		else cout << "�����Ѵ��ڣ�" << endl;

		cout << "����Ҫ��ӵ��´ʣ�����!qֹͣ���⣩:";
		cin >> word;
		send(ConnectSocket, word.c_str(), word.size() + 1, 0);
	}
	system("cls");
	currentPlayer->showInfo(recvbuf);
}

void game::match(std::string name)
{
	cout << "���Ժ�..." << endl;
	recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
	switch (recvbuf[0])
	{
	case '0':
		cout << "�û������ڣ�" << endl;
		break;
	case '1':
		cout << "�û������ߣ�" << endl;
		break;
	case '2':
		cout << "�Է�������Ϸ��" << endl;
		break;
	case '3':
		cout << "�Է��ܾ��������ս��" << endl;
	case '4':
		opponent = name;
		fight();
	default:
		break;
	}
}

void game::fight()
{
	system("cls");
	
	string currentWord;

	int pass = 0, solved = 0;
	int duration = 5000;
	int start, finish;
	
	{
		cout << "��ս" << opponent << endl;
		cout << "************************�� " << pass + 1 << " ��************************" << endl;
		
		recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		currentWord = recvbuf;
		cout << currentWord << endl;
		_sleep(duration);
		system("cls");
		cout << "��ս" << opponent << endl;
		cout << "************************�� " << pass + 1 << " ��************************" << endl;
		cout << "�����:" << endl;
		start = clock();
		while (pass!=9)
		{
			solved = currentPlayer->solve(currentWord);
			finish = clock();
			sprintf(recvbuf, "%d %ld", solved,finish-start);
			send(ConnectSocket, recvbuf, strlen(recvbuf) + 1, 0);
			if (solved == 0) break;
			system("cls");
			pass++;
			recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
			currentWord = recvbuf;
			if (currentWord == "1") break;
			cout << "��ս" << opponent << endl;
			cout << "************************�� " << pass + 1 << " ��************************" << endl;
			cout << currentWord << endl;
			if ((pass - 1) % 5 == 0 && duration >= 1000) duration -= 600;
			_sleep(duration);
			system("cls");
			cout << "��ս" << opponent << endl;
			cout << "************************�� " << pass + 1 << " ��************************" << endl;
			cout << "�����:" << endl;
			start = clock();
		}
		int win;
		if(currentWord=="1") cout << "��Ӯ�ˣ�" << endl;
		else
		{
			recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
			sscanf(recvbuf, "%d", &win);
			if (win == 0)
			{
				cout << "�����ˣ�" << endl;
			}
			else
			{
				cout << "��Ӯ�ˣ�" << endl;
			}
		}
	}

	cout << "****************************************************" << endl;
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
			<< begin->second->getPuzzle() << endl;
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
		cout << "�����������ʧ�ܣ�" << endl;
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cout << "�޷����ӵ���������" << endl;
		WSACleanup();
		return false;
	}

	for (struct addrinfo *p = result; p != nullptr; p = p->ai_next)
	{
		ConnectSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(ConnectSocket == INVALID_SOCKET)
		{
			cout << "�޷����ӵ���������" << endl;
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
		cout << "�޷����ӵ���������" << endl;
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
