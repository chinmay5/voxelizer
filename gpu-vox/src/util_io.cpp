#include "util.h"
#include "util_io.h"
#include <H5Cpp.h>


bool write_transformations(const voxinfo &voxinfo, const std::string &output);

using namespace std;

size_t get_file_length(const std::string base_filename) {
    // open file at the end
    std::ifstream input(base_filename.c_str(), ios_base::ate | ios_base::binary);
    assert(input);
    size_t length = input.tellg();
    input.close();
    return length; // get file length
}

void read_binary(void *data, const size_t length, const std::string base_filename) {
    // open file
    std::ifstream input(base_filename.c_str(), ios_base::in | ios_base::binary);
    assert(input);
#ifndef SILENT
    fprintf(stdout, "[I/O] Reading %llu kb of binary data from file %s \n", size_t(length / 1024.0f),
            base_filename.c_str());
    fflush(stdout);
#endif
    input.seekg(0, input.beg);
    input.read((char *) data, 8);
    input.close();
    return;
}

void write_off(const unsigned int *vtable, const unsigned int *colortable, const size_t gridsize,
               const std::string base_filename, voxinfo voxinfo) {
    string filename_output = base_filename + string("_") + to_string(gridsize) + string(".off");
#ifndef SILENT
    fprintf(stdout, "[I/O] Writing data in obj format to %s \n", filename_output.c_str());
#endif
    ofstream output(filename_output.c_str(), ios::out);
    size_t voxels_written = 0;
    uint n = 0;
    std::vector<string> faces;
    std::vector<string> verts;
    long double secondScaler = 1 / voxinfo.scales.x; //Since scales remain the same for all three indices
    double scale = gridsize;
    long double t_x = voxinfo.translation.x;
    long double t_y = voxinfo.translation.y;
    long double t_z = voxinfo.translation.z;
    assert(output);
    for (size_t x = 0; x < gridsize; x++) {
        for (size_t y = 0; y < gridsize; y++) {
            for (size_t z = 0; z < gridsize; z++) {
                if (checkVoxel(x, y, z, gridsize, vtable)) {
                    voxels_written += 8;
                    n = verts.size();

//                    verts.push_back(  std::to_string(x) + " "+  std::to_string(y)+ " "+ std::to_string(z)) ;
//                    verts.push_back(  std::to_string(x)+" "+ std::to_string(y)+" "+ std::to_string(z + 1))  ;
//                    verts.push_back(  std::to_string(x)+" "+ std::to_string(y + 1) +" "+ std::to_string(z));
//                    verts.push_back(  std::to_string(x)+" "+ std::to_string(y + 1)+" "+ std::to_string(z + 1));
//                    verts.push_back(  std::to_string(x + 1)+" "+ std::to_string(y)+" "+ std::to_string(z) );
//                    verts.push_back(  std::to_string(x + 1)+" "+ std::to_string(y)+" "+ std::to_string(z + 1));
//                    verts.push_back(  std::to_string(x + 1)+" "+ std::to_string(y + 1)+" "+ std::to_string(z) );
//                    verts.push_back(  std::to_string(x + 1)+" "+ std::to_string(y + 1)+" "+ std::to_string(z + 1));
                    size_t location = x + (y * gridsize) + (z * gridsize * gridsize);
                    size_t int_location = location * size_t(4);
//                    handle labels here since they are 100 but should be -100
                    int label = colortable[int_location + 3] == 100 ? -100 : colortable[int_location + 3];
                    string color = std::to_string(colortable[int_location]) + " " +
                                   std::to_string(colortable[int_location + 1]) + " " +
                                   std::to_string(colortable[int_location + 2]) + " " +
                                   std::to_string(label);

                    verts.push_back(std::to_string((x / scale - 0.5) * secondScaler - t_x) + " " +
                                    std::to_string((y / scale - 0.5) * secondScaler - t_y) + " " +
                                    std::to_string((z / scale - 0.5) * secondScaler - t_z) + " " +
                                    color); // +0.5 to put vertex in the middle of the voxel
                    verts.push_back(std::to_string((x / scale - 0.5) * secondScaler - t_x) + " " +
                                    std::to_string((y / scale - 0.5) * secondScaler - t_y) + " " +
                                    std::to_string(((z + 1) / scale - 0.5) * secondScaler - t_z) + " " +
                                    color); // +0.5 to put vertex in the middle of the voxel
                    verts.push_back(std::to_string((x / scale - 0.5) * secondScaler - t_x) + " " +
                                    std::to_string(((y + 1) / scale - 0.5) * secondScaler - t_y) + " " +
                                    std::to_string((z / scale - 0.5) * secondScaler - t_z) + " " +
                                    color); // +0.5 to put vertex in the middle of the voxel
                    verts.push_back(std::to_string((x / scale - 0.5) * secondScaler - t_x) + " " +
                                    std::to_string(((y + 1) / scale - 0.5) * secondScaler - t_y) + " " +
                                    std::to_string(((z + 1) / scale - 0.5) * secondScaler - t_z) + " " +
                                    color); // +0.5 to put vertex in the middle of the voxel
                    verts.push_back(std::to_string(((x + 1) / scale - 0.5) * secondScaler - t_x) + " " +
                                    std::to_string((y / scale - 0.5) * secondScaler - t_y) + " " +
                                    std::to_string((z / scale - 0.5) * secondScaler - t_z) + " " +
                                    color); // +0.5 to put vertex in the middle of the voxel
                    verts.push_back(std::to_string(((x + 1) / scale - 0.5) * secondScaler - t_x) + " " +
                                    std::to_string((y / scale - 0.5) * secondScaler - t_y) + " " +
                                    std::to_string(((z + 1) / scale - 0.5) * secondScaler - t_z) + " " +
                                    color); // +0.5 to put vertex in the middle of the voxel
                    verts.push_back(std::to_string(((x + 1) / scale - 0.5) * secondScaler - t_x) + " " +
                                    std::to_string(((y + 1) / scale - 0.5) * secondScaler - t_y) + " " +
                                    std::to_string((z / scale - 0.5) * secondScaler - t_z) + " " +
                                    color); // +0.5 to put vertex in the middle of the voxel
                    verts.push_back(std::to_string(((x + 1) / scale - 0.5) * secondScaler - t_x) + " " +
                                    std::to_string(((y + 1) / scale - 0.5) * secondScaler - t_y) + " " +
                                    std::to_string(((z + 1) / scale - 0.5) * secondScaler - t_z) + " " +
                                    color); // +0.5 to put vertex in the middle of the voxel

//                    [0, 1, 2], [2, 3, 0], [1, 5, 6], [6, 2, 1], [7, 6, 5], [5, 4, 7],
//                    [4, 0, 3], [3, 7, 4], [4, 5, 1], [1, 0, 4], [3, 2, 6],
//                    [6, 7, 3]
//                    Left
                    faces.push_back(
                            "3 " + std::to_string(n + 0) + " " + std::to_string(n + 1) + " " + std::to_string(n + 2));
                    faces.push_back(
                            "3 " + std::to_string(n + 2) + " " + std::to_string(n + 1) + " " + std::to_string(n + 3));
//                    Bottom
                    faces.push_back(
                            "3 " + std::to_string(n + 0) + " " + std::to_string(n + 6) + " " + std::to_string(n + 4));
                    faces.push_back(
                            "3 " + std::to_string(n + 6) + " " + std::to_string(n + 0) + " " + std::to_string(n + 2));
//                    Right
                    faces.push_back(
                            "3 " + std::to_string(n + 4) + " " + std::to_string(n + 6) + " " + std::to_string(n + 5));
                    faces.push_back(
                            "3 " + std::to_string(n + 5) + " " + std::to_string(n + 6) + " " + std::to_string(n + 7));
//                    Top
                    faces.push_back(
                            "3 " + std::to_string(n + 5) + " " + std::to_string(n + 7) + " " + std::to_string(n + 1));
                    faces.push_back(
                            "3 " + std::to_string(n + 1) + " " + std::to_string(n + 7) + " " + std::to_string(n + 3));
//                    Front
                    faces.push_back(
                            "3 " + std::to_string(n + 0) + " " + std::to_string(n + 4) + " " + std::to_string(n + 5));
                    faces.push_back(
                            "3 " + std::to_string(n + 5) + " " + std::to_string(n + 1) + " " + std::to_string(n + 0));
//                    Back
                    faces.push_back(
                            "3 " + std::to_string(n + 7) + " " + std::to_string(n + 6) + " " + std::to_string(n + 2));
                    faces.push_back(
                            "3 " + std::to_string(n + 7) + " " + std::to_string(n + 2) + " " + std::to_string(n + 3));

                }
                //else {
                //	output << "NOVOXEL " << x << " " << y << " " << z << endl;
                //}
            }
        }
    }
    // Finally, write all the faces at the end
    std::ostream_iterator<std::string> output_iterator(output, "\n");
    output << "COFF \n";
    output << verts.size() << " " << faces.size() << " 0 \n";
    std::copy(verts.begin(), verts.end(), output_iterator);
    std::copy(faces.begin(), faces.end(), output_iterator);
    std::cout << "written " << voxels_written << std::endl;
    output.close();
}

