#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#define WINDOWS_LEAN_AND_MEAN // Please, not too much windows shenanigans
#endif

// Standard libs
#include <string>
#include <cstdio>
// GLM for maths
#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <functional>
// Trimesh for model importing
#include "TriMesh.h"

// Util
#include "util.h"
#include "util_io.h"
#include "util_cuda.h"
#include "timer.h"
// CPU voxelizer fallback
#include "cpu_voxelizer.h"

#define TINYPLY_IMPLEMENTATION
#include "tinyply.h"

//Header files for the Boost function
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>


using namespace std;
string version_number = "v0.4.4";

// Forward declaration of CUDA functions
float *meshToGPU_thrust(const trimesh::TriMesh *mesh, vector<ushort> voxinfo); // METHOD 3 to transfer triangles can be found in thrust_operations.cu(h)
void cleanup_thrust();
void voxelize(const voxinfo & v, float* triangle_data, unsigned int* vtable, unsigned int* colortable, bool useThrustPath, bool morton_code);

// Output formats
enum class OutputFormat { output_binvox = 0, output_morton = 1, output_off = 2};
char *OutputFormats[] = { "binvox file", "morton encoded blob", "off file"};

// Default options
string filename = "";
string filename_base = "";
OutputFormat outputformat = OutputFormat::output_binvox;
unsigned int gridsize = 256;
unsigned int gridsize_x = 0;
unsigned int gridsize_y = 0;
unsigned int gridsize_z = 0;
bool useThrustPath = false;
bool forceCPU = false;
float voxel_size = 0.0;

class PlyFile;

void printHeader(){
	fprintf(stdout, "## CUDA VOXELIZER \n");
	cout << "CUDA Voxelizer " << version_number << " by Jeroen Baert" << endl; 
	cout << "github.com/Forceflow/cuda_voxelizer - mail@jeroen-baert.be" << endl;
}

void printExample() {
	cout << "Example: cuda_voxelizer -f /home/jeroen/bunny.ply -s 512" << endl;
}

void printHelp(){
	fprintf(stdout, "\n## HELP  \n");
	cout << "Program options: " << endl;
	cout << " -f <path to model file: .ply, .obj, .3ds> (required)" << endl;
	cout << " -s <voxelization grid size, power of 2: 8 -> 512, 1024, ... (default: 256)>" << endl;
	cout << " -o <output format: binvox, obj or morton (default: binvox)>" << endl;
	cout << " -t : Force using CUDA Thrust Library (possible speedup / throughput improvement)" << endl;
	printExample();
}

// METHOD 1: Helper function to transfer triangles to automatically managed CUDA memory ( > CUDA 7.x)
float* meshToGPU_managed(const trimesh::TriMesh *mesh) {
	Timer t; t.start();
	size_t n_floats = sizeof(float) * 9 * (mesh->faces.size());
	float* device_triangles;
	fprintf(stdout, "[Mesh] Allocating %s of CUDA-managed UNIFIED memory for triangle data \n", (readableSize(n_floats)).c_str());
	checkCudaErrors(cudaMallocManaged((void**) &device_triangles, n_floats)); // managed memory
	fprintf(stdout, "[Mesh] Copy %llu triangles to CUDA-managed UNIFIED memory \n", (size_t)(mesh->faces.size()));
	for (size_t i = 0; i < mesh->faces.size(); i++) {
		glm::vec3 v0 = trimesh_to_glm<trimesh::point>(mesh->vertices[mesh->faces[i][0]]);
		glm::vec3 v1 = trimesh_to_glm<trimesh::point>(mesh->vertices[mesh->faces[i][1]]);
		glm::vec3 v2 = trimesh_to_glm<trimesh::point>(mesh->vertices[mesh->faces[i][2]]);
		size_t j = i * 9;
		memcpy((device_triangles)+j, glm::value_ptr(v0), sizeof(glm::vec3));
		memcpy((device_triangles)+ j+3, glm::value_ptr(v1), sizeof(glm::vec3));
		memcpy((device_triangles)+ j+6, glm::value_ptr(v2), sizeof(glm::vec3));
	}
	t.stop();fprintf(stdout, "[Perf] Mesh transfer time to GPU: %.1f ms \n", t.elapsed_time_milliseconds);

	return device_triangles;
}

