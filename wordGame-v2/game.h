#pragma once
#include "player.h"
#include <map>
#include <vector>
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <cstring>

#pragma comment(lib,"WS2_32.lib")
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib,"AdvApi32.lib")


#define DEFAULT_PORT "27015"
#define SERVER_IP "127.0.0.1"
enum OPTION{LOGIN,REGISTER,LOGOUT,QUIT};

//��Ϸϵͳ�࣬ʵ���������û���¼��ע�ᡢ��Ϸ�ȹ���
class game
{
public:
	game();
	~game();
	void run();//������Ϸ
private:
	Mode Login();//��ҵ�¼
	Mode Register();//���ע��
	int userIn();//���ѡ���¼��ע����˳�����һ������
	int challengeUi();//�����߽���
	void challenge();//�����߽�����Ϸ
	void rank();//�鿴����
	void search();//�������
	bool searchChallenger(string name, int level, int exp, int pass);//���Ҵ�����
	void searchDesigner(string name, int level, int word);//���ҳ�����
	int designUi();//�����߽���
	void design();//�����߳���
	void match();
	void fight();
	//���ش����ߣ������ߺ���Ŀ����
	void loadChallenger();
	void loadDesigner();
	void loadVocabulary();
	//���洳���ߣ������ߺ���Ŀ����
	void saveChallenger();
	void saveDesigner();
	void saveVocabulary();

	bool ConnectServer();
	void disconnect(bool connecting);

	player *currentPlayer;//��ǰ�û�ָ��
	int currentPass;//��ǰ�ؿ�
	Mode mode;//��ǰ��¼ģʽ
	map<string, player*> ChallengerInfo;//�洢���д�������Ϣ
	map<string, player*> designerInfo;//�洢���г�������Ϣ
	map<int, vector<string>> vocabulary;//��Ŀ�����ʣ���
	SOCKET ConnectSocket;
	bool connectSucceed;
	char recvbuf[DEFAULT_BUFLEN];
};