void write_binary(void *data, size_t bytes, const std::string base_filename) {
    string filename_output = base_filename + string(".bin");
#ifndef SILENT
    fprintf(stdout, "[I/O] Writing data in binary format to %s (%s) \n", filename_output.c_str(),
            readableSize(bytes).c_str());
#endif
    ofstream output(filename_output.c_str(), ios_base::out | ios_base::binary);
    output.write((char *) data, bytes);
    output.close();
}

void write_binvox(const unsigned int *vtable, const size_t gridsize, const std::string base_filename) {
    // Open file
    string filename_output = base_filename + string("_") + to_string(gridsize) + string(".binvox");
#ifndef SILENT
    fprintf(stdout, "[I/O] Writing data in binvox format to %s \n", filename_output.c_str());
#endif
    ofstream output(filename_output.c_str(), ios::out | ios::binary);
    assert(output);

    // Write ASCII header
    output << "#binvox 1" << endl;
    output << "dim " << gridsize << " " << gridsize << " " << gridsize << "" << endl;
    output << "data" << endl;

    // Write BINARY Data (and compress it a bit using run-length encoding)
    char currentvalue, current_seen;
    for (size_t x = 0; x < gridsize; x++) {
        for (size_t z = 0; z < gridsize; z++) {
            for (size_t y = 0; y < gridsize; y++) {
                if (x == 0 && y == 0 && z == 0) { // special case: first voxel
                    currentvalue = checkVoxel(0, 0, 0, gridsize, vtable);
                    output.write((char *) &currentvalue, 1);
                    current_seen = 1;
                    continue;
                }
                char nextvalue = checkVoxel(x, y, z, gridsize, vtable);
                if (nextvalue != currentvalue || current_seen == (char) 255) {
                    output.write((char *) &current_seen, 1);
                    current_seen = 1;
                    currentvalue = nextvalue;
                    output.write((char *) &currentvalue, 1);
                } else {
                    current_seen++;
                }
            }
        }
    }

    // Write rest
    output.write((char *) &current_seen, 1);
    output.close();
}

