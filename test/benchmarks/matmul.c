void multiply(int A[100][100], int B[100][100], int C[100][100]) {
    for (int i = 0; i < 100; i++)
        for (int j = 0; j < 100; j++)
            for (int k = 0; k < 100; k++)
                C[i][j] += A[i][k] * B[k][j];
}
int main() {
    int A[100][100] = {0}, B[100][100] = {0}, C[100][100] = {0};
    for (int i = 0; i < 100; i++) multiply(A, B, C); 
    return 0;
}
