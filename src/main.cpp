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

#include "Emulator.hpp"

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "Error: bad argument number (" << argc << ", must be 2)" << std::endl;
		return EXIT_FAILURE;
	}

    Emulator emulator(argv[1]);
    return emulator.run();
}
#endif
