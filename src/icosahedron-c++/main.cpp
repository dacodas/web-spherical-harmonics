#include <iostream>
#include <fstream>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMeshT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include "borderPhi.cpp"
#include "mesh.cpp"
#include "spherical.cpp"
#include "writeArray.cpp"

typedef struct Context {
	std::ostream& vertices_file;
	std::ostream& indices_file;
	Mesh& mesh;
	size_t index;
	bool correctPhi;
} Context;

typedef struct FaceContext {
	Context& context;
	const Mesh::FaceHandle& face;
} FaceContext;

void writeVertexToFile(const Mesh::VertexHandle& vertex, FaceContext& face_context)
{
	const auto& v = face_context.context.mesh.vertex(vertex);
	auto [theta, phi] = spherical(face_context.context.mesh.point(vertex));

	if ( face_context.context.correctPhi ) { phi = adjustPhi(phi); }

	face_context.context.vertices_file << theta / M_PI << " " << phi / (2.0 * M_PI) << "\n";
	face_context.context.indices_file << face_context.context.index++ << "\n";
}

void writeFaceToFile(const Mesh::FaceHandle& face, Context& context)
{
	FaceContext face_context {context, face};

	std::for_each_n(
			context.mesh.fv_iter(face), 3,
			[&face_context] (const Mesh::VertexHandle& vertex)
			{
				writeVertexToFile(vertex, face_context);
			}
		       );
}

bool writePatch(const Mesh::FaceHandle& face, Context& context)
{
	for ( Mesh::FaceVertexIter fv_it = context.mesh.fv_iter(face);
		fv_it.is_valid();
		++fv_it )
	{
		auto [theta, phi] = spherical(context.mesh.point(*fv_it));
		if ( borderPhiLow(phi) or borderPhiHigh(phi) ) 
		{
			writeFaceToFile(face, context);
			return true;
		}
	}

	return false;
}

int main() 
{
	Mesh mesh;

	const std::string input_mesh {"build/icosahedron/subdivided-5.stl"};
	if (!OpenMesh::IO::read_mesh(mesh, input_mesh))
	{
		std::cerr << "Error reading mesh " << input_mesh << "\n";
		return 1;
	}

	// Find all faces with phi on the border 
	std::vector<std::vector<Mesh::VertexHandle>> border_faces {};

	std::string vertices_file {"build/icosahedron-c++/vertices"};
	std::string indices_file {"build/icosahedron-c++/indices"};
	std::string patch_vertices_file {"build/icosahedron-c++/patch-vertices"};
	std::string patch_indices_file {"build/icosahedron-c++/patch-indices"};
	{
		std::ofstream vertices_ostream(vertices_file);
		std::ofstream indices_ostream(indices_file);
		std::ofstream patch_vertices_ostream(patch_vertices_file);
		std::ofstream patch_indices_ostream(patch_indices_file);

		Context context {vertices_ostream, indices_ostream, mesh, 0, false};
		std::for_each(
				mesh.faces_begin(),
				mesh.faces_end(),
				[&] (const Mesh::FaceHandle& face)  
				{
					writeFaceToFile(face, context);	
				}
			     );

		Context phi_context {vertices_ostream, indices_ostream, mesh, context.index, true};
		// Context phi_context {patch_vertices_ostream, patch_indices_ostream, mesh, context.index, true};
		// Context phi_context {patch_vertices_ostream, patch_indices_ostream, mesh, 0, true};
		std::for_each(
				mesh.faces_begin(),
				mesh.faces_end(),
				[&] (const Mesh::FaceHandle& face)  
				{
					writePatch(face, phi_context);
				}
			     );
	}
}

