// Copyright 2020 Ekaterina Burmistrova
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <utility>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include "../../../modules/task_3/bourmistrova_e_shell_sort/shell_sort.h"

std::vector<int> gen_input(int sz) {
    std::mt19937 gener;
    gener.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> vect(sz);
    for (int i = 0; i < sz; i++) {
         vect[i] = gener() % 1000;
    }
    return vect;
}
std::vector<int> Sequential_Shell(std::vector<int> vec) {
    int length = vec.size();
    int h = 1;
    while (h < length / 3) {
       h = 3 * h + 1;
    }
    while (h > 0) {
        for (int i = h; i < length; i++) {
            for (int j = i; j > 0 && vec[j] < vec[j - h]; j -= h) {
                std::swap(vec[j], vec[j - h]);
            }
        }
        h -= h / 3;  // decreasing h
    }
    return vec;
}



std::vector<int> Sequential_sort(std::vector<int> vect) {
    int size = vect.size();
    int k;
    for (int i = 1; i < size; i++) {
        k = 1;
        while ((i - k >= 0) && (vect[i - k + 1] < vect[i - k])) {
            int n = vect[i - k + 1];
            vect[i - k + 1] = vect[i - k];
            vect[i - k] = n;
            k++;
        }
    }
    return vect;
}

std::vector<int> Parallel_sort(std::vector<int> vect) {
    int mynode;
    int totnodes;
    std::vector<int> local_vect;  //  local_vector

    int vect_size = vect.size();
    int tmp_size_count = vect.size();  //  is used for count preparation and also repres number of go-throgths thr inp vect

    MPI_Comm_size(MPI_COMM_WORLD, &totnodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);
    if (vect_size <= 1) {
        return vect;
    }
    if (totnodes == 1) {
        return Sequential_Shell(vect);
    }
    int lvect_size;  // local vector size
    int count;  //  number of used processes for do{}while loop
    int tag = 0;  //  tag for send/recv operations
    int counter = 0;
    int countProc;

    while (tmp_size_count > 0) {
        tmp_size_count = tmp_size_count / 2;
        countProc = tmp_size_count % totnodes;
        if (tmp_size_count < totnodes) {
            count = tmp_size_count;
        } else {
            count = totnodes;
        }
        tag = 0;
        counter = 0;
        lvect_size = vect_size / tmp_size_count;
        do {
            if (counter + countProc == tmp_size_count)
                count = countProc;
            for (int proc = 0; proc < count; proc++) {
                if (mynode == 0) {
                    local_vect.clear();
                    for (int i = 0; i < lvect_size; i++) {
                        local_vect.push_back(vect[proc + counter + tmp_size_count * i]);
                    }
                    if (proc == 0) {
                        local_vect = Sequential_sort(local_vect);
                        for (int i = 0; i < lvect_size; i++) {
                            vect[counter + tmp_size_count * i] = local_vect[i];
                        }
                    } else {
                        MPI_Send(&local_vect[0], lvect_size, MPI_INT, proc, tag, MPI_COMM_WORLD);
                    }
                } else {
                    if (mynode == proc) {
                        MPI_Status status;
                        local_vect.resize(lvect_size);
                        MPI_Recv(&local_vect[0], lvect_size, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
                        local_vect = Sequential_sort(local_vect);
                        MPI_Send(&local_vect[0], lvect_size, MPI_INT, 0, proc + tag, MPI_COMM_WORLD);
                    }
                }
            }
            counter += totnodes;
            tag++;
        } while (counter < tmp_size_count);

        if (tmp_size_count < totnodes) {
            count = tmp_size_count;
        } else {
            count = totnodes;
        }
        counter = 0;
        tag = 0;

        if (mynode == 0) {
            do {
                if (counter + countProc == tmp_size_count)
                    count = countProc;
                for (int proc = 1; proc < count; proc++) {
                    MPI_Status status;
                    local_vect.resize(lvect_size);
                    MPI_Recv(&local_vect[0], lvect_size, MPI_INT, proc,
                        proc + tag, MPI_COMM_WORLD, &status);
                    for (int i = 0; i < lvect_size; i++) {
                        vect[proc + tmp_size_count * i + counter] = local_vect[i];
                    }
                }
                counter = counter + totnodes;
                tag++;
            } while (counter < tmp_size_count);
        }
        if (lvect_size == vect_size)
            tmp_size_count = 0;
    }
    return vect;
}
