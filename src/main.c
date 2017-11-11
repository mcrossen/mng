#include "display.h"
#include "text.h"

void error(uint64_t idx, uint64_t esr, uint64_t elr, uint64_t spsr, uint64_t far, uint64_t sctlr, uint64_t tcr) {
	// TODO: how to handle exceptions
}

/**
 * MNG entry point
 */
int main()
{
	display_init();
	printString("MNG initialization succesful!\n");
	while(1);
}
