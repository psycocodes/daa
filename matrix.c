#include <stdio.h>
#include <limits.h>

// Recursive function to print optimal parenthesization based on table s
void printOptimalParens(int i, int j, int n, int s[n][n]) {
    if (i == j) {
        printf("M%d", i);
    } else {
        printf("(");
        printOptimalParens(i, s[i][j], n, s);
        printOptimalParens(s[i][j] + 1, j, n, s);
        printf(")");
    }
}

// Iterative DP implementation
void matrixChainOrder(int p[], int n) {
    int m[n+1][n+1];
    int s[n+1][n+1];

    // Base case: cost is 0 for 1 matrix
    for (int i = 1; i <= n; i++) m[i][i] = 0; 

    // Chain length L
    for (int L = 2; L <= n; L++) {            
        for (int i = 1; i <= n - L + 1; i++) {
            int j = i + L - 1;
            m[i][j] = INT_MAX;
            for (int k = i; k <= j - 1; k++) {
                int q = m[i][k] + m[k + 1][j] + p[i - 1] * p[k] * p[j];
                if (q < m[i][j]) {
                    m[i][j] = q;
                    s[i][j] = k;
                }
            }
        }
    }
    
    printf("Minimum number of scalar multiplications is: %d\n", m[1][n]);
    printf("Optimal Parenthesization is: ");
    printOptimalParens(1, n, n+1, s);
    printf("\n");
}

int main() {
    int p[] = {13, 5, 89, 3, 34};
    int n = sizeof(p) / sizeof(p[0]) - 1; 
    matrixChainOrder(p, n);
    return 0;
}
