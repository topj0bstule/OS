#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <vector>

void runParentMode();
void runChildMode();

int main(int argc, char* argv[]) { // вводим аргументы, анализируя их выполняем какие-то действия
	if (argc > 1 && strcmp(argv[1], "child") == 0) runChildMode();
	else runParentMode();
	return 0;
}

void runParentMode() {
	int size;
	std::cout << "Enter array size (1-1000): \n";
	std::cin >> size;
	if (size < 1 || size > 1000) {
		std::cout << "Memory allocation failed";
		return;
	}

	std::vector<int> numbers(size);
	std::cout << "Enter " << size << " array numbers:\n";
	for (int i = 0; i < size; i++) {
		std::cin >> numbers[i];
		if(std::cin.fail()){
			std::cerr << "Invalid input";
			return;
		}
	}

	SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, true }; // настройка аттрибутов безопасности каналов
	HANDLE readPipetoChild, writePipefromChild;
	HANDLE writePipefromParent, readPipetoParent;
	if (!(CreatePipe(&readPipetoChild, &writePipefromChild, &sa, 0) || CreatePipe(&readPipetoParent, &writePipefromParent, &sa, 0))) {
		std::cerr << "Chaannel creation error";
		return;
	}

	SetHandleInformation(&readPipetoChild, HANDLE_FLAG_INHERIT, 0); // отключаем наследование Parent
	SetHandleInformation(&writePipefromChild, HANDLE_FLAG_INHERIT, 0);

	STARTUPINFO si{}; // настройка дочернего процесса перед его запуском (переопределение стандартных дескрипторов input, output, error)
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = readPipetoChild;
	si.hStdOutput = writePipefromChild;
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);




}

void runChildMode() {

}