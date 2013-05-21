

#ifndef _CRT_SECURE_NO_DEPRECATE  // suppress MS security warnings
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <iostream>


/*int find(char* source, char element){
	int j;
	bool fin;

	fin = false;
	j = 0;
	while ((j < strlen(source)) && (!fin)){
		if (source[j] == element){
			fin = true;
		}else {
			j++; 
		}
	}
	if (fin) {
		return j;
	}else{
		return -1;
	}
}*/

int find(char* source, std::string element){
// retorna el indice dentro de source donde comienza el string element
// si element no pertenece a source -> retorna std::string::npos
	std::string s_source = std::string(source);

	std::size_t found = s_source.find(element);
	return found;
}