// Parse the program parameters and set them as global variables
void parseProgramParameters(int argc, char* argv[]){
	if(argc<2){ // not enough arguments
		fprintf(stdout, "Not enough program parameters. \n \n");
		printHelp();
		exit(0);
	} 
	bool filegiven = false;
	for (int i = 1; i < argc; i++) {
		if (string(argv[i]) == "-f") {
			filename = argv[i + 1];
			filename_base = filename.substr(0, filename.find_last_of("."));
			filegiven = true;
			if (!file_exists(filename)) {
				fprintf(stdout, "[Err] File does not exist / cannot access: %s \n", filename.c_str());
				exit(1);
			}
			i++;
		}
		else if (string(argv[i]) == "-s") {
			gridsize = atoi(argv[i + 1]);
			i++;
//			Use the same value for all grid dimensions
            gridsize_x = gridsize;
            gridsize_y = gridsize;
            gridsize_z = gridsize;
		}
//		Just add another parameter which can give outputs at different scale.
        else if (string(argv[i]) == "-vs") {
//            Using boost library
            vector<string> result;
            string input = argv[i+1];
            boost::split(result, input, boost::is_any_of(","));

            gridsize_x = stoi(result.at(0));
            gridsize_y = stoi(result.at(1));
            gridsize_z = stoi(result.at(2));
            i ++;
        }
        else if (string(argv[i]) == "-voxel_size") {
            voxel_size = atof(argv[i + 1]);
            i++;
        }
        else if (string(argv[i]) == "-h") {
			printHelp();
			exit(0);
		} else if (string(argv[i]) == "-o") {
			string output = (argv[i + 1]);
			transform(output.begin(), output.end(), output.begin(), ::tolower); // to lowercase
			if (output == "binvox"){outputformat = OutputFormat::output_binvox;}
			else if (output == "morton"){outputformat = OutputFormat::output_morton;}
			else if (output == "obj"){outputformat = OutputFormat::output_off;}
			else {
				fprintf(stdout, "[Err] Unrecognized output format: %s, valid options are binvox (default) or morton \n", output.c_str());
				exit(1);
			}
		}
		else if (string(argv[i]) == "-t") {
			useThrustPath = true;
		}
		else if (string(argv[i]) == "-cpu") {
			forceCPU = true;
		}
	}
	if (!filegiven) {
		fprintf(stdout, "[Err] You didn't specify a file using -f (path). This is required. Exiting. \n");
		printExample();
		exit(1);
	}
	fprintf(stdout, "[Info] Filename: %s \n", filename.c_str());
	fprintf(stdout, "[Info] Grid size: %i %i %i\n", gridsize_x, gridsize_y, gridsize_z);
	fprintf(stdout, "[Info] Output format: %s \n", OutputFormats[int(outputformat)]);
	fprintf(stdout, "[Info] Using CUDA Thrust: %s (default: No)\n", useThrustPath ? "Yes" : "No");
}


