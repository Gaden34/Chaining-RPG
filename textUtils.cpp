#include "textUtils.h"

namespace TextUtils {
	std::string capitalizeFirst(const std::string& text) {
		std::string result = text;
		
		if (!text.empty()) {
			result[0] = std::toupper(text[0]);
		}
		return result;
	}

	std::string lowerFirst(const std::string& text) {
		std::string result = text;
		
		if (!text.empty()) {
			result[0] = std::tolower(text[0]);
		}
		return result;
	}

	std::string articleFor(const std::string& text) {
		if (text.empty()) return "a";
		char firstChar = std::tolower(text[0]);
		if (firstChar == 'a' || firstChar == 'e' || firstChar == 'i' || firstChar == 'o' || firstChar == 'u') {
			return "an";
		}
		return "a";
	}
}