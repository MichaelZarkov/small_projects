
namespace ArrFun
{
    // Should try to make all functions template.
    public static class Manip    // Static class because every method operates on external objects.
    {
        // Reverses an array.
        public static void Reverse(int[] arr)
        {
            for (int i = 0; i < arr.Length / 2; i++)
            {
                // Swap.
                (arr[i], arr[arr.Length - 1 - i]) = (arr[arr.Length - 1 - i], arr[i]);
            }
        }

        // This implementation is from "Лекции по Алгоритми, 2023 година, Минко Марков"
        public static void InsertionSort(int[] arr)
        {
            for(int i = 1; i < arr.Length; i++)
            {
                int key = arr[i];
                int j = i - 1;
                while (j >= 0 && arr[j] > key)
                {
                    arr[j + 1] = arr[j];
                    j--;
                }
                arr[j + 1] = key;
            }
        }

        // This implementation is from "Лекции по Алгоритми, 2023 година, Минко Марков"
        // Moves elements in the array only with swaps.
        public static void SelectionSort1(int[] arr)
        {
            for (int i = 0; i < arr.Length - 1; i++)
                for (int j = i + 1; j < arr.Length; j++)
                    if (arr[j] < arr[i])
                        (arr[i], arr[j]) = (arr[j], arr[i]);    // Swap (see 'tuple' for more info).
        }

        // Another implementation which first finds min element and then swaps.
        public static void SelectionSort2(int[] arr)
        {
            for (int i = 0; i < arr.Length - 1; i++)
            {
                int min_index = i;
                for (int j = i + 1; j < arr.Length; j++)
                    if (arr[j] < arr[min_index])
                        min_index = j;

                if (min_index != i)
                    (arr[i], arr[min_index]) = (arr[min_index], arr[i]);
            }
        }


        // Returns true if 'index' is not a leaf node.
        static bool IsNotLeaf(int arrLength, int index)
        { return (2 * index + 1) < arrLength; }
        
        // 'i' - index in 'arr'
        // If LeftChild(index) and RightChild(index) exist they must be piramids for the function to work.
        static void HeapifyIterative(int[] arr, int arrLen, int i)
        {
            while (IsNotLeaf(arrLen, i))
            {
                int largest = i;         // Index of the largest from arr[i], arr[left], arr[right].
                int left = 2 * i + 1;    // Index of the left child.
                int right = left + 1;    // Index of the rights child.

                if (arr[left] > arr[i])    // No need to check if left child exists because 'i' is not a leaf.
                    largest = left;

                if (right < arrLen && arr[right] > arr[largest])
                    largest = right;

                if (largest != i)
                {
                    (arr[largest], arr[i]) = (arr[i], arr[largest]);
                    i = largest;    // Go down the tree.
                }
                else { break; }    // Break if the tree with root 'i' is already a piramid.
            }
        }
        static void BuildHeap(int[] arr)
        {
            // Start from the last node that is not a leaf and go up.
            for (int i = arr.Length / 2 - 1; i >= 0; i--)
                HeapifyIterative(arr, arr.Length, i);
        }
        public static void Heapsort(int[] arr)
        {
            BuildHeap(arr);
            for (int i = arr.Length - 1; i > 0; i--)
            {
                (arr[0], arr[i]) = (arr[i], arr[0]);    // Place the root of the piramid in its place in the sorted array.
                HeapifyIterative(arr, i, 0);
            }
        }


        // arr[beg], arr[beg+1], ... , arr[mid] and arr[mid+1], ... , arr[end] have to be sorted arrays.
        // The function merges them in a single sorted array and puts it in arr[beg], ..., arr[end].
        static void Merge(int[] arr, int[] buff, int beg, int mid, int end)
        {
            // Iterators for the result, left and right arrays.
            int result = beg;
            int left = beg;
            int right = mid + 1;
            while (left <= mid && right <= end)
                buff[result++] = arr[left] < arr[right] ? arr[left++] : arr[right++];

            while (left <= mid)
                buff[result++] = arr[left++];

            while (right <= end)
                buff[result++] = arr[right++];

            // Copy the merged arrays.
            for (int i = beg; i <= end; i++)
                arr[i] = buff[i];
        }
        static void MergesortStep(int[] arr, int[] buff, int beg, int end)
        {
            if (beg >= end)    // Arrays with one or zero element are trivially sorted.
                return;

            int mid = (beg + end) / 2;

            // Sort left and right subarrays.
            MergesortStep(arr, buff, beg, mid);
            MergesortStep(arr, buff, mid + 1, end);

            // Merge left and right subarrays.
            Merge(arr, buff, beg, mid, end);
        }
        public static void Mergesort(int[] arr)
        {
            int[] buff = new int[arr.Length];    // Used in 'Merge()'.

            MergesortStep(arr, buff, 0, arr.Length - 1);
        }
    }
}