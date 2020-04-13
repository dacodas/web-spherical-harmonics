#!/bin/bash

icosahedron() {
	mkdir build/icosahedron
	python src/utility/icosahedron/subdivide.py
}

template() {

	while read TEMPLATE_FILE
	do
		envsubst \
			<${TEMPLATE_FILE} \
			>${TEMPLATE_FILE%.template}

		rm ${TEMPLATE_FILE}
	done
}

srv() {
	read ELEMENT_INDICES_LENGTH < <( wc -l < build/icosahedron/indices )
	read ELEMENT_INDICES_LENGTH < <( echo "3 * ${ELEMENT_INDICES_LENGTH}" | bc )

	read -d '' ELEMENT_INDICES < build/icosahedron/indices
	read -d '' SAMPLE_POSITIONS < build/icosahedron/vertices

	export ELEMENT_INDICES ELEMENT_INDICES_LENGTH SAMPLE_POSITIONS

	echo $ELEMENT_INDICES_LENGTH

	rsync -av srv/ build/srv/

	find build/ -name '*.template' | template

	# envsubst srv/
}

"${@}"
