// Copyright 2020 Ekaterina Burmistrova
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <math.h>
#include <algorithm>
#include <vector>

#include "./shell_sort.h"

TEST(Parallel_sort, sort_random_vect) {
    int mynode;
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);

    std::vector<int> vect = { 7, 11, -45, 0, 23, 44, -100 };
    std::vector<int> res = { -100, -45, 0, 7, 11, 23, 44 };
	double MPISortedS = MPI_Wtime();
    std::vector<int> sortVector = Parallel_sort(vect);
	double MPISortedE = MPI_Wtime();
    if (mynode == 0) {
		std::cout << std::fixed << std::setprecision(8) << "MPI_Sort_Time :    "
			<< MPISortedE - MPISortedS << std::endl;
        ASSERT_EQ(sortVector, res);
    }
}

TEST(Parallel_sort, compare_time_sort) {
    int mynode;
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);

    std::vector<int> vect = gen_input(5);
	
	double MPISortedS = MPI_Wtime();
    std::vector<int> sortVector = Parallel_sort(vect);
	double MPISortedE = MPI_Wtime();
    if (mynode == 0) {
		double SortedS = MPI_Wtime();
		std::vector<int> s_vect = Sequential_Shell(vect);
		double SortedE = MPI_Wtime();
		std::cout << std::fixed << std::setprecision(8) << "MPI_Sort_Time :    "
			<< MPISortedE - MPISortedS << std::endl;
		std::cout << std::fixed << std::setprecision(8) << "Mine :    "
			<< SortedE - SortedS << std::endl;
        ASSERT_EQ(sortVector, s_vect);
    }
}

TEST(Parallel_sort, compare_time_seq_shell_w_seq_ins) {
    int mynode;
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);

    std::vector<int> vect = { 10, 6, 17, -2, -2 };
    std::vector<int> res = { -2, -2, 6, 10, 17 };

    if (mynode == 0) {
		double ShellS = MPI_Wtime();
		std::vector<int> sortVect_shell = Sequential_Shell(vect);
		double ShellE = MPI_Wtime();
		double InsS = MPI_Wtime();
		std::vector<int> sortVect_ins = Sequential_sort(vect);
		double InsE = MPI_Wtime();
		std::cout << std::fixed << std::setprecision(8) << "Shell_Sort_Time :    "
			<< ShellE - ShellS << std::endl;
		std::cout << std::fixed << std::setprecision(8) << "Insertions_Sort_Time :    "
			<< InsE - InsS << std::endl;
        ASSERT_EQ(sortVect_shell, sortVect_ins);
    }
}

TEST(Parallel_sort, sort_one_elem) {
    int mynode;
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);

    std::vector<int> vect = { -3 };
    std::vector<int> res = { -3 };

    std::vector<int> sortArray = Parallel_sort(vect);

    if (mynode == 0) {
        ASSERT_EQ(sortArray, res);
    }
}

TEST(Parallel_sort, return_empty_vect) {
    int mynode;
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);

    std::vector<int> vect = { };
    std::vector<int> res = { };

    std::vector<int> sortArray = Parallel_sort(vect);

    if (mynode == 0) {
        ASSERT_EQ(sortArray, res);
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);

    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners =
    ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);

    return RUN_ALL_TESTS();
}
