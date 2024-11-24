#include <condition_variable> 
#include <mutex> 
#include<vector>

template<typename T>
class TSVector {
	std::vector<T> m_vector;
	std::mutex m_mutex;
	std::condition_variable m_cond;
public:
	void push_back(T el) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_vector.push_back(el);
		m_cond.notify_one();
	}
	T pop_back() {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.wait(lock,
			[this]() { return !m_vector.empty(); });
		T el = m_vector.back();
		m_vector.pop_back();
		return el;
	}
	size_t size() {
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_vector.size();
	}
	bool empty() {
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_vector.empty();
	}
	T& operator[](size_t idx) {
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_vector[idx];
	}
};