template<int RANK>
bool write_int_hdf5(const std::string filepath, Eigen::Tensor<int, RANK, Eigen::RowMajor> &tensor) {

    try {

        /*
         * Turn off the auto-printing when failure occurs so that we can
         * handle the errors appropriately
         */
        H5::Exception::dontPrint();

        /*
         * Create a new file using H5F_ACC_TRUNC access,
         * default file creation properties, and default file
         * access properties.
         */
        H5::H5File file(filepath, H5F_ACC_TRUNC);

        /*
         * Define the size of the array and create the data space for fixed
         * size dataset.
         */
        hsize_t rank = RANK;
        hsize_t dimsf[rank];
        for (int i = 0; i < rank; i++) {
            dimsf[i] = tensor.dimension(i);

        }

        H5::DataSpace dataspace(rank, dimsf);

        /*
         * Define datatype for the data in the file.
         * We will store little endian INT numbers.
         */
        H5::IntType datatype(H5::PredType::NATIVE_INT);
        datatype.setOrder(H5T_ORDER_LE);

        /*
         * Create a new dataset within the file using defined dataspace and
         * datatype and default dataset creation properties.
         */
        H5::DataSet dataset = file.createDataSet("tensor", datatype, dataspace);

        /*
         * Write the data to the dataset using default memory space, file
         * space, and transfer properties.
         */
        int *data = static_cast<int *>(tensor.data());
        dataset.write(data, H5::PredType::NATIVE_INT);
    }  // end of try block

        // catch failure caused by the H5File operations
    catch (H5::FileIException error) {
        error.printError();
        return false;
    }

        // catch failure caused by the DataSet operations
    catch (H5::DataSetIException error) {
        error.printError();
        return false;
    }

        // catch failure caused by the DataSpace operations
    catch (H5::DataSpaceIException error) {
        error.printError();
        return false;
    }

        // catch failure caused by the DataSpace operations
    catch (H5::DataTypeIException error) {
        error.printError();
        return false;
    }

    return true;
}


