#include <stdio.h>
#include "restart_case.h"

struct e1{};
struct e2{};
struct e3{};

template<typename RC>
int fn(int* arr, int sz, RC rc) {
    int accum = 0;
    int i;
    for(i = 0; i < sz; i++) {
        int val = arr[i];
        if(val == 0) val = rc(e1{}, arr, i);
        accum += 10 / val;
    }
    for(i = 0; i < sz; i++) {
        int val = arr[i];
        if(val == 0) val = rc(e2{}, arr, i);
        accum *= val;
    }
    // here we fall into default case
    int j = rc(e3{}, arr, i);
    return accum;
}

int main(int, char **)
{
    auto&& hc = restart_case(
        std::tuple{e1{}, [](int*, int i) -> int {
            printf("found 0 for division at %d\n", i);
            return 10;
        }},
        std::tuple{e2{}, [](int*, int i) -> int {
            printf("found 0 for multiplication at %d\n", i);
            return 1;
        }},
        //default case
        std::tuple{any{}, [](...){
            printf("Jocker\n");
        }}
    );

    int arr[] = {1,2,3,4,5,0,4,5,6,0,1,2,3,0,9};

    printf("result = %d\n", fn(arr, sizeof(arr)/sizeof(arr[0]), hc));
    return 0;
}
