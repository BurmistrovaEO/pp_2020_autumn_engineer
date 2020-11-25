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

std::vector<double> Parallel_method(std::vector<double> vect, int n_elem) {
    int mynode;
    int totnodes;
    std::vector<double> expons(n_elem);
    MPI_Comm_size(MPI_COMM_WORLD, &totnodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);
    double *buf;
    MPI_Request *requests;
    MPI_Status status;
    if (totnodes == 1) {
        if (mynode == 0)
            expons = calc_exp_pow_num(vect);
        return expons;
    } else {
        if (mynode == totnodes - 1) {  // consumer
            buf = new double(sizeof(double)*(totnodes - 1));
            requests = new MPI_Request(sizeof(MPI_Request)*(totnodes - 1));
            for (int i = 0; i < totnodes - 1; i++) {  /* main loop*/
                requests[i] = MPI_REQUEST_NULL;
                for (int j = 0; j < n_elem; j++) {
                    MPI_Waitany(totnodes - 1, requests, &i, &status);
                    if (i == MPI_UNDEFINED) {
                        for (int k = 0; k < totnodes - 1; k++)
                            MPI_Irecv(buf, 1, MPI_DOUBLE, j, status.MPI_TAG, MPI_COMM_WORLD, &requests[j]);
                    } else {
                        MPI_Get_count(&status, MPI_DOUBLE, &n_elem);
                        expons[i] = buf[i];
                    }
                }
            }
        } else {  // producer
            buf = new double(sizeof(double));
            std::cout << "The current process is = " << mynode;
            for (int i = 0; i < n_elem; i++) {
                buf[i] = exp(vect[i]);
                MPI_Send(&buf[i], 1, MPI_DOUBLE, totnodes - 1, i, MPI_COMM_WORLD);
            }
        }
    }
    // MPI_Buffer_detach(message_buffer, &message_buffer_size);
    // free(message_buffer);
    MPI_Barrier(MPI_COMM_WORLD);
    return expons;
}
