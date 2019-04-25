#include <string>
#include <sstream>


namespace util {
	template<typename T, typename S>
	std::string join(T &iterable, const S& separator) {
		std::stringstream ss;

		bool first = true;
		for (const auto& e : iterable) {
			if (!first) {
				ss << separator;
			} else {
				first = false;
			}
			ss << e;
		}

		return ss.str();
	}
}
