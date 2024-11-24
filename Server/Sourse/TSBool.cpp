#ifndef _TSBOOL_
#define _TSBOOL_
#include<mutex>

class TSBool {
	bool m_data;
	std::mutex m_mutex;
public:
	TSBool(bool b) :m_data(b) {}
	TSBool& operator=(const TSBool& other) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_data = other.m_data;
		return *this;
	}
	TSBool& operator=(const bool& b) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_data = b;
		return *this;
	}
	TSBool& operator=(TSBool&& other) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_data = std::move(other.m_data);
		return *this;
	}
	TSBool& operator=(bool&& b) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_data = b;
		return *this;
	}
	operator bool() const{
		return m_data;
	}
};
#endif
