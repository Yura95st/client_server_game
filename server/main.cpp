#include <locale>
#include "server.h"

int main() {
	setlocale(LC_ALL,"RUS");

	Server server;
	int iResult = 0;

	iResult = server.start();
	
	if (iResult != 0) {
		wprintf(L"Error: %ld\n", iResult);
		return 1;
	}
	system("pause");
    return 0;	
}