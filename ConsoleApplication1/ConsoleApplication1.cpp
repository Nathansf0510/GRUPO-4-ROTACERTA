#include <iostream>
#include <string>
using namespace std;

int main() {
    const int C = 3;
    double ids[C];
    string Nome[C];
    for (int l = 0; l < 3; l++) {
        bool repetido;
        do {
            repetido = false;
            cout << "Digite o Codigo do Produto do " << l+1 << " : ";
            cin >> ids[l];
            for (int i = 0; i < l; i++) {
                if (ids[l] == ids[i]) {
                    repetido = true;
                    cout << "Codigo ja existe! Tente outro." << endl;
                }
            }
        } while (repetido); // Repete enquanto encontrar um valor igual
        cout << "Digite o nome do Produto do " << l+1 << " : ";
        cin >> Nome[l];
        
    }
    return 0;
}