bool combine_data(const unsigned int *vtable, const unsigned int *colortable, const size_t gridsize,
                  voxinfo voxinfo, const string output) {
    Eigen::Tensor<int, 4, Eigen::RowMajor> occ(gridsize, gridsize, gridsize, 4);
    occ.setZero();
//    Last column is set to -100 since that is how we have generated the labels for ourselves
//    occ(Eigen::all, Eigen::last)= -100;
//    occ:all
    for (size_t x = 0; x < gridsize; x++) {
        for (size_t y = 0; y < gridsize; y++) {
            for (size_t z = 0; z < gridsize; z++) {
//                This is the data that we have. We simply need to write this into an Eigen Tensor.
//                In case we have no intersection at this location, that means it
//                is empty space.
                if (checkVoxel(x, y, z, gridsize, vtable)) {
                    size_t location = x + (y * gridsize) + (z * gridsize * gridsize);
                    size_t int_location = location * size_t(4);
                    occ(x, y, z, 0) = colortable[int_location];
                    occ(x, y, z, 1) = colortable[int_location + 1];
                    occ(x, y, z, 2) = colortable[int_location + 2];
//                Again labels need to be specifically checked since we stored them as 100 as they are unsigned here
                    int label = colortable[int_location + 3] == 100 ? -100 : colortable[int_location + 3];

                    occ(x, y, z, 3) = label;
                } else {
//                    Once it is empty space, that also means that we should put a label of -100 there since this is a don't care
//                location for us
                    occ(x, y, z, 3) = -100;
                }

            }
        }
    }
    bool success = write_int_hdf5<4>(output, occ);
    return success && write_transformations(voxinfo, output);
}

bool write_transformations(const voxinfo &voxinfo, const std::string &output) {
//    Writing json data format using a simple text writer as I wanted to avoid extra file dependencies
    ofstream myfile;
    string vox_info_file = output + ".json";
    myfile.open(vox_info_file);
    long double scaling_factor = 1 / voxinfo.scales.x; //Since scales remain the same for all three indices
    myfile << "{";
    myfile << "\"scales\": [" + to_string(scaling_factor) + ", " + to_string(scaling_factor) + ", " +
              to_string(scaling_factor) + "]";
    myfile << ", ";
    myfile << "\"translation\": [" + to_string(voxinfo.translation.x) + ", " + to_string(voxinfo.translation.y) + ", " +
              to_string(voxinfo.translation.z) + "]";
    myfile << "}";
    myfile.close();
}

