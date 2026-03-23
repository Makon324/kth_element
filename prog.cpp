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
const int SELECTED_ALGO = 4;

// =====================================================================
// 1. Insertion Sort Selection
// =====================================================================
int insertionSortSelect(vector<int> arr, int k) { // Pass by value to avoid modifying original
    int n = arr.size();

    for (int i = 0; i < k; i++){
        for (int j = i + 1; j < n; j++){
            if (arr[i] > arr[j]){
                swap(arr[i], arr[j]);
            }
        }
    }

    return arr[k - 1];
}

// =====================================================================
// 2. QuickSelect (Randomized QuickSort-based)
// =====================================================================
int randomPartition(vector<int>& arr, int left, int right) { // Pick a random pivot    
    int pivotIndex = left + rand() % (right - left + 1);
    swap(arr[pivotIndex], arr[right]); // Move pivot to end
    
    int pivotValue = arr[right];
    int i = left;
    
    for (int j = left; j < right; j++) {
        if (arr[j] <= pivotValue) {
            swap(arr[i], arr[j]);
            i++;
        }
    }
    swap(arr[i], arr[right]); // Move pivot to its final place
    return i;
}

int quickSelect(vector<int>& arr, int left, int right, int k) {
    if (left == right) {
        return arr[left];
    }
    
    int pivotIndex = randomPartition(arr, left, right);
    int targetIndex = k - 1;
    
    if (targetIndex == pivotIndex) {
        return arr[pivotIndex];
    } else if (targetIndex < pivotIndex) {
        return quickSelect(arr, left, pivotIndex - 1, k);
    } else {
        return quickSelect(arr, pivotIndex + 1, right, k);
    }
}

// =====================================================================
// 3. Magic Fives (Median of Medians - Deterministic)
// =====================================================================

// Finds the median for a small subarray (up to 5 elements)
int findSmallMedian(vector<int>& arr, int left, int right) {
    sort(arr.begin() + left, arr.begin() + right + 1);
    return arr[left + (right - left) / 2];
}

// Partitions the array around specific pivot value
int partitionByValue(vector<int>& arr, int left, int right, int pivotValue) {
    // Find the pivot value in the array and move it to the end
    for (int i = left; i <= right; i++) {
        if (arr[i] == pivotValue) {
            swap(arr[i], arr[right]);
            break;
        }
    }
    
    // Standard partitioning
    int i = left;
    for (int j = left; j < right; j++) {
        if (arr[j] <= pivotValue) {
            swap(arr[i], arr[j]);
            i++;
        }
    }
    swap(arr[i], arr[right]);
    return i;
}

int magicFives(vector<int>& arr, int left, int right, int k) {
    int n = right - left + 1;
    
    // Base case: if there are 5 or fewer elements, sort and return the k-th element.
    if (n <= 5) {
        sort(arr.begin() + left, arr.begin() + right + 1);
        return arr[k - 1]; 
    }
    
    // Divide into groups of 5 and find their medians
    vector<int> medians;
    for (int i = 0; i < n / 5; i++) {
        medians.push_back(findSmallMedian(arr, left + i * 5, left + i * 5 + 4));
    }
    
    // Handle the remainder (if n is not divisible by 5, the last group is smaller)
    if (n % 5 != 0) {
        medians.push_back(findSmallMedian(arr, left + (n / 5) * 5, right));
    }
    
    // Find the median of medians recursively. 
    int mediansCount = medians.size();
    int pivotValue = magicFives(medians, 0, mediansCount - 1, mediansCount / 2 + 1);
    
    // Partition the original array around the found pivot value
    int pivotIndex = partitionByValue(arr, left, right, pivotValue);
    
    int targetIndex = k - 1;
    
    // Check which part of the array contains our target element
    if (targetIndex == pivotIndex) {
        return arr[pivotIndex];
    } else if (targetIndex < pivotIndex) {
        return magicFives(arr, left, pivotIndex - 1, k);
    } else {
        return magicFives(arr, pivotIndex + 1, right, k);
    }
}

// =====================================================================
// 4. Floyd-Rivest (Probabilistic Sampling)
// =====================================================================
int floydRivest(vector<int>& arr, int left, int right, int k) {
    int targetIndex = k - 1;
    
    while (right > left) {
        // If the subarray is large enough, use sampling to find a good pivot
        if (right - left > 600) {
            double n = right - left + 1;
            double i = targetIndex - left + 1;
            
            // Calculate the sample size and bounds using standard Floyd-Rivest math
            double z = std::log(n);
            double s = 0.5 * std::exp(2.0 * z / 3.0);
            double sd = 0.5 * std::sqrt(z * s * (n - s) / n) * ((i - n / 2.0 < 0) ? -1.0 : 1.0);
            
            int newLeft = std::max(left, (int)(targetIndex - i * s / n + sd));
            int newRight = std::min(right, (int)(targetIndex + (n - i) * s / n + sd));
            
            // Recursively run the algorithm on the selected sample
            floydRivest(arr, newLeft, newRight, k);
        }
        
        // Use the element at targetIndex as our pivot 
        // (it's now a highly accurate estimate thanks to the sampling step)
        int pivotValue = arr[targetIndex];
        
        // Partition the array around the pivotValue
        int pivotLoc = left;
        
        // Move pivot out of the way to the end
        for (int j = left; j <= right; j++) {
            if (arr[j] == pivotValue) {
                swap(arr[j], arr[right]);
                break;
            }
        }
        
        // Standard partitioning
        int i = left;
        for (int j = left; j < right; j++) {
            if (arr[j] <= pivotValue) {
                swap(arr[i], arr[j]);
                i++;
            }
        }
        swap(arr[i], arr[right]); // Put pivot in its final sorted place
        pivotLoc = i;
        
        // Narrow down the search range iteratively
        if (pivotLoc == targetIndex) {
            return arr[pivotLoc];
        } else if (targetIndex < pivotLoc) {
            right = pivotLoc - 1;
        } else {
            left = pivotLoc + 1;
        }
    }
    
    // Base case when left == right
    return arr[left];
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