int main(int argc, char *argv[]) {
	Timer t; t.start();
	printHeader();
	fprintf(stdout, "\n## PROGRAM PARAMETERS \n");
	parseProgramParameters(argc, argv);
	fflush(stdout);
	trimesh::TriMesh::set_verbose(false);

	// SECTION: Read the mesh from disk using the TriMesh library
	fprintf(stdout, "\n## READ MESH \n");
#ifdef _DEBUG
	trimesh::TriMesh::set_verbose(true);
#endif
	fprintf(stdout, "[I/O] Reading mesh from %s \n", filename.c_str());
	trimesh::TriMesh *themesh = trimesh::TriMesh::read(filename.c_str());
	themesh->need_faces(); // Trimesh: Unpack (possible) triangle strips so we have faces for sure
	fprintf(stdout, "[Mesh] Number of triangles: %zu \n", themesh->faces.size());
	fprintf(stdout, "[Mesh] Number of vertices: %zu \n", themesh->vertices.size());
	fprintf(stdout, "[Mesh] Number of colors: %zu \n", themesh->colors.size());
	fprintf(stdout, "[Mesh] Computing bbox \n");
	themesh->need_bbox(); // Trimesh: Compute the bounding box (in model coordinates)

	// SECTION: Compute some information needed for voxelization (bounding box, unit vector, ...)
	fprintf(stdout, "\n## VOXELISATION SETUP \n");
	// Initialize our own AABox
	AABox<glm::vec3> bbox_mesh(trimesh_to_glm(themesh->bbox.min), trimesh_to_glm(themesh->bbox.max));
	// Transform that AABox to a cubical box (by padding directions if needed)
	// Create voxinfo struct, which handles all the rest
//	If the voxel size is specified, it will cause creation of fixes size voxels and variable size grids
    if (voxel_size > 0){
        // The values should be integers
        gridsize_x = static_cast<unsigned int> ((bbox_mesh.max.x - bbox_mesh.min.x) / voxel_size);
        gridsize_y = static_cast<unsigned int> ((bbox_mesh.max.y - bbox_mesh.min.y) / voxel_size);
        gridsize_z = static_cast<unsigned int> ((bbox_mesh.max.z - bbox_mesh.min.z) / voxel_size);
    }
//	voxinfo voxelization_info(createMeshBBCube<glm::vec3>(bbox_mesh), glm::uvec3(gridsize_x, gridsize_y, gridsize_z), themesh->faces.size());
	voxinfo voxelization_info(bbox_mesh, glm::uvec3(gridsize_x, gridsize_y, gridsize_z), themesh->faces.size());
	voxelization_info.print();
	// Compute space needed to hold voxel table (1 voxel / bit)
	size_t vtable_size = static_cast<size_t>(ceil(static_cast<size_t>(voxelization_info.gridsize.x)* static_cast<size_t>(voxelization_info.gridsize.y)* static_cast<size_t>(voxelization_info.gridsize.z)) / 8.0f);
	size_t colortable_size = static_cast<size_t>(ceil(static_cast<size_t>(voxelization_info.gridsize.x)* static_cast<size_t>(voxelization_info.gridsize.y)* static_cast<size_t>(voxelization_info.gridsize.z) * size_t(4) *  size_t(32)/ 8.0f));
	unsigned int* vtable; // Both voxelization paths (GPU and CPU) need this
	unsigned int* colortable; // Both voxelization paths (GPU and CPU) need this

//	TODO: My sections to see if things work fine
    std::unique_ptr<std::istream> file_stream;
    string base_path = filename.substr (0, filename.find("_aligned"));
    string filepath = base_path + ".labels.ply";

    file_stream.reset(new std::ifstream(filepath, std::ios::binary));
    tinyply::PlyFile  file;

    file_stream->seekg(0, std::ios::end);
    const float size_mb = file_stream->tellg() * float(1e-6);
    file_stream->seekg(0, std::ios::beg);

    file.parse_header(*file_stream);

    std::shared_ptr<tinyply::PlyData> labels;
    try {
        labels = file.request_properties_from_element("vertex", { "label"});
    }
    catch (const std::exception & e) {
        std::cerr << "tinyply exception: " << e.what() << std::endl;
    }
//    Now read the file contents
    file.read(*file_stream);
//    Copy the label information next
    const size_t numLabelsBytes = labels->buffer.size_bytes();
    std::vector<ushort> labels_vector(labels->count);
    std::memcpy(labels_vector.data(), labels->buffer.get(), numLabelsBytes);

//    Again, we need to take care of the remapping operation as well. So,
    std::map<ushort, ushort> remapper;
    remapper[1] = 0;
    remapper[2] = 1;
    remapper[3] = 2;
    remapper[4] = 3;
    remapper[5] = 4;
    remapper[6] = 5;
    remapper[7] = 6;
    remapper[8] = 7;
    remapper[9] = 8;
    remapper[10] = 9;
    remapper[11] = 10;
    remapper[12] = 11;
    remapper[14] = 12;
    remapper[16] = 13;
    remapper[24] = 14;
    remapper[28] = 15;
    remapper[33] = 16;
    remapper[34] = 17;
    remapper[36] = 18;
    remapper[39] = 19;

    for(std::size_t i=0; i<labels_vector.size(); ++i){
//        If the value is not within the keys, it is simply -100
        if (remapper.find(labels_vector.at(i)) != remapper.end())
            labels_vector.at(i) = remapper[labels_vector.at(i)];
        else
            labels_vector.at(i) = 100; //Since only positive values are allowed, we treat 100 now and change it later
    }
    //TODO: End my section

    // SECTION: Try to figure out if we have a CUDA-enabled GPU
	fprintf(stdout, "\n## CUDA INIT \n");
	bool cuda_ok = initCuda();
	if (cuda_ok) {
		fprintf(stdout, "[Info] CUDA GPU found\n");
	}
	else {
		fprintf(stdout, "[Info] CUDA GPU not found\n");
	}

	// SECTION: The actual voxelization
	if (cuda_ok && !forceCPU) {
		// GPU voxelization
		fprintf(stdout, "\n## TRIANGLES TO GPU TRANSFER \n");

		float* device_triangles;
		// Transfer triangles to GPU using either thrust or managed cuda memory
		if (useThrustPath) { device_triangles = meshToGPU_thrust(themesh, labels_vector); }
		else { device_triangles = meshToGPU_managed(themesh); }

		if (!useThrustPath) {
			fprintf(stdout, "[Voxel Grid] Allocating %s of CUDA-managed UNIFIED memory for Voxel Grid\n", readableSize(vtable_size).c_str());
			checkCudaErrors(cudaMallocManaged((void**)&vtable, vtable_size));
		}
		else {
			// ALLOCATE MEMORY ON HOST
			fprintf(stdout, "[Voxel Grid] Allocating %s kB of page-locked HOST memory for Voxel Grid\n", readableSize(vtable_size).c_str());
			checkCudaErrors(cudaHostAlloc((void**)&vtable, vtable_size, cudaHostAllocDefault));
            // ALLOCATE MEMORY ON HOST
            fprintf(stdout, "[Color Grid] Allocating %s kB of page-locked HOST memory for Color Grid\n", readableSize(colortable_size).c_str());
            checkCudaErrors(cudaHostAlloc((void**)&colortable, colortable_size, cudaHostAllocDefault));
		}
		fprintf(stdout, "\n## GPU VOXELISATION \n");
		voxelize(voxelization_info, device_triangles, vtable, colortable, useThrustPath, (outputformat == OutputFormat::output_morton));
	} else {
		// CPU VOXELIZATION FALLBACK
		fprintf(stdout, "\n## CPU VOXELISATION \n");
		if (!forceCPU) { fprintf(stdout, "[Info] No suitable CUDA GPU was found: Falling back to CPU voxelization\n"); }
		else { fprintf(stdout, "[Info] Doing CPU voxelization (forced using command-line switch -cpu)\n"); }
		vtable = (unsigned int*) calloc(1, vtable_size);
		cpu_voxelizer::cpu_voxelize_mesh(voxelization_info, themesh, vtable, (outputformat == OutputFormat::output_morton));
	}

	//// DEBUG: print vtable
	//for (int i = 0; i < vtable_size; i++) {
	//	char* vtable_p = (char*)vtable;
	//	cout << (int) vtable_p[i] << endl;
	//}

	fprintf(stdout, "\n## FILE OUTPUT \n");
	if (outputformat == OutputFormat::output_morton){
		write_binary(vtable, vtable_size, filename);
	} else if (outputformat == OutputFormat::output_binvox){
		write_binvox(vtable, gridsize, filename);
	}
	else if (outputformat == OutputFormat::output_off) {
        write_off(vtable, colortable, gridsize, filename, voxelization_info);
	}

//	TODO: Put a condition to save this file in H5 and not generate Off File
    string outfile = base_path + "_"+ std::to_string(voxel_size * 1000)[0]+ ".data.h5"; // Take the first element from voxel size
    bool success = combine_data(vtable, colortable, gridsize, voxelization_info, outfile);
    printf("\nThe status of print attempt is %d \n", success);
	if (useThrustPath) {
		cleanup_thrust();
	}

	fprintf(stdout, "\n## STATS \n");
	t.stop(); fprintf(stdout, "[Perf] Total runtime: %.1f ms \n", t.elapsed_time_milliseconds);
}
