// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

// -- BEGIN LICENSE BLOCK ----------------------------------------------
// -- END LICENSE BLOCK ------------------------------------------------

//----------------------------------------------------------------------
/*!\file
*
* \author  Felix Mauch <mauch@fzi.de>
* \date    2015-06-16
*
*/
//----------------------------------------------------------------------

#ifndef GPU_VOXELS_VOXEL_DEFAULT_SVCOLLIDER_HPP
#define GPU_VOXELS_VOXEL_DEFAULT_SVCOLLIDER_HPP

#include "SVCollider.h"
#include <gpu_voxels/voxel/BitVoxel.hpp>
#include <gpu_voxels/voxel/ProbabilisticVoxel.hpp>

namespace gpu_voxels {

SVCollider::SVCollider() :
    m_threshold1(100), m_threshold2(100),
    m_type_range(10)
{

}

SVCollider::SVCollider(const float coll_threshold, const size_t window_size) :
    m_threshold1(floatToProbability(coll_threshold)), m_threshold2(floatToProbability(coll_threshold)),
    m_type_range(window_size)
{

}

SVCollider::SVCollider(const probability threshold1, const probability threshold2, const size_t window_size) :
    m_threshold1(threshold1), m_threshold2(threshold2),
    m_type_range(window_size)
{

}

bool SVCollider::collide(const ProbabilisticVoxel& v1, const ProbabilisticVoxel& v2) const
{
  return v1.getOccupancy() >= m_threshold1 && v2.getOccupancy() > m_threshold2;
}

bool SVCollider::collide(const ProbabilisticVoxel& v1) const
{
  return v1.getOccupancy() >= m_threshold1;
}

template<std::size_t length>
__host__ __device__
bool SVCollider::collide(const ProbabilisticVoxel& v1, const BitVoxel<length>& v2) const
{
  return v1.getOccupancy() >= m_threshold1 && !v2.bitVector().isZero();
}

template<std::size_t length>
__host__ __device__
bool SVCollider::collide(const BitVoxel<length>& v1, const ProbabilisticVoxel& v2) const
{
  return collide(v2, v1);
}


template<std::size_t length>
__host__ __device__
bool SVCollider::collide(const BitVoxel<length>& v1, const BitVoxel<length>& v2) const
{
  BitVector<length> collisions;

  return collide(v1, v2, &collisions);
}


template<std::size_t length>
__host__ __device__
bool SVCollider::collide(const BitVoxel<length>& v1, const BitVoxel<length>& v2,
                         BitVector<length>* collisions, const uint32_t sv_offset) const
{
  return bitMarginCollisionCheck<length>(v1.bitVector(), v2.bitVector(), collisions, m_type_range, sv_offset);
}

probability SVCollider::floatToProbability(const float val)
{
  float tmp = (val * (float(MAX_PROBABILITY) - float(MIN_PROBABILITY))) + MIN_PROBABILITY;
  return probability(tmp);
}

} // end of ns
#endif // GPU_VOXELS_VOXEL_DEFAULT_SVCOLLIDER_HPP
