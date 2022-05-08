/**
 * @file crackme_joearbo13.c
 * @author Joseph Arbolino
 * @brief Compiled with "cl crackme_joearbo13.c"
 * 
 *        There are many keys, the key must be 32 letters long.
 *        The key will include lowercase and uppercase letters.
 *        The answer is calculated by taking the sum of the first 16 
 *        letters and the sum of the second 16 letters, squaring each
 *        them, and them adding the two values together. That value
 *        is then sqrt to perform the pythagorean theorem.
 * 
 *        Equation: ans = [(sum of 1st 16 chars)^2 + (sum of 2nd 16 chars)^2]^0.5
 *        Example Keys: vARlDWzDXfzQMdJubtRFSnVDnwhxZOCk
 *                      LLobXXLPmyrhnyeHXvgILLJNMDcHMzzt
 *                      lUEtwkoqFWnrZDzcZhEQGdXmcKPVHmOT
 *                      udTqawtFEIxBjdYiRDonFPeoCniQUFMr
 * @date 2022-02-25
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int* getNums(char * inputKey){
    int *nums = malloc(sizeof(int)*2);
    nums[0] = 0;
    nums[1] = 0;
    for (int i = 0; i < 16; ++i){
        nums[0] += inputKey[i];
        nums[1] += inputKey[i+16];
    }
    return nums;
}

void loseCond() {
    printf("Wrong! Try Again!\n");
}

void winCond(int ans) {
    double sqrt = pow(ans, 0.5);
    if (sqrt < 2115 && sqrt > 2110) {
        printf("Correct! Good Job!\n");
    } else 
    if (ans > 0) {
        loseCond();
    }
}

void calculate(int *nums) {
    int num1 = pow(nums[0], 2);
    int num2 = pow(nums[1], 2);
    winCond(num1 + num2);
}

void red_herring(int *nums) {
    int ans = nums[0] + nums[1] + (nums[1]*2);
    ans = -1 * ans;
    winCond(ans);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Incorrect Number of Arguments\n");
        return 1;
    }

    char *inputKey = argv[1];
    if (strlen(inputKey) != 32) {
        loseCond();
        return 1;
    }
        
    int *nums = getNums(inputKey);
    calculate(nums);
    red_herring(nums);

    return 0;
}