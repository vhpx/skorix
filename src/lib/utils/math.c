#include "../headers/math.h"

int abs(int x) { return (x < 0) ? -x : x; }
int max(int a, int b) { return (a > b) ? a : b; }
int min(int a, int b) { return (a < b) ? a : b; }
float sqrt(int x) {
  if (x == 0 || x == 1) {
    return x;
  }

  int start = 1, end = x, ans;
  while (start <= end) {
    int mid = (start + end) / 2;

    // If x is a perfect square
    if (mid * mid == x) {
      return mid;
    }

    // Since we need floor, we update answer when mid*mid is smaller than x, and
    // move closer to sqrt(x)
    if (mid * mid < x) {
      start = mid + 1;
      ans = mid;
    } else { // If mid*mid is greater than x
      end = mid - 1;
    }
  }
  return ans;
}