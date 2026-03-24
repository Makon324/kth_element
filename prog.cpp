#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <time.h>
#include <random>

using namespace std;

random_device rd;
mt19937 gen(rd());

// 1 = Insertion Sort based
// 2 = Quick Sort based
// 3 = Magic Fives (Median of Medians)
// 4 = Probabilistic based on sampling
const int SELECTED_ALGO = 4;

// =====================================================================
// 1. Insertion Sort Selection
// =====================================================================
int insertionSortSelect(vector<int>& arr, int k) {
    int n = arr.size();

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }

    return arr[k - 1];
}

// =====================================================================
// 2. QuickSelect (Randomized QuickSort-based)
// =====================================================================

// 3-way partitioning - returns: [start, end] of the range of elements equal to the pivot
pair<int, int> partition3Way(vector<int>& arr, int left, int right, int pivotValue) {
    int i = left;
    int j = left;
    int k = right;

    while (j <= k) {
        if (arr[j] < pivotValue) {
            swap(arr[i], arr[j]);
            i++;
            j++;
        }
        else if (arr[j] > pivotValue) {
            swap(arr[j], arr[k]);
            k--;
        }
        else {
            j++; // Element equals the pivot, leave it in the middle
        }
    }
    return { i, k };
}

int quickSelect(vector<int>& arr, int left, int right, int k) {
    if (left >= right) {
        return arr[left];
    }

    // Pick a random pivot value
    uniform_int_distribution<> dis(left, right);
    int pivotIndex = dis(gen);
    int pivotValue = arr[pivotIndex];

    // Partition the array
    pair<int, int> pivotRange = partition3Way(arr, left, right, pivotValue);
    int startEqual = pivotRange.first;
    int endEqual = pivotRange.second;

    int targetIndex = k - 1;

    // Determine which segment the target index falls into
    if (targetIndex >= startEqual && targetIndex <= endEqual) {
        return arr[targetIndex];
    }
    else if (targetIndex < startEqual) {
        return quickSelect(arr, left, startEqual - 1, k);
    }
    else {
        return quickSelect(arr, endEqual + 1, right, k);
    }
}

// =====================================================================
// 3. Magic Fives
// =====================================================================

// Sorts a small segment and returns the index of the median
int findSmallMedianIndex(vector<int>& arr, int left, int right) {
    sort(arr.begin() + left, arr.begin() + right + 1);
    return left + (right - left) / 2;
}

// Optimized findSmallMedianIndex for exactly 5 elements - fast
inline int findSmallMedianIndexFive(vector<int>& arr, int left) {
    int* a = &arr[left];
    
    #define CSWAP(i, j) \
        if (a[i] > a[j]) { \
            int tmp = a[i]; \
            a[i] = a[j]; \
            a[j] = tmp; \
        }

    CSWAP(0, 1); CSWAP(2, 3);
    CSWAP(0, 2); CSWAP(1, 4);
    CSWAP(1, 3); CSWAP(2, 4);
    CSWAP(1, 2); CSWAP(3, 4);
    CSWAP(2, 3);

    return left + 2;
}

int magic5sSelect(vector<int>& arr, int left, int right, int k);

// Find the perfect pivot
int getMagicPivot(vector<int>& arr, int left, int right) {
    int n = right - left + 1;

    int numFullGroups = n / 5;
    int remainder = n % 5;

    // Process all full groups of exactly 5 elements
    for (int i = 0; i < numFullGroups; i++) {
        int groupLeft = left + i * 5;
        int medianIndex = findSmallMedianIndexFive(arr, groupLeft);
        swap(arr[left + i], arr[medianIndex]);
    }

    // Process the leftover elements (if any)
    int totalGroups = numFullGroups;
    if (remainder > 0) {
        int groupLeft = left + numFullGroups * 5;
        int medianIndex = findSmallMedianIndex(arr, groupLeft, right);
        swap(arr[left + numFullGroups], arr[medianIndex]);
        totalGroups++;
    }

    // Look for the median of medians by calling the main search function for this small prefix
    int medianOfMediansIndex = left + totalGroups / 2;
    return magic5sSelect(arr, left, left + totalGroups - 1, medianOfMediansIndex + 1);
}

