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
 * \author  Florian Drews
 * \date    2014-07-08
 *
 */
//----------------------------------------------------------------------/*
#ifndef GPU_VOXELS_BIT_VECTOR_H_INCLUDED
#define GPU_VOXELS_BIT_VECTOR_H_INCLUDED

#include <cstring>
#include <cuda.h>
#include <stdio.h>
#include <ostream>
#include <istream>
#include <bitset>
#include "thrust/functional.h"
#include <gpu_voxels/helpers/common_defines.h>
#include <gpu_voxels/helpers/cuda_handling.h>

namespace gpu_voxels {

/**
 * @brief This template class represents a vector of bits with a given number of bits.
 */
template<std::size_t num_bits>
class BitVector
{

public:
  typedef uint8_t item_type;

protected:

/**
 * @brief getBit Gets the reference to the given index position (given in Bits).
 * @return Reference to byte of given index position
 */
__host__ __device__
item_type* getByte(const uint32_t index)
{
  return &m_bytes[index >> 3];
}


public:

  __host__     __device__
  BitVector()
  {
    if(num_bits % 32 != 0)
    {
      printf("Critical Error: Bitvector size must be a multitude of 32 Bits!");
    }
    assert(num_bits % 32 == 0);
    clear();
  }

  /**
   * @brief clear Sets all bits to zero.
   */
  __host__ __device__
  void clear()
  {
    memset(m_bytes, 0, sizeof(m_bytes));
  }

  /**
   * @brief operator | Bitwise or-operator
   * @param o Other operand

   */
  __host__     __device__
  BitVector<num_bits> operator|(const BitVector<num_bits>& o) const
  {
    BitVector<num_bits> res;
  #if defined(__CUDACC__) && !defined(__GNUC__)
  # pragma unroll
  #endif
    for (uint32_t i = 0; i < m_size; ++i)
      res.m_bytes[i] = m_bytes[i] | o.m_bytes[i];
    return res;
  }

  /**
   * @brief operator == Bitwise equal comparison
   * @param o Other operand
   */
  __host__     __device__
  bool operator==(const BitVector<num_bits>& o) const
  {
  #if defined(__CUDACC__) && !defined(__GNUC__)
  # pragma unroll
  #endif
    for (uint32_t i = 0; i < m_size; ++i)
    {
      if(m_bytes[i] != o.m_bytes[i]) return false;
    }
    return true;
  }

  /**
   * @brief operator |= Bitwise or-operator
   * @param o Other operand
   */
  __host__ __device__
  void operator|=(const BitVector<num_bits>& o)
  {
  #if defined(__CUDACC__) && !defined(__GNUC__)
  # pragma unroll
  #endif
    for (uint32_t i = 0; i < m_size; ++i)
      m_bytes[i] |= o.m_bytes[i];
  }

  /**
   * @brief operator ~ Bitwise not-operator
   * @return Returns the bitwise not of 'this'
   */
  __host__     __device__
  BitVector<num_bits> operator~() const
  {
    BitVector<num_bits> res;
  #if defined(__CUDACC__) && !defined(__GNUC__)
  # pragma unroll
  #endif
    for (uint32_t i = 0; i < m_size; ++i)
      res.m_bytes[i] = ~res.m_bytes[i];
    return res;
  }

  /**
   * @brief operator ~ Bitwise and-operator
   * @return Returns the bitwise and of 'this'
   */
  __host__     __device__
  BitVector<num_bits> operator&(const BitVector<num_bits>& o) const
  {
    BitVector<num_bits> res;
  #if defined(__CUDACC__) && !defined(__GNUC__)
  # pragma unroll
  #endif
    for (uint32_t i = 0; i < m_size; ++i)
      res.m_bytes[i] = m_bytes[i] & o.m_bytes[i];
    return res;
  }

  /**
   * @brief isZero Checks the bit vector for zero
   * @return True if all bits are zero, false otherwise
   */
  __host__ __device__
  bool isZero() const
  {
    bool result = true;
  #if defined(__CUDACC__) && !defined(__GNUC__)
  # pragma unroll
  #endif
    for (uint32_t i = 0; i < m_size; ++i)
      result &= m_bytes[i] == 0;
    return result;
  }

  /**
   * @brief getBit Gets the bit with at the given index.
   * @return Value of the selected bit.
   */
  __host__ __device__
  bool getBit(const uint32_t index) const
  {
    return getByte(index) & (1 << (index & 7));
  }

