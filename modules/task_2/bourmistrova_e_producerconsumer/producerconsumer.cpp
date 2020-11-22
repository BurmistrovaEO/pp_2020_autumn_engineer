// Copyright 2020 Ekaterina Burmistrova
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "../../../modules/task_2/bourmistrova_e_producerconsumer/producerconsumer.h"

std::vector<double> gen_input(int sz) {
    std::mt19937 gener;
    gener.seed(static_cast<unsigned int>(time(0)));
    std::vector<double> vect(sz);
    for (int i = 0; i < sz; i++) {
        vect[i] = static_cast<double>(gener() % 1000);
    }
    return vect;
}
std::vector<double> calc_exp_pow_num(std::vector<double> vec) {
    std::vector<double> exps(vec.size());
    for (int i = 0; i < vec.size(); i++) {
        exps[i] = exp(vec[i]);
    }
    return exps;
}
/*double produce()
{
	std::mt19937 gener;
	gener.seed(static_cast<unsigned int>(time(0)));
	return (double)(gener() % 1000);
}

void consume()
{

}*/
std::vector<double> Parallel_method(std::vector<double> vect, int n_elem) {
    int mynode;
    int totnodes;
    std::vector<double> expons;
    // int message_buffer_size = n_elem * sizeof(double) + MPI_BSEND_OVERHEAD;
    // double* message_buffer = (double*)malloc(message_buffer_size);
    // MPI_Buffer_attach(message_buffer, message_buffer_size);
    // int par_sum, inter;
    // int blocked = 1; // consumer_status
    // int free = 1; // producer status
    // int delta = n_elem %
    MPI_Comm_size(MPI_COMM_WORLD, &totnodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);
    double buf = 0;
    if (mynode == 0) {  // producer
        if (totnodes == 1) {
            expons = calc_exp_pow_num(vect);
            return expons;
        }
        for (int i = 0; i < n_elem; ++i) {
            MPI_Status status;
            MPI_Recv(&buf, 1, MPI_DOUBLE,
                MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG > 0) {
                expons[status.MPI_TAG - 1] = buf;
            }
            MPI_Send(&vect[i - 1], 1, MPI_DOUBLE, status.MPI_SOURCE,
                i, MPI_COMM_WORLD);
        }
        int k = 0;
        for (int j = 0; j < totnodes - 1; j++) {
            MPI_Status status;
            // double buf = 0;
            MPI_Recv(&buf, 1, MPI_DOUBLE,
                MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG > 0) {
                expons[status.MPI_TAG - 1] = buf;
            }
            MPI_Send(&k, 1, MPI_INT, status.MPI_SOURCE,
                0, MPI_COMM_WORLD);
        }
    } else {  // consumer
        MPI_Status status;
        buf = 0;
        //for (int j = 0; j < n_elem; j++) {
        MPI_Send(&buf, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        int killed = 0;
        do {
            double n = 0;
            MPI_Recv(&n, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == 0) {
                killed++;
            } else {
                buf = exp(n);
                MPI_Send(&buf, 1, MPI_DOUBLE, 0, status.MPI_TAG, MPI_COMM_WORLD);
            }
        } while (killed != n_elem);
    }
    // MPI_Buffer_detach(message_buffer, &message_buffer_size);
    // free(message_buffer);
    MPI_Barrier(MPI_COMM_WORLD);
    return expons;
}
