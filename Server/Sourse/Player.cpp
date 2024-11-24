#include"Player.h"

Player::Player(SOCKET sock,const std::string& name, const std::string& password):m_name(name),m_password(password),m_sock(sock){}
bool Player::isTruePassword(const std::string& password) const{
	return m_password == password;
}
bool Player::getStatus() const{
	return m_online;
}
void Player::setStatus(bool b) {
	m_online = b;
}
std::string Player::getName() const{
	return m_name;
}
Player::Player(const Player& other) {
	m_name = other.m_name;
	m_password = other.m_password;
	m_online = other.m_online;
	m_sock = other.m_sock;
}
Player::Player(Player&& other) {
	m_name = std::move(other.m_name);
	m_password = std::move(other.m_password);
	m_online = std::move(other.m_online);
	m_sock = std::move(other.m_sock);
}
Player& Player::operator=(const Player& other){
	if (this != &other) {
		m_name = other.m_name;
		m_password = other.m_password;
		m_online = other.m_online;
		m_sock = other.m_sock;
	}
	return *this;
}
Player& Player::operator=(Player&& other) {
	if (this != &other) {
		m_name = std::move(other.m_name);
		m_password = std::move(other.m_password);
		m_online = std::move(other.m_online);
		m_sock = std::move(other.m_sock);
	}
	return *this;
}