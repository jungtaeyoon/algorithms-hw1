#include "sort.h"
#include <stdlib.h>
#include <stdio.h>

void bubbleSort(int a[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        /* 한 번 훑을 때마다 가장 큰 값이 뒤로 밀려 자리를 잡는다. */
        for (int j = 0; j < n - 1 - i; j++) {
            if (a[j] > a[j + 1]) {
                int tmp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = tmp;
                swapped = 1;
            }
        }
        /* 한 바퀴 동안 교환이 없었다면 이미 정렬된 것이다. */
        if (!swapped) {
            return;
        }
    }
}
///
void insertionSort(int a[], int n) {
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;

        /* key보다 큰 원소를 오른쪽으로 한 칸씩 이동 */
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }

        /* 비워진 위치에 key 삽입 */
        a[j + 1] = key;
    }
}
///
/* 정렬된 두 구간을 합친다.
   구간: [left, mid), [mid, right) */
static void merge(int a[], int temp[],
                  int left, int mid, int right) {
    int i = left;
    int j = mid;
    int k = left;

    while (i < mid && j < right) {
        if (a[i] <= a[j]) {
            temp[k++] = a[i++];
        } else {
            temp[k++] = a[j++];
        }
    }

    while (i < mid) {
        temp[k++] = a[i++];
    }

    while (j < right) {
        temp[k++] = a[j++];
    }

    for (int p = left; p < right; p++) {
        a[p] = temp[p];
    }
}

/* right는 구간에 포함되지 않는다. */
static void mergeSortRange(int a[], int temp[],
                           int left, int right) {
    if (right - left <= 1) {
        return;
    }

    int mid = left + (right - left) / 2;

    mergeSortRange(a, temp, left, mid);
    mergeSortRange(a, temp, mid, right);
    merge(a, temp, left, mid, right);
}

void mergeSort(int a[], int n) {
    if (n <= 1) {
        return;
    }

    /* 임시 배열은 한 번만 할당해서 계속 사용 */
    int *temp = malloc((size_t)n * sizeof(*temp));

    if (temp == NULL) {
        fprintf(stderr, "병합 정렬: 메모리 할당 실패\n");
        exit(EXIT_FAILURE);
    }

    mergeSortRange(a, temp, 0, n);
    free(temp);
}
///
/* 부모가 자식보다 크거나 같도록 최대 힙을 복구 */
static void siftDown(int a[], int n, int root) {
    /* 자식이 있는 노드에서만 반복 */
   while (root < n / 2) 
    {
        int child = 2 * root + 1;  /* 왼쪽 자식 */

        /* 두 자식 중 더 큰 자식을 선택 */
        if (child + 1 < n && a[child + 1] > a[child]) {
            child++;
        }

        if (a[root] >= a[child]) {
            return;
        }

        int temp = a[root];
        a[root] = a[child];
        a[child] = temp;

        root = child;
    }
}

void heapSort(int a[], int n) {
    if (n <= 1) {
        return;
    }

    /* 아래쪽 부모부터 최대 힙 만들기 */
    for (int i = n / 2 - 1; i >= 0; i--) {
        siftDown(a, n, i);
    }

    /* 최댓값을 배열 뒤로 보내고 남은 힙 복구 */
    for (int end = n - 1; end > 0; end--) {
        int temp = a[0];
        a[0] = a[end];
        a[end] = temp;

        siftDown(a, end, 0);
    }
}
