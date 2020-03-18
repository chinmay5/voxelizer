#include "thrust_operations.cuh"

// thrust vectors (global) (see https://stackoverflow.com/questions/54742267/having-thrustdevice-vector-in-global-scope)
thrust::host_vector<glm::vec3>* trianglethrust_host;
thrust::device_vector<glm::vec3>* trianglethrust_device;

// method 3: use a thrust vector
float *meshToGPU_thrust(const trimesh::TriMesh *mesh, vector<ushort> labels) {
	Timer t; t.start(); // TIMER START
	// create vectors on heap 
	trianglethrust_host = new thrust::host_vector<glm::vec3>;
	trianglethrust_device = new thrust::device_vector<glm::vec3>;
	// fill host vector
	fprintf(stdout, "[Mesh] Copying %zu triangles to Thrust host vector \n", mesh->faces.size());
	for (size_t i = 0; i < mesh->faces.size(); i++) {
		glm::vec3 v0 = trimesh_to_glm<trimesh::point>(mesh->vertices[mesh->faces[i][0]]);
		glm::vec3 v1 = trimesh_to_glm<trimesh::point>(mesh->vertices[mesh->faces[i][1]]);
		glm::vec3 v2 = trimesh_to_glm<trimesh::point>(mesh->vertices[mesh->faces[i][2]]);
//		Include the color info as well
		glm::vec3 c1 = trimesh_to_glm<trimesh::Color>(mesh->colors[mesh->faces[i][0]]);
		glm::vec3 c2 = trimesh_to_glm<trimesh::Color>(mesh->colors[mesh->faces[i][1]]);
		glm::vec3 c3 = trimesh_to_glm<trimesh::Color>(mesh->colors[mesh->faces[i][2]]);
//		Also include the color information
        glm::vec3 ll (labels.at(mesh->faces[i][0]), labels.at(mesh->faces[i][1]), labels.at(mesh->faces[i][2]));
//        Now pushing the values back
		trianglethrust_host->push_back(v0);
		trianglethrust_host->push_back(v1);
		trianglethrust_host->push_back(v2);
//		Push the color info as well
        trianglethrust_host->push_back(c1);
        trianglethrust_host->push_back(c2);
        trianglethrust_host->push_back(c3);
//        Also push back the label information
        trianglethrust_host->push_back(ll);
	}
	fprintf(stdout, "[Mesh] Copying Thrust host vector to Thrust device vector \n");
	*trianglethrust_device = *trianglethrust_host;
	t.stop(); fprintf(stdout, "[Mesh] Transfer time to GPU: %.1f ms \n", t.elapsed_time_milliseconds); // TIMER END
	return (float*) thrust::raw_pointer_cast(&((*trianglethrust_device)[0]));
}

void cleanup_thrust(){
	fprintf(stdout, "[Mesh] Freeing Thrust host and device vectors \n");
	if (trianglethrust_device) free(trianglethrust_device);
	if (trianglethrust_host) free(trianglethrust_host);
}