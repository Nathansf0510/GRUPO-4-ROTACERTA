#include <iostream>
#include <string>
using namespace std;

int main() {
	const int C = 3;
	char uuid[C];
	string Nome[C];
	int peco[C];
	for (int l = 0; l < 3; l++) {
		cout << "=== Produto " << l+1 << " ==="<<"\n";
		bool repetido;
		do {
			repetido = false;
			cout << "Digite o Codigo: ";
			cin >> uuid[l];
			for (int i = 0; i < l; i++) {
				if (uuid[l] == uuid[i]) {
					repetido = true;
					cout << "Codigo ja existe! Tente outro." << endl;
				}
			}
		} while (repetido); // Repete enquanto encontrar um valor igual
		cout << "Digite o nome: ";
		cin >> Nome[l];
		cout << "Digite o peço: ";
		cin >> peco[l];
		cout << "\n";
	}
	return 0;
}
