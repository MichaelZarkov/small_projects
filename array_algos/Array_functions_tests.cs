namespace Array_manipulations_tests
{
    [TestClass]
    public class Array_functions_tests
    {
        [TestMethod]
        public void ReverseTest()
        {
            // Arrange.
            int[] input = { 1, 2, 3 };
            int[] expected = { 3, 2, 1 };

            // Act.
            ArrFun.Manip.Reverse(input);

            // Assert.
            bool areEqual = Enumerable.SequenceEqual(input, expected);    // Compare.
            Assert.IsTrue(areEqual);
        }

        [TestMethod]
        public void InsertionSortTest()
        {
            // Arrange.
            int[] input = { -7, 29, 33, 8 };
            int[] expected = { -7, 8, 29, 33 };

            // Act.
            ArrFun.Manip.InsertionSort(input);

            // Assert.
            Assert.IsTrue(Enumerable.SequenceEqual(input, expected));
        }

        [TestMethod]
        public void SelectionSort1SortTest()
        {
            // Arrange.
            int[] input = { 0, 0, 0, -7, -7 };
            int[] expected = { -7, -7, 0, 0, 0 };

            // Act.
            ArrFun.Manip.SelectionSort1(input);

            // Assert.
            Assert.IsTrue(Enumerable.SequenceEqual(input, expected));
        }

        [TestMethod]
        public void SelectionSort2SortTest()
        {
            // Arrange.
            int[] input = { 0, 0, 0, -7, -7 };
            int[] expected = { -7, -7, 0, 0, 0 };

            // Act.
            ArrFun.Manip.SelectionSort2(input);

            // Assert.
            Assert.IsTrue(Enumerable.SequenceEqual(input, expected));
        }

        [TestMethod]
        public void HeapsortTest()
        {
            int[] input1 = new int[0];
            int[] input2 = { 1, 2, 3, 4, 5 };
            int[] input3 = { -2, -2, 9, 9, -2 };

            int[] output2 = { 1, 2, 3, 4, 5 };
            int[] output3 = { -2, -2, -2, 9, 9 };

            ArrFun.Manip.Heapsort(input1);    // This will throw exception if something is not right.
            ArrFun.Manip.Heapsort(input2);
            ArrFun.Manip.Heapsort(input3);

            Assert.IsTrue(Enumerable.SequenceEqual(input2, output2));
            Assert.IsTrue(Enumerable.SequenceEqual(input3, output3));
        }

        [TestMethod]
        public void MergesortTest()
        {
            int[] input1 = new int[0];
            int[] input2 = { 1, 2, 3, 4, 5 };
            int[] input3 = { -2, -2, 9, 9, -2 };

            int[] output2 = { 1, 2, 3, 4, 5 };
            int[] output3 = { -2, -2, -2, 9, 9 };

            ArrFun.Manip.Mergesort(input1);    // This will throw exception if something is not right.
            ArrFun.Manip.Mergesort(input2);
            ArrFun.Manip.Mergesort(input3);

            Assert.IsTrue(Enumerable.SequenceEqual(input2, output2));
            Assert.IsTrue(Enumerable.SequenceEqual(input3, output3));
        }

        [TestMethod]
        public void QuicksortTest()
        {
            int[] input1 = new int[0];
            int[] input2 = { 1, 2, 3, 4, 5 };
            int[] input3 = { -2, -2, 9, 9, -2 };

            int[] output2 = { 1, 2, 3, 4, 5 };
            int[] output3 = { -2, -2, -2, 9, 9 };

            ArrFun.Manip.Quicksort(input1);    // This will throw exception if something is not right.
            ArrFun.Manip.Quicksort(input2);
            ArrFun.Manip.Quicksort(input3);

            Assert.IsTrue(Enumerable.SequenceEqual(input2, output2));
            Assert.IsTrue(Enumerable.SequenceEqual(input3, output3));
        }

        [TestMethod]
        public void CompareSortingAlgs()
        {
            // Make a bunch of arrays and test if the sorting algos agree
            // on the result.
            var rand = new Random();
            const int upTo = 100;      // Test with arrays with length up to this number.
            for (int i = 0; i < upTo; i++)
            {
                int[] insertionsort = new int[i];
                int[] selectionsort = new int[i];
                int[] heapsort = new int[i];
                int[] mergesort = new int[i];
                int[] quicksort = new int[i];

                // Fill with random numbers.
                for (int j = 0; j < insertionsort.Length; j++)
                {
                    insertionsort[j] = rand.Next();
                    selectionsort[j] = insertionsort[j];
                    heapsort[j] = insertionsort[j];
                    mergesort[j] = insertionsort[j];
                    quicksort[j] = insertionsort[j];
                }

                // Act.
                ArrFun.Manip.InsertionSort(insertionsort);
                ArrFun.Manip.SelectionSort1(selectionsort);
                ArrFun.Manip.Heapsort(heapsort);
                ArrFun.Manip.Mergesort(mergesort);
                ArrFun.Manip.Quicksort(quicksort);

                // Test if sorting algos agree with each other.
                Assert.IsTrue(Enumerable.SequenceEqual(insertionsort, selectionsort));
                Assert.IsTrue(Enumerable.SequenceEqual(selectionsort, heapsort));
                Assert.IsTrue(Enumerable.SequenceEqual(heapsort, mergesort));
                Assert.IsTrue(Enumerable.SequenceEqual(mergesort, quicksort));
            }
        }
    }
}