#include <iostream>

void nested_loops() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            std::cout << "[" << i << "," << j << "]";
        }
    }
}

int main() {
    for (int k = 0; k < 5; k++) {
        std::cout << "Iteration " << k;
    }
    
    nested_loops();
    return 0;
}