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
	//设计一个包的长度为256字节，前三个字符用来表示这一个包有多少字节，如果不满256字节也在前三个字符中写出来
	//首先求出字符串的长度，然后进行分块
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
				n = send_socket.RecvFrom(buf, 4, peer);//接收follower的响应，如果没有响应则重发一次
				if (n>=0&&strcmp(buf,"OK")==0)
				{
					break;
				}
				OS_Thread::Msleep(5);//如果没有响应休眠20ms后重新发包
				cout << "重新发包1" << endl;
				times++;
				if (times==3)
				{
					cout << "重新发包没收到回应，不再重发1" << endl;
					break;
				}
			}
			head += 1024;
			tail += 1024;
			OS_Thread::Msleep(3);
		}
		else if (tail == signatures.size() - 1)//如果正好发送完
		{
			//tempstr = "256";
			tempstr += signatures.substr(head, 1024);
			int time = 0;
			while (true)
			{
				send_socket.SendTo(tempstr.c_str(), 1024, peer);
				n = send_socket.RecvFrom(buf, 4, peer);//接收follower的响应，如果没有响应则重发一次
				if (n >= 0 && strcmp(buf, "OK") == 0)
				{
					break;
				}
				OS_Thread::Msleep(5);//如果没有响应休眠20ms后重新发包
				cout << "重新发包2" << endl;
				time++;
				if (time==3)
				{
					cout << "重新发包没收到回应，不再重发2" << endl;
					break;
				}
			}
			while (true)
			{
				//tempstr = "END";
				send_socket.SendTo(tempstr.c_str(),tempstr.size(), peer);
				n = send_socket.RecvFrom(buf, 4, peer);//接收follower的响应，如果没有响应则重发一次
				if (n >= 0 && strcmp(buf, "OK") == 0)
				{
					break;
				}
				OS_Thread::Msleep(20);//如果没有响应休眠20ms后重新发包
				cout << "重新发包3" << endl;
			}
			break;
		}
		else//如果最后填不满256
		{
			//tempstr = "FFF";
			tempstr+=signatures.substr(head,signatures.size()-head);
			int times = 0;
			while (true)
			{
				send_socket.SendTo(tempstr.c_str(), tempstr.size(), peer);
				n = send_socket.RecvFrom(buf, 4, peer);//接收follower的响应，如果没有响应则重发一次
				if (n >= 0 && strcmp(buf, "OK") == 0)
				{
					break;
				}
				OS_Thread::Msleep(5);//如果没有响应休眠20ms后重新发包
				cout << "重新发包4" << endl;
				times++;
				if (times==3)
				{
					cout << "重新发包没收到回应，不再重发3" << endl;
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
	if (!fin)cerr << "打开失败" << endl;
	stringstream buffer;
	buffer << fin.rdbuf();
	string signatures(buffer.str());//一次读取全部数据
	fin.close();
	Udp_Send(signatures, peer);
	cout << signatures << endl;
	cout << "发送完毕" << endl;
	getchar();
	return 0;
}