  /**
   * @brief clearBit Clears the bit at the given index
   */
  __host__ __device__
  void clearBit(const uint32_t index)
  {
    item_type* selected_byte = getByte(index);
    *selected_byte = *selected_byte & item_type(~(1 << (index & 7)));
  }

  /**
   * @brief setBit Sets the bit with at the given index.
   */
  __host__ __device__
  void setBit(const uint32_t index)
  {
    item_type* selected_byte = getByte(index);
    *selected_byte = *selected_byte | item_type(1 << (index & 7));
  }

  /**
   * @brief getByte Gets the byte to the given index position.
   * Note: The dummy argument helps nvcc to distinguish this function from the protected pointer version
   *
   * @return Byte of given index position
   */
  __host__ __device__
  item_type getByte(const uint32_t index, const uint8_t dummy = 0) const
  {
    return m_bytes[index >> 3];
  }

  /**
   * @brief setByte Sets the byte at the given index position.
   * @param index Which byte to set
   * @param data Data to write into byte
   */
  __host__ __device__
  void setByte(const uint32_t index, const item_type data)
  {
    item_type* selected_byte = getByte(index);
    *selected_byte = data;
  }


   __host__ __device__
   void dump()
   {
     const size_t byte_size = sizeof(typename BitVector<num_bits>::item_type);
     printf("[");
     for(std::size_t i = 0; i < num_bits; i+=byte_size*8)
     {
       printf(" %hu", *(getByte(i)));
     }
     printf(" ]\n");
   }

  /**
   * @brief operator >> Overloaded ostream operator. Please note that the output bit string is starting from
   * Type 0.
   */
  __host__
  friend std::ostream& operator<<(std::ostream& os, const BitVector<num_bits>& dt)
  {
    const size_t byte_size = sizeof(typename BitVector<num_bits>::item_type);
    typename BitVector<num_bits>::item_type byte = 0;
    for(std::size_t i = 0; i < num_bits; i+=byte_size*8)
    {
      byte = dt.getByte(i);
      // reverse bit order
      byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
      byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
      byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
      std::bitset<byte_size*8> bs(byte);
      os << bs;
    }
    return os;
  }


  /**
   * @brief operator << Overloaded istream operator. Please note that the input bit string should
   * be starting from Type 0 and it should be complete, meaning it should have all Bits defined.
   */
  __host__
  friend std::istream& operator>>(std::istream& in, BitVector<num_bits>& dt)
  {
    typename BitVector<num_bits>::item_type byte;
    std::bitset<num_bits> bs;
    in >> bs;
    const size_t byte_size = sizeof(typename BitVector<num_bits>::item_type);
    for(std::size_t i = 0; i < num_bits; i+=byte_size*8)
    {
      // The Bit reverse is in here
      byte = bs[i+7] + 2*bs[i+6] + 4*bs[i+5] + 8*bs[i+4] + 16*bs[i+3] + 32*bs[i+2] + 64*bs[i+1] + 128*bs[i+0];

      // Fill last bit first
      dt.setByte(num_bits-i-1, byte);
    }
    return in;
  }



  // This CUDA Code was taken from Florians BitVoxelFlags that got replaced by BitVectors
  #ifdef __CUDACC__
    __device__
    static void reduce(BitVector<num_bits>& flags, const int thread_id, const int num_threads,
                       BitVector<num_bits>* shared_mem)
    {
  #if defined(__CUDACC__) && !defined(__GNUC__)
  # pragma unroll
  #endif
      shared_mem[thread_id] = flags;
      __syncthreads();
      REDUCE(shared_mem, thread_id, num_threads, |)
      if (thread_id == 0)
        flags = shared_mem[0];
      __syncthreads();
    }
  #endif

