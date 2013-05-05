#include <locale>
#include "client.h"

int main() {
	setlocale(LC_ALL,"RUS");

	Client client;
	int iResult = 0;

	iResult = client.Start();
	
	if (iResult != 0) {
		wprintf(L"Error: %ld\n", iResult);
		return 1;
	}

	system("pause");
	return 0;
}