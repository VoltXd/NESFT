#ifdef TEST_6502
#include <cstdio>

#include "gtest/gtest.h"

GTEST_API_ int main(int argc, char **argv) {
	printf("Running main() from %s\n", __FILE__);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

#else
#include <iostream>

#include "App.hpp"

int main()
{
    App emulator;
    return emulator.run();
}
#endif
