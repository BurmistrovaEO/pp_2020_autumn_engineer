// Copyright 2020 Ekaterina Burmistrova
#ifndef MODULES_TASK_2_BOURMISTROVA_E_PRODUCERCONSUMER_PRODUCERCONSUMER_H_
#define MODULES_TASK_2_BOURMISTROVA_E_PRODUCERCONSUMER_PRODUCERCONSUMER_H_

#include <vector>

std::vector<double> gen_input(int  sz);
std::vector<double> calc_exp_pow_num(std::vector<double> vec);
std::vector<double> Parallel_method(std::vector<double> vect,
    int n_elem);

#endif  // MODULES_TASK_2_BOURMISTROVA_E_PRODUCERCONSUMER_PRODUCERCONSUMER_H_
