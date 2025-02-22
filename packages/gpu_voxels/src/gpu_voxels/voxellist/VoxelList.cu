// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

// -- BEGIN LICENSE BLOCK ----------------------------------------------
// -- END LICENSE BLOCK ------------------------------------------------

//----------------------------------------------------------------------
/*!\file
*
* \author  Felix Mauch <mauch@fzi.de>
* \date    2015-05-04
*
*/
//----------------------------------------------------------------------


#include "VoxelList.hpp"
#include <gpu_voxels/voxelmap/VoxelMap.hpp>

namespace gpu_voxels {

template class BitVector<BIT_VECTOR_LENGTH>;
template class BitVoxel<BIT_VECTOR_LENGTH>;


namespace voxellist {

// ############################### BitVoxelList ######################################
// Explicit instantiation of template class to link against from other files where this template is used
template class BitVoxelList<BIT_VECTOR_LENGTH, MapVoxelID>;
template class BitVoxelList<BIT_VECTOR_LENGTH, OctreeVoxelID>;
// ##################################################################################

// ############################### TemplateVoxelList ######################################
// Explicitly instantiate template methods to enable GCC to link agains NVCC compiled objects
//template uint32_t TemplateVoxelMap<ProbabilisticVoxel>::collisionCheckWithCounter<ProbabilisticVoxel, DefaultCollider>(
//                                                                TemplateVoxelMap<ProbabilisticVoxel>*, DefaultCollider);

// ############################### ProbVoxelMap (inherits from TemplateVoxelMap) ######################################
// Explicitly instantiate template methods to enable GCC to link agains NVCC compiled objects
//template void ProbVoxelMap::insertSensorData<BIT_VECTOR_LENGTH>(const Vector3f*, const bool, const bool,
//                                                                const uint32_t, BitVoxel<BIT_VECTOR_LENGTH>*);


// ##################################################################################

} // end of namespace voxellist


} // end of namespace
