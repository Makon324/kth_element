#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <time.h>
#include <random>

using namespace std;

// Wrapper function to find the k-th smallest element
int kthSmallest(vector<int>& arr, int k) {
	
	/* write all the necessary code here */
	
	//quicksort(arr, 0, arr.size()-1); return arr[k];
	//insertsort(arr, 0, arr.size()-1); return arr[k];
		
	//return medianOfMedians(arr, 0, arr.size() - 1, k);
	//return myFunction(arr, 0, arr.size()-1, k-1);
	
	return 0;
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
		//cout << "Data set:"<< j << "\n";
		cin >> n >> k;
		//cout << "Reading the input (size:" << n <<")....\n";
		arr.clear();
		auto start = std::chrono::high_resolution_clock::now();
		for(int i=0; i< n; i++)
		{	
			cin >> x;
			arr.push_back(x);
		}
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = end - start;
		//cout << "Reading time:" << duration.count() << endl; 
		
		start = std::chrono::high_resolution_clock::now();
		//cout << "Element at index "<<k <<": "<< 
		cout << kthSmallest(arr, k) << endl;
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;
		//cout << "Execution time:" << duration.count() << endl; 
		
	}
    return 0;
}
