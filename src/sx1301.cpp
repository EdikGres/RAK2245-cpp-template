#include <iostream>
extern "C" {
#include "loragw_hal.h"
}

int main() {
	std::cout << lgw_version_info() << std::endl;
	return 0;
}
