// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

// -- BEGIN LICENSE BLOCK ----------------------------------------------
// This file is part of the GPU Voxels Software Library.
//
// This program is free software licensed under the CDDL
// (COMMON DEVELOPMENT AND DISTRIBUTION LICENSE Version 1.0).
// You can find a copy of this license in LICENSE.txt in the top
// directory of the source code.
//
// © Copyright 2014 FZI Forschungszentrum Informatik, Karlsruhe, Germany
//
// -- END LICENSE BLOCK ------------------------------------------------

//----------------------------------------------------------------------
/*!\file
 *
 * \author  Sebastian Klemm
 * \date    2012-09-13
 *
 */
//----------------------------------------------------------------------
#ifndef GPU_VOXELS_VOXELMAP_BIT_VOXELMAP_H_INCLUDED
#define GPU_VOXELS_VOXELMAP_BIT_VOXELMAP_H_INCLUDED

#include <gpu_voxels/voxelmap/TemplateVoxelMap.h>
#include <gpu_voxels/voxel/BitVoxel.h>
#include <gpu_voxels/voxel/ProbabilisticVoxel.h>
#include <gpu_voxels/voxelmap/ProbVoxelMap.h>
#include <cstddef>

namespace gpu_voxels {
namespace voxelmap {

template<std::size_t length>
class BitVoxelMap: public TemplateVoxelMap<BitVoxel<length> >
{
public:
  typedef BitVoxel<length> Voxel;
  typedef TemplateVoxelMap<Voxel> Base;

  BitVoxelMap(const uint32_t dim_x, const uint32_t dim_y, const uint32_t dim_z, const float voxel_side_length, const MapType map_type);
  BitVoxelMap(Voxel* dev_data, const Vector3ui dim, const float voxel_side_length, const MapType map_type);

  virtual ~BitVoxelMap();

  virtual void clearBit(const uint32_t bit_index);

  virtual void clearBits(BitVector<length> bits);

  /**
   * @brief Collides two Bit-Voxelmaps and delivers the Voxelmeanings that lie in collision
   * \param other The map to collide with
   * \param collider The collider kernel to use
   * \param colliding_meanings The result vector in which the colliding meanings are set to 1
   * \param sv_offset Offset which is added while checking to ignore the first bits
   */
  template<class Collider>
  uint32_t collisionCheckBitvector(BitVoxelMap<length>* other, Collider collider,
                                   BitVector<length>& colliding_meanings, const uint16_t sv_offset = 0);

  void triggerAddressingTest(Vector3ui dimensions, float voxel_side_length, size_t nr_of_tests, bool *success);

  /**
   * @brief Shifts all swept-volume-IDs by shift_size towards lower IDs.
   * Currently this is limited to a shift size <64
   * @param shift_size Shift size of bitshift
   */
  void shiftLeftSweptVolumeIDs(uint8_t shift_size);

  virtual bool insertRobotConfiguration(const MetaPointCloud *robot_links, bool with_self_collision_test);

  virtual void clearBitVoxelMeaning(BitVoxelMeaning voxel_meaning);

  virtual MapType getTemplateType() { return MT_BITVECTOR_VOXELMAP; }

  virtual size_t collideWithTypes(const GpuVoxelsMapSharedPtr other, BitVectorVoxel&  meanings_in_collision,
                                  float coll_threshold, const Vector3ui &offset);

protected:
  virtual void clearVoxelMapRemoteLock(const uint32_t bit_index);
};

} // end of namespace
} // end of namespace

#endif
