#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "sort.h"

/* 표준 정렬 함수 qsort에 사용할 비교 함수 */
static int compareInt(const void *p, const void *q) {
    int x = *(const int *)p;
    int y = *(const int *)q;
    return (x > y) - (x < y);
}
static int benchmarkSorts(void) {
    const int sizes[] = {1000, 5000, 10000};
    const char *states[] = {"random", "ascending", "descending"};
    const char *names[] = {"insertion", "merge", "heap"};

    void (*sorts[])(int[], int) = {
        insertionSort, mergeSort, heapSort
    };

    /* 실험용 배열: 세 정렬이 동일한 입력을 받도록 복사 */
    static int original[10000];
    static int work[10000];
    static int expected[10000];

    const int repeats = 5;

    FILE *fp = fopen("results.csv", "w");
    if (fp == NULL) {
        perror("results.csv");
        return 1;
    }

    srand(42);  /* 같은 실행 환경에서 입력을 재현하기 위한 시드 */

    printf("\n[성능 비교: 평균 실행 시간]\n");
    printf("size,state,algorithm,mean_ms\n");
    fprintf(fp, "size,state,algorithm,mean_ms\n");

    for (int si = 0; si < 3; si++) {
        int n = sizes[si];
        size_t bytes = (size_t)n * sizeof(int);

        for (int state = 0; state < 3; state++) {
            double total[3] = {0.0, 0.0, 0.0};

            for (int trial = 0; trial < repeats; trial++) {
                /* 입력 생성: 측정 시간에서 제외 */
                for (int i = 0; i < n; i++) {
                    if (state == 0) {
                        original[i] = rand() % 100000;
                    } else if (state == 1) {
                        original[i] = i;
                    } else {
                        original[i] = n - i;
                    }
                }

                /* 정답 생성: 측정 시간에서 제외 */
                memcpy(expected, original, bytes);
                qsort(expected, (size_t)n,
                      sizeof(int), compareInt);

                /* 실행 순서를 바꿔 특정 정렬이 늘 먼저 돌지 않게 함 */
                for (int step = 0; step < 3; step++) {
                    int s = (trial + step) % 3;

                    /* 원본 복사: 측정 시간에서 제외 */
                    memcpy(work, original, bytes);

                    clock_t start = clock();
                    sorts[s](work, n);
                    clock_t finish = clock();

                    if (start == (clock_t)-1 ||
                        finish == (clock_t)-1) {
                        fprintf(stderr, "시간 측정 실패\n");
                        fclose(fp);
                        return 1;
                    }

                    double ms =
                        1000.0 * (double)(finish - start)
                        / CLOCKS_PER_SEC;

                    total[s] += ms;

                    /* 결과 확인도 측정 시간에서 제외 */
                    for (int i = 0; i < n; i++) {
                        if (work[i] != expected[i]) {
                            fprintf(stderr,
                                    "정렬 실패: %s, n=%d, %s\n",
                                    names[s], n, states[state]);
                            fclose(fp);
                            return 1;
                        }
                    }
                }
            }

            for (int s = 0; s < 3; s++) {
                double mean = total[s] / repeats;

                printf("%d,%s,%s,%.6f\n",
                       n, states[state], names[s], mean);

                fprintf(fp, "%d,%s,%s,%.6f\n",
                        n, states[state], names[s], mean);
            }
        }
    }

    if (fclose(fp) != 0) {
        perror("results.csv 저장");
        return 1;
    }

    printf("\n결과를 results.csv에 저장했습니다.\n");
    return 0;
}

int main(void) {
    struct {
        const char *name;
        int data[10];
        int n;
    } cases[] = {
        {"무작위",    {6, 8, 5, 9, 10, 1, 7, 2, 4, 3}, 10},
        {"오름차순",  {1, 2, 3, 4, 5},                   5},
        {"역순",      {5, 4, 3, 2, 1},                   5},
        {"중복",      {3, 1, 3, 2, 1, 3},                6},
        {"음수 포함", {-3, 0, 2, -1, 2, -5},             6},
        {"원소 하나", {7},                               1},
        {"빈 배열",   {0},                               0}
    };

    /* 동일한 형태의 정렬 함수들을 배열로 묶음 */
    void (*sorts[])(int[], int) = {
        insertionSort, mergeSort, heapSort
    };

    const char *names[] = {
        "삽입 정렬", "병합 정렬", "힙 정렬"
    };

    int caseCount = (int)(sizeof(cases) / sizeof(cases[0]));
    int failures = 0;

    for (int s = 0; s < 3; s++) {
        printf("\n[%s]\n", names[s]);

        for (int c = 0; c < caseCount; c++) {
            int actual[10];
            int expected[10];
            int n = cases[c].n;
            size_t bytes = (size_t)n * sizeof(int);

            /* 두 정렬에 같은 원본 데이터를 복사 */
            memcpy(actual, cases[c].data, bytes);
            memcpy(expected, cases[c].data, bytes);

            sorts[s](actual, n);
            qsort(expected, (size_t)n, sizeof(int), compareInt);

            /* 표준 정렬 결과와 원소별 비교 */
            int passed = 1;
            for (int i = 0; i < n; i++) {
                if (actual[i] != expected[i]) {
                    passed = 0;
                    break;
                }
            }

            printf("%s: %s\n",
                   cases[c].name, passed ? "PASS" : "FAIL");

            if (!passed) {
                failures++;
            }
        }
    }

    printf("\n총 %d개 검사, 실패 %d개\n",
           3 * caseCount, failures);

    /* 정확성 검사가 통과한 경우에만 성능 비교 */
if (failures != 0) {
    return EXIT_FAILURE;
}

return benchmarkSorts() == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
