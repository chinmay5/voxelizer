#pragma once

#include <string>
#include <iostream>
#include <fstream>

// Eigen
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

size_t get_file_length(const std::string base_filename);
void read_binary(void* data, const size_t length, const std::string base_filename);
void write_binary(void* data, const size_t bytes, const std::string base_filename);
void write_binvox(const unsigned int* vtable, const size_t gridsize, const std::string base_filename);
void write_off(const unsigned int *vtable, const unsigned int *colortable, const size_t gridsize,
               const std::string base_filename, voxinfo voxinfo);

//h5 file
bool combine_data(const unsigned int *vtable, const unsigned int *colortable, const size_t gridsize,
               voxinfo voxinfo, std::string output);
