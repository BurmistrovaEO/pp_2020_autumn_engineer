// Copyright 2020 Ekaterina Burmistrova
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <math.h>
#include <algorithm>
#include <vector>

#include "./producerconsumer.h"

TEST(Producer_Consumer_MPI, Test_Eq) {
    int mynode;
	int totnodes;
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);
	MPI_Comm_size(MPI_COMM_WORLD, &totnodes);
    int num = 10000;
    std::vector<double> matrix(num);
    std::vector<double> ref_matrix(num);
    if (mynode == totnodes-1) {
        matrix = gen_input(num);
    }

    std::vector<double> par_sum = Parallel_method(matrix, num);
	std::sort(par_sum.begin(), par_sum.end());

    if (mynode == totnodes - 1) {
        ref_matrix = calc_exp_pow_num(matrix);
		std::sort(ref_matrix.begin(),ref_matrix.end());
        ASSERT_EQ(ref_matrix, par_sum);
    }
}
TEST(Producer_Consumer_MPI, Test_Eq_Small_Num) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<double> nums = { 1.0, 2.0, 4.0, 6.0 };
    std::vector<double> exps = calc_exp_pow_num(nums);
    std::vector<double> exp_exps;

    exp_exps = Parallel_method(nums, 4);

    bool flag = true;

    for (int i = 0; i < 4; i++) {
        if (exps[i] != exp_exps[i]) {
            flag = false;
            break;
        }
    }
    if (rank == 0) {
        EXPECT_TRUE(flag);
    }
}
TEST(Producer_Consumer_MPI, Test_Exp_Creation) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<double> vector = { 0.0, 1.0, 2.0 };
    std::vector<double> vector_exps = calc_exp_pow_num(vector);
    bool flag = true;
    for (int i = 0; i < 3; i++) {
        if (vector_exps[i] != exp(vector[i])) {
            flag = false;
            break;
        }
    }
    if (rank == 0) {
        EXPECT_TRUE(flag);
    }
}

TEST(Producer_Consumer_MPI, Test_Vect_Creation_NN) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int max_size = 50;
    std::vector<double> vector = gen_input(max_size);
    bool flag = true;
    if (vector.empty() == true) {
        flag = false;
    }
    if (rank == 0) {
        EXPECT_TRUE(flag);
    }
}
TEST(Producer_Consumer_MPI, Test_Exp_Creation_NN) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int max_size = 20;
    std::vector<double> vector = gen_input(max_size);
    std::vector<double> vector_exps = calc_exp_pow_num(vector);

    bool flag = true;
    if (vector_exps.empty() == true) {
        flag = false;
    }
    if (rank == 0) {
        EXPECT_TRUE(flag);
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
