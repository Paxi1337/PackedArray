/**
 * PackedArray
 *
 * @file: main.cpp
 * @authors: Christian Ondracek & Lukas Oberbichler
 */


#include <assert.h>
#include <stdio.h>
#include <memory>

#define DEBUG_TEST 1

typedef unsigned int MeshID;

static const unsigned int MAX_MESH_COUNT = 255;

struct Mesh
{
	// the mesh usually has several members (vertex buffer, index buffer, etc.).
	// for our exercise we only use a dummy member. this is only used to check whether the implementation works.
	int dummy;
};

// in our exercise, the RenderWorld only holds meshes, nothing more.
// the RenderWorld implementation makes sure that after *each* operation its internal data structure has no holes,
// and contains Mesh instances which are stored contiguously in memory.
class RenderWorld
{
public:
	RenderWorld(void)
		: m_meshCount(0), mMaxID(0)
	{
		// VS argues about "cannot specify explicit initializer for arrays" when using {0} for initalizing mOverridenCount (works with GCC)
		// therefore it cannot be initialized in initializer list
		std::memset(mOverriddenCount, 0, sizeof(mOverriddenCount));
	}


	MeshID AddMesh(void) {
		assert(m_meshCount < MAX_MESH_COUNT);

		unsigned int index = ((m_meshCount + MAX_MESH_COUNT) % MAX_MESH_COUNT);

		unsigned int ID = index + 1;

		ID += (mOverriddenCount[index]*MAX_MESH_COUNT);

		/// maxID is used to determine later on if a user want to access or remove
		/// a MeshID that has not been created yet
		if(ID > mMaxID) {
			mMaxID = ID;
		}

		/// meshCount increases
		m_meshCount++;


		return ID;
	}


	void RemoveMesh(MeshID id) {
		/// we need 0 based IDs for the following to work
		/// but the user gets 1 based IDs
		id--;

		/// make sure id is not bigger than the current maximum ID
		assert(id <= mMaxID);

		/// get the index associated with this id
		unsigned int index = (id + MAX_MESH_COUNT) %  MAX_MESH_COUNT;

		/// disallow removing a mesh that was already invalidated
		assert(id >= mOverriddenCount[index] * MAX_MESH_COUNT);

		/// if the element is not the last element it will be swapped with the last element to fill the hole
		if(index != m_meshCount)
			m_meshes[index] = m_meshes[m_meshCount - 1];

		mOverriddenCount[index]++;

		/// update mesh count and the next free index
		m_meshCount--;
	}


	Mesh* Lookup(MeshID id) {
		/// we need 0 based IDs for the following to work
		/// but the user gets 1 based IDs
		id--;

		/// make sure id is not bigger than the current maximum ID
		assert(id <= mMaxID);

		unsigned int index = (id + MAX_MESH_COUNT) %  MAX_MESH_COUNT;

		/// a mesh is invalid if the ID is smaller than overriden * MAX_MESH_COUNT
		if(id < mOverriddenCount[index] * MAX_MESH_COUNT)
			return nullptr;

		return &m_meshes[index];
	}


	// DO NOT CHANGE!
	// the implementation of this method needs to stay as it is.
	// you need to correctly implement all other methods to ensure that:
	// a) m_meshCount is up-to-date
	// b) m_meshes stores instances of Mesh contiguously, without any holes
	// c) external MeshIDs still refer to the correct instances
	void Iterate(void) {
		for (unsigned int i=0; i<m_meshCount; ++i) {
			printf("Mesh instance %d: dummy = %d\n", i, m_meshes[i].dummy);
		}
	}


private:
	unsigned int mOverriddenCount[MAX_MESH_COUNT];
	unsigned int m_meshCount;
	// used for assert int RemoveMesh
	unsigned int mMaxID;

	// DO NOT CHANGE!
	// these two members are here to stay. see comments regarding Iterate().
	Mesh m_meshes[MAX_MESH_COUNT];

};



int main(void)
{
	RenderWorld rw;

#if DEBUG_TEST

	// add 3 meshes to the world. we only ever refer to them by their ID, the RenderWorld has complete ownership
	// over the individual Mesh instances.
	MeshID meshID0 = rw.AddMesh();
	MeshID meshID1 = rw.AddMesh();
	MeshID meshID2 = rw.AddMesh();

	// lookup the meshes, and fill them with data.
	{
		Mesh* mesh0 = rw.Lookup(meshID0);
		mesh0->dummy = 0;
		Mesh* mesh1 = rw.Lookup(meshID1);
		mesh1->dummy = 1;
		Mesh* mesh2 = rw.Lookup(meshID2);
		mesh2->dummy = 2;
	}

	// by now, the world contains 3 meshes, filled with dummy data 0, 1 and 2.
	// in memory, the 3 meshes should be contiguous in memory:
	// [Mesh][Mesh][Mesh]
	rw.Iterate();

	// we now remove the second mesh (referenced by meshID1), which creates a hole in the world's data structure:
	// [Mesh][Empty][Mesh]
	// the world should internally update its data structure(s), so that the other two remaining meshes are stored contiguously in memory.
	rw.RemoveMesh(meshID1);

	// iteration must still work, because the instances are contiguous in memory.
	rw.Iterate();

	// even though the render world might have copied/changed some data structures, the two remaining meshes must still
	// refer to the correct object. this is verified by checking their dummy members.
	assert(rw.Lookup(meshID0)->dummy == 0);
	assert(rw.Lookup(meshID2)->dummy == 2);

	// the mesh referenced by meshID1 has been removed above, yet we intentionally try to access it.
	// the implementation should give an error, and return a nullptr in that case.
	Mesh* mesh1 = rw.Lookup(meshID1);
	assert(mesh1 == nullptr);

#else
	/**
	 * TEST 1
	 *
	 * fill whole packed array and return all meshes
	 */
	for(unsigned int i = 0; i < MAX_MESH_COUNT; ++i) {
		rw.AddMesh();
	}

	for(unsigned int i = 1; i < MAX_MESH_COUNT + 1; ++i) {
		Mesh* m = rw.Lookup(i);
		m->dummy = i;
	}

	printf("\nTEST 1\n");
	rw.Iterate();

	/**
	 * TEST 2
	 *
	 * delete first and last mesh and iterate
	 */

	for (unsigned int i = 1; i < MAX_MESH_COUNT + 1; ++i) {
		rw.RemoveMesh(i);
	}

	/*for (unsigned int i = MAX_MESH_COUNT - 1; i > 0; --i) {
		rw.RemoveMesh(i);
	}

	rw.RemoveMesh(0);*/

	printf("\n\n");
	rw.Iterate();


	for (unsigned int i = 0; i < MAX_MESH_COUNT; ++i) {
		rw.AddMesh();
	}

	//Mesh* m1 = rw.Lookup(0);
	//m1->dummy = 5;

	//rw.RemoveMesh(1);

	//Mesh* m2 = rw.Lookup(3);
	//m1 = rw.Lookup(0);


	//rw.RemoveMesh(MAX_MESH_COUNT - 1);

	//rw.AddMesh();
	//rw.AddMesh();



	Mesh* m1 = rw.Lookup(300);
	m1->dummy = 5;

	Mesh* m2 = rw.Lookup(2);
	(void)sizeof(m2);

	//rw.RemoveMesh(2048);

	//m2->dummy = 10;

	//Mesh* m2 = rw.Lookup(6);
	//m2->dummy = 6;

	printf("\nTEST 2\n");

	rw.Iterate();
#endif

	return 0;
}
