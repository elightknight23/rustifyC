void swap(int* a, int* b) { int t = *a; *a = *b; *b = t; }
int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) { i++; swap(&arr[i], &arr[j]); }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
int main() {
    int arr[1000];
    for(int i=0; i<1000; i++) arr[i] = 1000-i;
    for(int i=0; i<1000; i++) quickSort(arr, 0, 999);
    return 0;
}
