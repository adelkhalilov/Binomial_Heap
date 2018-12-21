#include "pch.h"
#include <gtest/gtest.h>
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE testBinomialHeap


#include "Header.h"


TEST(BinomialHeapBasics, Constructor) {
	binomial_heap<int> heap = binomial_heap<int>();
	EXPECT_EQ(heap.is_empty(), true);
}

TEST(BinomialHeapBasics, Empty) {
	binomial_heap<int> heap = binomial_heap<int>();
	EXPECT_EQ(heap.is_empty(), true);

	heap.insert(1);
	EXPECT_EQ(heap.is_empty(), false);
}

TEST(BinomialHeapBasics, Insert) {
	binomial_heap<int> heap = binomial_heap<int>();
	auto pointer = heap.insert(1);
	EXPECT_EQ(pointer->value(), 1);
}

TEST(BinomialHeapBasics, GetMin) {
	binomial_heap<int> heap = binomial_heap<int>();
	auto pointer = heap.insert(1);
	heap.insert(2);
	heap.insert(5);
	heap.insert(0);
	heap.insert(3);
	EXPECT_EQ(heap.get_min(), 0);
}

TEST(BinomialHeapBasics, ExtractMin) {
	binomial_heap<int> heap = binomial_heap<int>();
	heap.insert(2);
	heap.insert(3);
	heap.insert(5);
	heap.insert(10);
	heap.insert(3);
	heap.insert(11);
	EXPECT_EQ(heap.extract_min(), 2);
	EXPECT_EQ(heap.extract_min(), 3);
	EXPECT_EQ(heap.extract_min(), 3);
	heap.insert(1);
	EXPECT_EQ(heap.extract_min(), 1);
}

TEST(BinomialHeapBasics, Size) {
	binomial_heap<int> heap = binomial_heap<int>();
	EXPECT_EQ(heap.size(), 0);
	heap.insert(1);
	EXPECT_EQ(heap.size(), 1);
	heap.insert(2);
	heap.insert(6);
	heap.insert(1);
	EXPECT_EQ(heap.size(), 4);
	heap.extract_min();
	heap.extract_min();
	EXPECT_EQ(heap.size(), 2);
}

TEST(BinomialHeapBasics, Delete) {
	binomial_heap<int> heap = binomial_heap<int>();
	auto pointer_to_1 = heap.insert(1);
	heap.erase(pointer_to_1);
	EXPECT_EQ(pointer_to_1->is_mark(), false);
	EXPECT_EQ(heap.size(), 0);

	heap.insert(3);
	auto pointer_to_0 = heap.insert(0);
	heap.insert(1);
	heap.erase(pointer_to_0);
	EXPECT_EQ(pointer_to_0->is_mark(), false);
	EXPECT_EQ(heap.get_min(), 1);
}

TEST(BinomialHeapAdvanced, Change) {
	binomial_heap<int> heap = binomial_heap<int>();
	heap.insert(1);
	heap.insert(3);
	auto pointer_to_2 = heap.insert(2);
	heap.insert(1);

	heap.change(pointer_to_2, 0);
	EXPECT_EQ(pointer_to_2->value(), 0);
	EXPECT_EQ(heap.get_min(), 0);

	heap.change(pointer_to_2, 100);
	EXPECT_EQ(pointer_to_2->value(), 100);
	EXPECT_EQ(heap.get_min(), 1);
}

TEST(BinomialHeapAdvanced, MergeEmptyToNonEmpty) {
	binomial_heap<int> heap1 = binomial_heap<int>();
	binomial_heap<int> heap2 = binomial_heap<int>();
	heap1.insert(1);
	heap1.insert(3);

	heap1.merge(heap2);
	EXPECT_EQ(heap1.get_min(), 1);
	EXPECT_EQ(heap1.size(), 2);
	EXPECT_EQ(heap2.size(), 0);
}

TEST(BinomialHeapAdvanced, MergeNonEmptyToEmpty) {
	binomial_heap<int> heap1 = binomial_heap<int>();
	binomial_heap<int> heap2 = binomial_heap<int>();
	heap1.insert(1);
	heap1.insert(3);

	heap2.merge(heap1);
	EXPECT_EQ(heap2.get_min(), 1);
	EXPECT_EQ(heap2.size(), 2);
	EXPECT_EQ(heap1.size(), 0);
}

TEST(BinomialHeapAdvanced, MergeNonEmptyToNonEmpty) {
	binomial_heap<int> heap1 = binomial_heap<int>();
	binomial_heap<int> heap2 = binomial_heap<int>();
	heap1.insert(1);
	heap1.insert(3);

	heap2.insert(0);
	heap2.insert(4);

	heap1.merge(heap2);
	EXPECT_EQ(heap1.get_min(), 0);
	EXPECT_EQ(heap1.size(), 4);
	EXPECT_EQ(heap2.size(), 0);
}

TEST(BinomialHeapExceptions, RequestsToEmptyHeap) {
	binomial_heap<int> heap = binomial_heap<int>();

	EXPECT_THROW(heap.get_min(), std::runtime_error);
	EXPECT_THROW(heap.extract_min(), std::runtime_error);
}

TEST(BinomialHeapExceptions, RequestsToInvalidatedPointer) {
	binomial_heap<int> heap = binomial_heap<int>();
	auto pointer_to_1 = heap.insert(1);
	heap.erase(pointer_to_1);

	EXPECT_THROW(heap.erase(pointer_to_1), std::runtime_error);
	EXPECT_THROW(heap.change(pointer_to_1, 10), std::runtime_error);
}

TEST(BinomialHeapExceptions, Stress) {
	binomial_heap<int> heap = binomial_heap<int>();
	int size = 0;
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 5000; ++j) {
			auto a = heap.insert(j);
			size++;
			EXPECT_EQ(heap.size(), size);
			heap.change(a, 2500);
			EXPECT_EQ(a->value(), 2500);
			heap.change(a, j);
			EXPECT_EQ(a->value(), j);
			heap.change(a, rand());
		}
		for (int k = 0; k < 2000; ++k) {
			heap.extract_min();

			size--;
			EXPECT_EQ(heap.size(), size);
		}
	}
}