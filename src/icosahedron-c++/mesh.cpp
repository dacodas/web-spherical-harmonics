struct WshTraits : public OpenMesh::DefaultTraits
{
	VertexAttributes( 
			OpenMesh::Attributes::Normal |
			OpenMesh::Attributes::Color |
			OpenMesh::Attributes::TexCoord2D
		);
};

using Mesh = OpenMesh::TriMesh_ArrayKernelT<WshTraits>;
