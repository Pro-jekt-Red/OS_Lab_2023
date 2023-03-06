#include <stdio.h>
int isPalindrome(int n) {
	int temp = n;
	int reverse = 0;
	while (temp != 0) {
		reverse = reverse * 10 + temp % 10;
		temp /= 10;
	}
	if (reverse == n) {
		return 1;
	} else {
		return 0;
	}
}
int main() {
	int n;
	scanf("%d", &n);

	if (isPalindrome(n)) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
