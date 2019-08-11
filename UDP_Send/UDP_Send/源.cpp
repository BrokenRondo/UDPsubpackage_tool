#include"osapi/osapi.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;


int Udp_Send(string signatures,OS_SockAddr peer)
{
	OS_UdpSocket send_socket;
	OS_SockAddr local("192.168.0.100", 55555);
	send_socket.Open(local, true);
	send_socket.SetOpt_RecvTimeout(300);
	//���һ�����ĳ���Ϊ256�ֽڣ�ǰ�����ַ�������ʾ��һ�����ж����ֽڣ��������256�ֽ�Ҳ��ǰ�����ַ���д����
	//��������ַ����ĳ��ȣ�Ȼ����зֿ�
	string tempstr;
	int head = 0, tail = 1024;
	int n = 0;
	char buf[5] = { 0 };

	while (true)
	{
		tempstr = "";
		if(tail<signatures.size()-1)
		{
			//tempstr = "256";
			tempstr += signatures.substr(head, 1024);
			int times = 0;
			while(true)
			{
				send_socket.SendTo(tempstr.c_str(),1024, peer);
				n = send_socket.RecvFrom(buf, 4, peer);//����follower����Ӧ�����û����Ӧ���ط�һ��
				if (n>=0&&strcmp(buf,"OK")==0)
				{
					break;
				}
				OS_Thread::Msleep(5);//���û����Ӧ����20ms�����·���
				cout << "���·���1" << endl;
				times++;
				if (times==3)
				{
					cout << "���·���û�յ���Ӧ�������ط�1" << endl;
					break;
				}
			}
			head += 1024;
			tail += 1024;
			OS_Thread::Msleep(3);
		}
		else if (tail == signatures.size() - 1)//������÷�����
		{
			//tempstr = "256";
			tempstr += signatures.substr(head, 1024);
			int time = 0;
			while (true)
			{
				send_socket.SendTo(tempstr.c_str(), 1024, peer);
				n = send_socket.RecvFrom(buf, 4, peer);//����follower����Ӧ�����û����Ӧ���ط�һ��
				if (n >= 0 && strcmp(buf, "OK") == 0)
				{
					break;
				}
				OS_Thread::Msleep(5);//���û����Ӧ����20ms�����·���
				cout << "���·���2" << endl;
				time++;
				if (time==3)
				{
					cout << "���·���û�յ���Ӧ�������ط�2" << endl;
					break;
				}
			}
			while (true)
			{
				//tempstr = "END";
				send_socket.SendTo(tempstr.c_str(),tempstr.size(), peer);
				n = send_socket.RecvFrom(buf, 4, peer);//����follower����Ӧ�����û����Ӧ���ط�һ��
				if (n >= 0 && strcmp(buf, "OK") == 0)
				{
					break;
				}
				OS_Thread::Msleep(20);//���û����Ӧ����20ms�����·���
				cout << "���·���3" << endl;
			}
			break;
		}
		else//���������256
		{
			//tempstr = "FFF";
			tempstr+=signatures.substr(head,signatures.size()-head);
			int times = 0;
			while (true)
			{
				send_socket.SendTo(tempstr.c_str(), tempstr.size(), peer);
				n = send_socket.RecvFrom(buf, 4, peer);//����follower����Ӧ�����û����Ӧ���ط�һ��
				if (n >= 0 && strcmp(buf, "OK") == 0)
				{
					break;
				}
				OS_Thread::Msleep(5);//���û����Ӧ����20ms�����·���
				cout << "���·���4" << endl;
				times++;
				if (times==3)
				{
					cout << "���·���û�յ���Ӧ�������ط�3" << endl;
					break;
				}
			}
			break;
		}

	}
	send_socket.Close();
	return 0;
}
int main()
{
	OS_SockAddr peer{ "192.168.0.102",55556 };
	ifstream fin("./signatures.txt");
	if (!fin)cerr << "��ʧ��" << endl;
	stringstream buffer;
	buffer << fin.rdbuf();
	string signatures(buffer.str());//һ�ζ�ȡȫ������
	fin.close();
	Udp_Send(signatures, peer);
	cout << signatures << endl;
	cout << "�������" << endl;
	getchar();
	return 0;
}