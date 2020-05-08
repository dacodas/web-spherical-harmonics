#!/bin/bash

icosahedron() {
	mkdir build/icosahedron
	python src/utility/icosahedron/subdivide.py
}

template() {

	# Environment variables and arguments share the same memory 
	# space which is limited by ARG_MAX. Because of this, I can't 
	# just envsubst the entirety of the indices and vertices files

	while read TEMPLATE_FILE
	do
		podman unshare bash -c \
			"envsubst \
				<${TEMPLATE_FILE} \
				>${TEMPLATE_FILE%.template}"

		podman unshare rm ${TEMPLATE_FILE}
	done
}

template_srv() {
	set -a

	# ELEMENT_INDICES=build/icosahedron/indices
	# SAMPLE_POSITIONS=build/icosahedron/vertices
	ELEMENT_INDICES=build/icosahedron-c++/indices
	SAMPLE_POSITIONS=build/icosahedron-c++/vertices

	read ELEMENT_INDICES_LENGTH < <( wc -l < ${ELEMENT_INDICES} )
	# read ELEMENT_INDICES_LENGTH < <( echo "3 * ${ELEMENT_INDICES_LENGTH}" | bc )

	TEXTURE_VERTEX_SHADER=src/shaders/texture/vertex.glsl
	TEXTURE_FRAGMENT_SHADER=src/shaders/texture/fragment.glsl
	SURFACE_VERTEX_SHADER=src/shaders/surface/vertex.glsl
	SURFACE_FRAGMENT_SHADER=src/shaders/surface/fragment.glsl
	BASE_VERTEX_SHADER=src/shaders/base/vertex.glsl
	BASE_FRAGMENT_SHADER=src/shaders/base/fragment.glsl

	set +a

	podman unshare rsync -av srv/ build/srv/

	find build/ -name '*.template' | template
}

run() {
	podman run \
		--rm \
		--name web-spherical-harmonics\
		--detach \
		--publish 7081:80 \
		--volume $(pwd)/build/srv/:/usr/local/apache2/htdocs/:z \
		container-registry.dacodastrack.com/web-spherical-harmonics
}

"${@}"