  #ifdef __CUDACC__
    __device__
    static void reduceAtomic(BitVector<num_bits>& flags, BitVector<num_bits>& global_flags)
    {
  #if defined(__CUDACC__) && !defined(__GNUC__)
  # pragma unroll
  #endif
      // interpret the 4 bytes as interger:
      for (uint32_t i = 0; i < m_size/4; i+=4)
      {
        // This is possible, as Vectors have to be a mutiple of 32 Bits
        int* tmp = (int*)(flags.m_bytes[0] + i);
        atomicOr((int*)(&global_flags.m_bytes[0] + i), *tmp);
      }
    }
  #endif

protected:
  static const uint32_t m_size = (num_bits + 7) / 8; // the size in Byte
  item_type m_bytes[m_size];

}; // END OF CLASS BitVector


/**
 * @brief performLeftShift Shifts the bits of a bitvector to the left
 * @param shift_size How many bits to shift.
 */
template<std::size_t num_bits>
__host__ __device__
void performLeftShift(BitVector<num_bits>& bit_vector, const uint8_t shift_size)
{
  uint64_t buffer = 0;
  uint8_t byte_shift_size = shift_size/8;

//    printf("Shifting by %d byte and %d bit to the right\n", byte_shift_size, bit_shift_size);

  for (uint32_t byte_idx = 0; byte_idx < 8; ++byte_idx)
  {
    const uint32_t shifted_byte_idx = (byte_idx+1) * 8;
    typename BitVector<num_bits>::item_type byte = bit_vector.getByte(shifted_byte_idx, 1);
    buffer += static_cast<uint64_t>(byte) << (byte_idx*8);
  }

//    printf("Buffer at start is %lu\n", buffer);

  for (uint32_t byte_idx = 1; byte_idx < num_bits/8 - byte_shift_size - 1; ++byte_idx)
  {
    uint8_t new_byte;
    new_byte = static_cast<uint8_t>(buffer >> shift_size);
    // only watch SV meanings
    if (byte_idx == 1)
    {
      new_byte = new_byte & 0xFC; //0b11111100;
    }
    bit_vector.setByte(byte_idx * 8, new_byte);
    buffer = buffer >> 8;
//      printf("New buffer at step %u is %lu\n", byte_idx*8, buffer);

    buffer += static_cast<uint64_t>(bit_vector.getByte((byte_idx+8) * 8, 1)) << 56;
  }
}



/**
 * @brief bitMarginCollisionCheck
 * @param v1 Bitvector 1
 * @param v2 Bitvector 2
 * @param collisions Aggregates the colliding bits. This will NOT get reset!
 * @param margin Fuzzyness of the check. How many bits get checked aside the actual colliding bit.
 * @param sv_offset Bit-Offset added to v1 before colliding
 * @return
 */
template<std::size_t num_bits>
__host__ __device__
bool bitMarginCollisionCheck(const BitVector<num_bits>& v1, const BitVector<num_bits>& v2,
                             BitVector<num_bits>* collisions, const uint8_t margin, const uint32_t sv_offset)
{
  uint64_t buffer = 0;
  const size_t buffer_half = 4*8; // middle of uint64_t
  if (margin > buffer_half)
  {
    printf("ERROR: Window size for SV collision check must be smaller than %lu\n", buffer_half);
  }

  // Fill buffer with first 4 bytes. We start at byte 1 and not 0 because we're only interested in SV IDs
  for (size_t byte_nr = 1; byte_nr < 5; ++byte_nr)
  {
    buffer += static_cast<uint64_t>(v2.getByte(byte_nr * 8)) << (3*8 + byte_nr*8);
  }

  uint8_t byte_offset = sv_offset % 8;
  uint8_t bit_offset = sv_offset / 8;

  // We start at bit 8 and not 0 because we're only interested in SV IDs
  for (uint32_t i = 8 + byte_offset; i < eBVM_SWEPT_VOLUME_END; i+=8)
  {

    uint8_t byte = 0;
    uint64_t byte_1 = static_cast<uint64_t>(v1.getByte(i)) << (buffer_half-margin + bit_offset);

    // Check range for collision
    for (size_t bitshift_size=0; bitshift_size <= 2*margin; ++bitshift_size)
    {
      byte |= (byte_1 & buffer) >> (buffer_half - margin + bitshift_size);
//      if ((byte_1 & buffer) != 0)
//      {
//        printf("Byte_1 step %u is %lu, buffer is %lu, Overlapping: %u\n", i/8, byte_1, buffer, byte);
//      }
      byte_1 = byte_1 << 1;
    }

    collisions->setByte(i, byte);

    // Move buffer along bitvector
    buffer = buffer >> 8;
    if (i < num_bits - buffer_half)
    {
      buffer += static_cast<uint64_t>(v2.getByte(i + buffer_half)) << 56;
    }
  }
  return !collisions->isZero();
}



/*!
 * \brief The BitvectorOr struct
 * Thrust operator that calculates the OR operation on two BitVectors
 */
template<std::size_t num_bits>
struct BitvectorOr : public thrust::binary_function<BitVector<num_bits>,BitVector<num_bits>,BitVector<num_bits> >
{
  __host__ __device__
  BitVector<num_bits> operator()(const BitVector<num_bits> &lhs, const BitVector<num_bits> &rhs) const
  {
    return lhs | rhs;
  }

};

} // end of ns

#endif
