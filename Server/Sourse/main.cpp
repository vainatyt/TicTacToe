#include<iostream>
#include<winsock2.h>
#include<thread>
#include<random>
#include<fstream>
#include <ctime>

#include"TicTacToe.h"
#include"TSQueue.cpp"
#include"TSVector.cpp"
#include"Player.h"

#pragma comment(lib, "ws2_32.lib")	

std::fstream my_log;
int PORT = 0;

SOCKET SERVER;
sockaddr_in ADDRESS;
int ADDRLEN;


TSQueue<std::string> mes;
TSVector<Player*> players;
size_t find(TSVector<Player*>& tVec, std::string name) {
	for (size_t i = 0; i < tVec.size();++i) {
		if (tVec[i]->getName() == name) {
			return i;
		}
	}
	return -1;
}
std::string parsMes(std::string message, size_t& curr) {
	std::string out;
	while (message[curr]==' ') {
		++curr;
	}
	while (message[curr]!=' ' && message[curr] != '\0' && curr<message.size()) {
		out += message[curr++];
	}
	return out;
}
std::string time() {
	time_t now = time(0);
	tm localTime;
	localtime_s(&localTime, &now);
	char time[80];
	strftime(time, sizeof(time), "%d.%m.%Y %H:%M:%S", &localTime);
	return "[" + std::string(time) + "] ";
}
void tRecv(Player* player) {
	std::lock_guard<std::mutex> lock(player->mtx);
	DWORD timeout = 100;
	if (setsockopt(player->m_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) { return;}
	my_log << time() << "Set timer for " + player->getName()+'\n';
	player->setStatus(true);
	while (player->getStatus()) {
		char message[1024];
		for (size_t i = 0; i < 1024;++i) {message[i] = '\0';}
		if (recv(player->m_sock, message, 1024, 0) == SOCKET_ERROR) {
			if (WSAETIMEDOUT == WSAGetLastError()) {continue;}
			player->setStatus(false);
			break;
		}
		std::string full = player->getName() + ' ' + message;
		mes.push(full);
		my_log << time() << full<<'\n';
	}
	timeout = 0;
	if (setsockopt(player->m_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) { return; }
	my_log << time() << "Remove timer for " + player->getName()+'\n';
}
void tRegistration(SOCKET member) {
	//registration and in end push member in base data
	my_log << time() << "Member on registration\n";
	std::string sender = "Hello, do you want \'sing up\' or \'log in\'?";
	char receiver[1024];
	if (send(member, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) {return;}
	while(true){
		for (size_t i = 0; i < 1024; ++i) {
			receiver[i] = 0;
		}
		if (recv(member, receiver, 1024, 0) == SOCKET_ERROR) {return;}
		if (!strcmp(receiver,"sing up")) {
			my_log << time() << "member choose sing up\n";
			sender = "Please choose your name: ";
			if (send(member, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
			do{//chose name
				for (size_t i = 0; i < 1024; ++i) {
					receiver[i] = 0;
				}
				if (recv(member, receiver, 1024, 0) == SOCKET_ERROR) { return; }
				if (find(players, receiver)!=-1|| !strcmp(receiver,"")) {
					sender = "Please chose another name: ";
					if (send(member, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
				}
				else {
					break;
				}
			} while (true);
			std::string name = receiver;
			sender = "Please chose your password: ";
			if (send(member, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
			do {//chose password
				for (size_t i = 0; i < 1024; ++i) {
					receiver[i] = 0;
				}
				if (recv(member, receiver, 1024, 0) == SOCKET_ERROR) { return; }
				if (!strcmp(receiver,"")) {
					sender = "Please chose another password: ";
					if (send(member, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
					break;
				}
				else {
					break;
				}
			} while (true);
			sender = "Welcome to TicTacToe";
			if (send(member, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
			std::string password = receiver;
			players.push_back(new Player(member, name, password));
			std::thread listen(tRecv,std::ref(players[find(players,name)]));
			listen.detach();
			my_log << "member: " << name << " sing up\n";
			return;
		}
		else if (!strcmp(receiver,"log in")) {
			my_log << time() << "member choose log in\n";
			sender = "What is your name: ";
			if (send(member, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
			do{
				for (size_t i = 0; i < 1024; ++i) {
					receiver[i] = 0;
				}
				if (recv(member, receiver, 1024, 0) == SOCKET_ERROR) { return; }
				if (find(players, receiver)== -1 || !strcmp(receiver,"")) {
					sender = "This name is not log in. Chose another name: ";
					if (send(member, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
				}
				else {
					break;
				}
			} while (true);
			Player* strange = players[find(players, receiver)];
			strange->m_sock = member;
			sender = "What is your password: ";
			if (send(member, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
			do {
				for (size_t i = 0; i < 1024; ++i) {
					receiver[i] = 0;
				}
				if (recv(member, receiver, 1024, 0) == SOCKET_ERROR) { return; }
				if (!strange->isTruePassword(receiver) || !strcmp(receiver,"")) {
					sender = "This name is not your password. Chose another one: ";
					if (send(member, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
				}
				else {
					break;
				}
			} while (true);
			sender = "Welcome to TicTacToe";
			if (send(member, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
			my_log << time() << "member: " << strange->getName() << " log in\n";
			std::thread rec(tRecv, std::ref(strange));
			rec.detach();
			return;
		}
		else {
			sender = "Sorry please choose \'sing up\' or \'log in\'\n";
			if (send(member, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) {return;}
		}
	}
}
void tAccept() {
	my_log << time() << "Wait new member\n";
	SOCKET member = accept(
		SERVER,
		(struct sockaddr*)&ADDRESS,
		&ADDRLEN
	);
	if (member == SOCKET_ERROR) {return;}
	my_log << time() << "New member here\n";
	std::thread reg(tRegistration, member);
	reg.detach();
}
void whileAccept() {
	my_log << time() << "Loop Accept is going\n";
	while (true) {
		std::thread thr(tAccept);
		thr.join();
	}
}
void whoIsOnline(Player* player) {
	my_log << time() << "Who online: " << players.size() << '\n';
	std::string names;
	for (size_t i = 0; i < players.size();++i) {
		if(players[i]->getStatus() == true) {
			names += players[i]->getName() + "; ";
		}
	}
	if (send(player->m_sock, names.c_str(), names.size(), 0) == SOCKET_ERROR) { return; }
}
void Play(Player* one,Player* two) {
	std::unique_lock<std::mutex> lock1(one->mtx);
	std::unique_lock<std::mutex> lock2(two->mtx);
	std::string sender = "Start the game with " + two->getName()+'\n'+"You are X, your move is firts\nTo put X you need write number row and column-> Exemple: 1 2";
	if (send(one->m_sock, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
	sender = "Start the game with " + one->getName() + '\n' + "You are O, your move is second\nTo put O you need write number row and column. Exemple: 1 2";
	if (send(two->m_sock, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
	Board board;
	my_log << time() << "Create board for " + one->getName() + " and " + two->getName() + '\n';
	while (!board.isEnd()) {
		std::string str = board.toString();
		char buffer[1024];
		if (send(one->m_sock, str.c_str(), str.size(), 0) == SOCKET_ERROR) { return;}
		if (send(two->m_sock,"Wait, your enemy move",22,0)==SOCKET_ERROR) { return; }
		std::string row;
		std::string col;
		do{
			if (send(one->m_sock, "Choose the place", 17, 0) == SOCKET_ERROR) { return; }
			for (size_t i = 0; i < 1024;++i) {
				buffer[i] = 0;
			}
			if (recv(one->m_sock, buffer, 1024, 0) == SOCKET_ERROR) { return; }
			size_t curr = 0;
			row = parsMes(buffer, curr);
			col = parsMes(buffer, curr);
		} while (!(col.size() == 1 && row.size() == 1 && col[0] < '4' && col[0]> '0' && row[0] < '4' && row[0]> '0' && board.put(row[0]-'1',col[0]-'1','X')));
		if (board.isEnd()) { break; }

		str = board.toString();
		if (send(two->m_sock, str.c_str(), str.size(), 0) == SOCKET_ERROR) { return; }
		if (send(one->m_sock, "Wait, your enemy move", 22, 0) == SOCKET_ERROR) { return; }
		do {
			if (send(two->m_sock, "Choose the place", 17, 0) == SOCKET_ERROR) { return; }
			for (size_t i = 0; i < 1024; ++i) {
				buffer[i] = 0;
			}
			if (recv(two->m_sock, buffer, 1024, 0) == SOCKET_ERROR) { return; }
			size_t curr = 0;
			row = parsMes(buffer, curr);
			col = parsMes(buffer, curr);
		} while (!(col.size() == 1 && row.size() == 1 && col[0] < '4' && col[0]> '0' && row[0] < '4' && row[0]> '0' && board.put(row[0] - '1', col[0] - '1', 'O')));
	}
	std::string b = board.toString();
	if (send(one->m_sock, b.c_str(), b.size(), 0) == SOCKET_ERROR) { return; }
	if (send(two->m_sock, b.c_str(), b.size(), 0) == SOCKET_ERROR) { return; }
	if (board.whoWin() == 'X') {
		if (send(one->m_sock, "You Win, congratulations!",26, 0) == SOCKET_ERROR) { return; }
		if (send(two->m_sock, "You Lose, well play", 20, 0) == SOCKET_ERROR) { return; }
	}
	else if (board.whoWin() == 'O') {
		if (send(one->m_sock, "You Lose, well play", 20, 0) == SOCKET_ERROR) { return; }
		if (send(two->m_sock, "You Win, congratulations!", 26, 0) == SOCKET_ERROR) { return; }
	}
	else {
		if (send(one->m_sock, "Draw", 5, 0) == SOCKET_ERROR) { return; }
		if (send(two->m_sock, "Draw", 5, 0) == SOCKET_ERROR) { return; }
	}
	my_log << time() << "Game end "+ one->getName() + " and " + two->getName() + '\n';
	lock1.unlock();
	lock2.unlock();
	std::thread rec1(tRecv, one);
	rec1.detach();
	std::thread rec2(tRecv, two);
	rec2.detach();
}
void request(Player* one, Player* two) {
	my_log << time() << one->getName() << " wants play with " << two->getName()<<'\n';
	one->setStatus(false);
	two->setStatus(false);
	std::unique_lock<std::mutex> lock1(one->mtx);
	std::unique_lock<std::mutex> lock2(two->mtx);
	std::string sender = "Do you want to play with " + one->getName()+"? Yes/No";
	if (send(two->m_sock, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
	char buffer[1024];
	do {
		for (size_t i = 0; i < 1024;++i) {
			buffer[i] = 0;
		}
		my_log << time() << "wait answer from " + two->getName();
		if (recv(two->m_sock, buffer, 1024, 0) == SOCKET_ERROR) { my_log << time() << WSAGetLastError(); return; }
		my_log << time() << "Answer is " << buffer << '\n';
	} while (strcmp(buffer, "Yes") != 0 && strcmp(buffer, "No") != 0);
	if (!strcmp(buffer, "Yes")) {
		my_log << time() << "Start game " + one->getName() +" and " + two->getName()+'\n';
		std::string sender = two->getName() + " wants to play";
		if (send(one->m_sock, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
		lock1.unlock();
		lock2.unlock();
		if (rand() % 2 == 0) {
			std::thread play(Play, one, two);
			play.detach();
		}
		else {
			std::thread play(Play, two, one);
			play.detach();
		}
	}
	else {
		lock1.unlock();
		lock2.unlock();
		std::thread rec1(tRecv, std::ref(one));
		rec1.detach();
		std::thread rec2(tRecv, std::ref(two));
		rec2.detach();
		std::string sender = two->getName() + " doesn\'t want to play with you";
		if (send(one->m_sock, sender.c_str(), sender.size(), 0) == SOCKET_ERROR) { return; }
	}
}
int main() {
	my_log.open("log.txt", std::ios::out | std::ios::trunc);
	my_log << time() << "log file is open\n";
	if (!my_log.is_open()) {
		std::cout << time() << "fatal\n";
		return 1;
	}
	std::ifstream konfig("konfig.txt", std::ios::in);
	if (!konfig.is_open()) {
		my_log << time() << "File \"konfig.txt\" not found" << std::endl;
		my_log.close();
		return 1;
	}
	my_log << time() << "File \"konfig.txt\" is open\n";
	std::string sPORT;
	getline(konfig, sPORT, '\n');
	for (size_t i = 0; i < sPORT.size(); ++i) {
		PORT *= 10;
		PORT += sPORT[i] - '0';
	}
	my_log << time() << ("Server socket is " + std::to_string(PORT) + '\n');

	//Initialize winsock
	my_log << time() << "Initialising Winsock...\n";
	WSADATA wsa;
	int res = WSAStartup(MAKEWORD(2, 2), &wsa);

	int valread;
	const char* hello = "hello";
	char buffer[1024] = { 0 };
	SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);
	// Creating socket file descriptor

	my_log << time() << "Server socket created.\n";

	struct sockaddr_in address;
	int addrlen = sizeof(address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	res = bind(
		server_sock,
		(struct sockaddr*)&address,
		sizeof(address)
	);

	SERVER = server_sock;//выносим в глобальные переменные что бы можно было в любом потоке без проблем создать новый сокет
	ADDRESS = address;
	ADDRLEN = sizeof(ADDRESS);

	my_log << time() << "Bind done\n";

	res = listen(server_sock, 3);
	if (res < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	std::thread wAcc(whileAccept);
	while (true) {
		if (!mes.empty()) {
			std::string message = mes.pop();
			size_t pointer = 0;
			my_log << time() << "From main ";
			std::string name = parsMes(message, pointer);
			my_log << name;
			std::string command = parsMes(message, pointer);
			my_log << ' ' << command<<command.size()<< '\n';
			if (strcmp(command.c_str(), "online")==0) {
				whoIsOnline(players[find(players, name)]);
			}
			else if (strcmp(command.c_str(), "play")==0) {
				std::string enemy = parsMes(message,pointer);
				if (name == enemy) {
					send(players[find(players, name)]->m_sock, "You can not play with yourself", 31, 0);
					continue;
				}
				if (find(players, enemy) == -1) {
					send(players[find(players, name)]->m_sock, "There are not players with this name", 37, 0);
					continue;
				}
				if (players[find(players, enemy)]->getStatus()) {
					std::thread req(request, std::ref(players[find(players,name)]),std::ref(players[find(players,enemy)]));
					req.detach();
				}
				else {
					send(players[find(players, name)]->m_sock, "This player is offline", 23, 0);
				}
			}
			else {
				my_log << time() << "False form command\n";
				Player* play = players[find(players, name)];
				send(play->m_sock, "It is not a command", 20, 0);
			}
		}
	}



}