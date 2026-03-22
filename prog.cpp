#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <time.h>
#include <random>

using namespace std;

// 1 = Insertion Sort based
// 2 = Quick Sort based
// 3 = Magic Fives (Median of Medians)
// 4 = Probabilistic based on sampling
const int SELECTED_ALGO = 2;

// =====================================================================
// 1. Insertion Sort Selection
// =====================================================================
int insertionSortSelect(vector<int> arr, int k) { // Pass by value to avoid modifying original
    for (size_t i = 1; i < arr.size(); ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
    return arr[k - 1];
}

// =====================================================================
// 2. QuickSelect (Randomized QuickSort-based)
// =====================================================================
int quickSelect(vector<int>& arr, int left, int right, int k) {
    if (left == right) return arr[left - 1];

    int pivotIndex = left + rand() % (right - left + 1);
    int pivotValue = arr[pivotIndex];

    swap(arr[pivotIndex], arr[right]);
    int storeIndex = left;
    for (int i = left; i < right; i++) {
        if (arr[i] < pivotValue) {
            swap(arr[i], arr[storeIndex]);
            storeIndex++;
        }
    }
    swap(arr[storeIndex], arr[right]);

    if (k == storeIndex) return arr[k - 1];
    else if (k < storeIndex) return quickSelect(arr, left, storeIndex - 1, k);
    else return quickSelect(arr, storeIndex + 1, right, k);
}

// =====================================================================
// 3. Magic Fives (Median of Medians - Deterministic)
// =====================================================================
int findMedian(vector<int>& arr, int left, int n) {
    sort(arr.begin() + left, arr.begin() + left + n);
    return arr[left + n / 2 - 1];
}

int magicFives(vector<int>& arr, int left, int right, int k) {
    int n = right - left + 1;
    if (k >= 0 && k < n) {
        vector<int> median((n + 4) / 5);
        int i;
        for (i = 0; i < n / 5; i++) {
            median[i] = findMedian(arr, left + i * 5, 5);
        }
        if (i * 5 < n) {
            median[i] = findMedian(arr, left + i * 5, n % 5);
            i++;
        }

        int medOfMed = (i == 1) ? median[i - 1] : magicFives(median, 0, i - 1, i / 2);

        int pos = left;
        for (int j = left; j <= right; j++) {
            if (arr[j] == medOfMed) {
                swap(arr[j], arr[right]);
                break;
            }
        }

        int storeIndex = left;
        for (int j = left; j < right; j++) {
            if (arr[j] < medOfMed) {
                swap(arr[j], arr[storeIndex]);
                storeIndex++;
            }
        }
        swap(arr[storeIndex], arr[right]);

        if (storeIndex - left == k) return arr[storeIndex - 1];
        if (storeIndex - left > k) return magicFives(arr, left, storeIndex - 1, k);
        return magicFives(arr, storeIndex + 1, right, k - storeIndex + left - 1);
    }
    return -1;
}

// =====================================================================
// 4. Floyd-Rivest (Probabilistic Sampling)
// =====================================================================
int sign(double x) { return (x > 0) ? 1 : ((x < 0) ? -1 : 0); }

int floydRivest(vector<int>& arr, int left, int right, int k) {
    while (right > left) {
        if (right - left > 600) {
            double n = right - left + 1;
            double i = k - left + 1;
            double z = log(n);
            double s = 0.5 * exp(2.0 * z / 3.0);
            double sd = 0.5 * sqrt(z * s * (n - s) / n) * sign(i - n / 2.0);
            int newLeft = max((double)left, k - i * s / n + sd);
            int newRight = min((double)right, k + (n - i) * s / n + sd);
            floydRivest(arr, newLeft, newRight, k);
        }
        int t = arr[k];
        int i = left;
        int j = right;
        swap(arr[left], arr[k]);
        if (arr[right] > t) swap(arr[right], arr[left]);
        while (i < j) {
            swap(arr[i], arr[j]);
            i++; j--;
            while (arr[i] < t) i++;
            while (arr[j] > t) j--;
        }
        if (arr[left] == t) swap(arr[left], arr[j]);
        else { j++; swap(arr[j], arr[right]); }
        if (j <= k) left = j + 1;
        if (k <= j) right = j - 1;
    }

    return arr[k - 1];
}

// Wrapper function to find the k-th smallest element
int kthSmallest(vector<int>& arr, int k) {
    int n = arr.size();

	switch (SELECTED_ALGO) {
    case 1:
        return insertionSortSelect(arr, k);
    case 2:
        return quickSelect(arr, 0, n - 1, k);
    case 3:
        return magicFives(arr, 0, n - 1, k);
    case 4:
        return floydRivest(arr, 0, n - 1, k);
	default:
		cout << "Invalid algorithm selection!" << endl;
		return 0;
	}
}


// Test the implementation
int main() {
	int z, n, k, x;
	vector<int> arr;
	cin >> z;
	int j = 0;
	while(j < z)
	{
		j++;
		cout << "Data set:"<< j << "\n";
		cin >> n >> k;
		cout << "Reading the input (size:" << n <<")....\n";
		arr.clear();
		auto start = std::chrono::high_resolution_clock::now();
		for(int i=0; i< n; i++)
		{	
			cin >> x;
			arr.push_back(x);
		}
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = end - start;
		cout << "Reading time:" << duration.count() << endl; 
		
		start = std::chrono::high_resolution_clock::now();
        cout << "Element at index " << k << ": ";
		cout << kthSmallest(arr, k) << endl;
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;
		cout << "Execution time:" << duration.count() << endl; 
		
	}
    return 0;
}
