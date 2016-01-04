/* Synacor challenge
 *
 * mod-n (almost) Ackermann Calculator
 */
#include <stdio.h>
#include <string.h>

#define M 4
#define N 1
#define MOD_N 32768

int memo[M+1][MOD_N];
int z;

int ackermann(int m, int n) {
    //printf("ACK: %d %d\n", m, n);

    if (memo[m][n] > -1) {
        return memo[m][n];
    }

    if (!m) {
        memo[m][n] = (n + 1) % MOD_N;
        return memo[m][n];
    }

    if (!n) {
        memo[m][n] = ackermann(m-1, z); // <-- Ackermann has z == 1
        return memo[m][n];
    }

    // m > 0, n > 0
    memo[m][n] = ackermann(m-1, ackermann(m, n-1));

    return memo[m][n];
}

int main(int argc __attribute__((unused)), char** argv __attribute__((unused))) {
    for (z=1; z < MOD_N; z++) {
        memset(memo, -1, sizeof(int) * (M+1) * MOD_N);
        if (ackermann(M, N) == 6) {
            printf("R7: %d\n", z);
            break;
        }
    }
} 
