#include <iostream>
using namespace std;

int main() {
    double list[3][6];

    for (int l = 0; l < 3; l++) {
        bool repetido;
        do {
            repetido = false;
            cout << "Digite o Codigo do Produto do " << l+1 << " : ";
            cin >> list[l][0];
            for (int i = 0; i < l; i++) {
                if (list[l][0] == list[i][0]) {
                    repetido = true;
                    cout << "Codigo ja existe! Tente outro." << endl;
                }
            }
        } while (repetido); // Repete enquanto encontrar um valor igual


    }

    return 0;
}
