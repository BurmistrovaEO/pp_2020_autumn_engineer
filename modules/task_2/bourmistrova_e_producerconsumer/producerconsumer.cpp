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
	//int *count_compl;
    std::vector<double> expons(n_elem);
	//std::vector<double>::const_iterator iter = vect.begin();
    MPI_Comm_size(MPI_COMM_WORLD, &totnodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);
	int delta;
	int rest = 0;
	if (totnodes > 1) {
		delta = n_elem / (totnodes - 1);		
		if (n_elem % (totnodes - 1) != 0)
			rest = n_elem % (totnodes - 1);
	}
    double *buf;
    MPI_Request *requests;
	//int k = 0;
	int tmp = 0;
    MPI_Status status;

    if (totnodes == 1) {
        if (mynode == 0)
            expons = calc_exp_pow_num(vect);
        return expons;
    } else {
		if (mynode != totnodes - 1) { // producer
			if (mynode < totnodes - 2) {
				buf = new double[sizeof(double) * delta];
				//std::cout << "The current process is = " << mynode;
				for (int i = 0; i < delta; i++) {
					buf[i] = exp(vect[tmp]);
					tmp++;
				}
				MPI_Send(&buf[0], delta, MPI_DOUBLE, totnodes - 1, mynode, MPI_COMM_WORLD);
			} else {
				buf = new double[sizeof(double) * (delta+rest)];
				for (int i = 0; i < delta+rest; i++) {
					buf[i] = exp(vect[tmp]);
					tmp++;
				}
				MPI_Send(&buf[0], delta+rest, MPI_DOUBLE, totnodes - 1, mynode, MPI_COMM_WORLD);
			}
        } else {  // consumer	
			//if (count_compl == n_elem)
			//{
			//	MPI_Barrier(MPI_COMM_WORLD);
			//	return expons;
			//}
			//std::cout << "The current process is = " << mynode;
		    buf = new double[sizeof(double) * n_elem];
			requests = new MPI_Request[sizeof(MPI_Request)*(totnodes - 1)];
			for (int i = 0; i < totnodes - 1; i++)
			{
				requests[i] = MPI_REQUEST_NULL;
				//while (1) {  /* main loop*/
					//for (int j = 0; j < n_elem; j++) {
						MPI_Waitany(totnodes - 1, requests, &i, &status);
						if (requests[i] == MPI_UNDEFINED) {
							for (int j = 0; j < totnodes - 1; j++)
								MPI_Irecv(&buf[i], sizeof(double) * n_elem, MPI_DOUBLE, j, status.MPI_TAG, MPI_COMM_WORLD, &requests[j]);
						}
						else {
							//MPI_Get_count(&status, MPI_DOUBLE, &count_compl);
							//count_compl += 1;
							expons[i] = buf[i];
						}
					//}
				//}
			}
		}
    }
    // MPI_Buffer_detach(message_buffer, &message_buffer_size);
    // free(message_buffer);
    MPI_Barrier(MPI_COMM_WORLD);
    return expons;
}
