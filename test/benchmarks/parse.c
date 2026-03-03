int main() {
    char buffer[10000];
    int sum = 0;
    for(int j=0; j<1000; j++) {
        for(int i=0; i<10000; i++) {
            buffer[i] = (i % 26) + 'a';
            if (buffer[i] == 'z') sum++;
        }
    }
    return sum > 0 ? 0 : 1;
}