// MAIN FUNCTION
int magic5sSelect(vector<int>& arr, int left, int right, int k) {
    int n = right - left + 1;

    // Base case: for small arrays (5 or fewer), simply sort and return the k-th element
    if (n <= 5) {
        findSmallMedianIndex(arr, left, right);
        return arr[k - 1];
    }

    int pivotValue = getMagicPivot(arr, left, right);

    pair<int, int> pivotRange = partition3Way(arr, left, right, pivotValue);
    
    int targetIndex = k - 1;

    // Quick Select partitioning
    if (targetIndex >= pivotRange.first && targetIndex <= pivotRange.second) {
        return pivotValue;
    }
    else if (targetIndex < pivotRange.first) {
        return magic5sSelect(arr, left, pivotRange.first - 1, k);
    }
    else {
        return magic5sSelect(arr, pivotRange.second + 1, right, k);
    }
}

// =====================================================================
// 4. Floyd-Rivest (Probabilistic Sampling)
// =====================================================================
int floydRivest(vector<int>& arr, int left, int right, int k) {
    int n = right - left + 1;

    // If the subarray is small, probabilistic sampling is inefficient
    if (n <= 500) {
        return quickSelect(arr, left, right, k);
    }

    while (true) {
        // Choose multiset R of size ceil(n^(3/4))
        uniform_int_distribution<> dist(left, right);
        int s = ceil(std::pow(n, 0.75));
        vector<int> R;
        R.resize(s);        

        for (int i = 0; i < s; i++) {
            R[i] = arr[dist(gen)];
        }

        // Sort the sample R
        sort(R.begin(), R.end());

        // Determine bounding elements d and u based on the target rank        
		int local_target = (k - 1) - left; // index in the current subarray

        // Map the target rank to the expected rank in the sample R
        double r_target = (double)local_target * s / n;
        double offset = sqrt(n);

		// Calculate indices in R for bounding elements d and u
        int d_idx = max(0, (int)floor(r_target - offset));
        int u_idx = min(s - 1, (int)ceil(r_target + offset));

        int d = (r_target - offset < 0) ? std::numeric_limits<int>::min() : R[d_idx];
        int u = (r_target + offset >= s) ? std::numeric_limits<int>::max() : R[u_idx];

        // Construct set C and count elements
        vector<int> C;
        int ld = 0; // count of elements < d
        int lu = 0; // count of elements > u
        int equal_d = 0;
        int equal_u = 0;

        for (int i = left; i <= right; i++) {
            if (arr[i] < d) {
                ld++;
            }
            else if (arr[i] > u) {
                lu++;
            }
            else if (arr[i] == d) {
                equal_d++;
            }
            else if (arr[i] == u) {
                equal_u++;
            }
            else {
                C.push_back(arr[i]); // strictly between d and u
            }
        }

        // Check if target is perfectly hitting our boundary duplicates
        if (local_target >= ld && local_target < ld + equal_d) return d;
        if (local_target >= n - lu - equal_u && local_target < n - lu) return u;

		// CHECK FAILURE CONDITIONS

        // Failure: The target element falls outside our bounding elements d and u.
        if (local_target < ld || local_target >= n - lu) {
            continue; // Retry sampling
        }

        // Failure: The set C is too large to sort efficiently in sublinear time.
        // C should be: |C| <= 4 * n^(3/4)
        if (C.size() > 4 * s) {
            continue; // Retry sampling
        }

        // Success! Select the exact element in C
        int target_in_C = local_target - (ld + equal_d);
        return quickSelect(C, 0, C.size() - 1, target_in_C + 1);
    }
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
        return magic5sSelect(arr, 0, n - 1, k);
    case 4:
        return floydRivest(arr, 0, n - 1, k);
	default:
		cout << "Invalid algorithm selection!" << endl;
		return 0;
	}
}


// Test the implementation
int main() {
	// Speed up I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

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
