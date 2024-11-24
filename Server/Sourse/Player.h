#ifndef _PLAYER_
#define _PLAYER_
#include<winsock2.h>
#include<string>
#include<mutex>
#include"TSBool.cpp"

class Player {
	std::string m_name;
	std::string m_password;
	TSBool m_online = true;
public:
	SOCKET m_sock;
	std::mutex mtx;
	Player(SOCKET sock,const std::string& name, const std::string& password);
	Player(const Player& other);
	Player(Player&& other);
	Player& operator=(const Player& other);
	Player& operator=(Player&& other);
	bool isTruePassword(const std::string& password) const;
	bool getStatus() const;
	void setStatus(bool b);
	std::string getName() const;
};
//static Player NULL_PLAYER(SOCKET_ERROR, "", "");
#endif