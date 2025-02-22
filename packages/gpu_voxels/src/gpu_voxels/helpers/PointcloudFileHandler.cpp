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
 * \date    2014-07-10
 *
 */
//----------------------------------------------------------------------

#include "gpu_voxels/helpers/PointcloudFileHandler.h"

namespace gpu_voxels {
namespace file_handling {

// Global static pointer used to ensure a single instance of the class.
PointcloudFileHandler* PointcloudFileHandler::m_instance = NULL;


PointcloudFileHandler* PointcloudFileHandler::Instance()
{
  if(!m_instance) // Only allow one instance of class to be generated.
    m_instance = new PointcloudFileHandler;

  return m_instance;
}

/*!
 * \brief loadPointCloud loads a PCD file and returns the points in a vector.
 * \param path Filename
 * \param points points are written into this vector
 * \param shift_to_zero If true, the pointcloud is shifted, so its minimum coordinates lie at zero
 * \param offset_XYZ Additional transformation offset
 * \return true if succeeded, false otherwise
 */
bool PointcloudFileHandler::loadPointCloud(const std::string _path, const bool use_model_path, std::vector<Vector3f> &points, const bool shift_to_zero,
                    const Vector3f &offset_XYZ, const float scaling)
{
  std::string path;

  // if param is true, prepend the environment variable GPU_VOXELS_MODEL_PATH
  if(use_model_path)
  {
    path = (getGpuVoxelsPath() / boost::filesystem::path(_path)).string();
  }else{
    path = _path;
  }

  LOGGING_DEBUG_C(
      Gpu_voxels_helpers,
      GpuVoxelsMap,
      "Loading Pointcloud file " << path << " ..." << endl);

  // is the file a simple xyz file?
  std::size_t found = path.find(std::string("xyz"));
  if (found!=std::string::npos)
  {
    if (!xyz_reader.readPointCloud(path, points))
    {
      return false;
    }
  }else{
    // is the file a simple pcl pcd file?
    std::size_t found = path.find(std::string("pcd"));
    if (found!=std::string::npos)
    {
      if (!pcd_reader.readPointCloud(path, points))
      {
        return false;
      }
    }else{
      // is the file a binvox file?
      std::size_t found = path.find(std::string("binvox"));
      if (found!=std::string::npos)
      {
        if (!binvox_reader.readPointCloud(path, points))
        {
          return false;
        }
      }else{
        LOGGING_ERROR_C(
            Gpu_voxels_helpers,
            GpuVoxelsMap,
            path << " has no known file format." << endl);
        return false;
      }
    }
  }

  if (shift_to_zero)
  {
    shiftPointCloudToZero(points);
  }

  for (size_t i = 0; i < points.size(); i++)
  {
    points[i].x = (scaling * points[i].x) + offset_XYZ.x;
    points[i].y = (scaling * points[i].y) + offset_XYZ.y;
    points[i].z = (scaling * points[i].z) + offset_XYZ.z;
  }
  return true;
}

/*!
 * \brief centerPointCloud Centers a pointcloud relative to its maximum coordinates
 * \param points Working cloud
 */
void PointcloudFileHandler::centerPointCloud(std::vector<Vector3f> &points)
{
  Vector3f min_xyz = points[0];
  Vector3f max_xyz = points[0];
  Vector3f center_offset_xyz;

  for (size_t i = 1; i < points.size(); i++)
  {
    min_xyz.x = std::min(min_xyz.x, points[i].x);
    min_xyz.y = std::min(min_xyz.y, points[i].y);
    min_xyz.z = std::min(min_xyz.z, points[i].z);

    max_xyz.x = std::max(max_xyz.x, points[i].x);
    max_xyz.y = std::max(max_xyz.y, points[i].y);
    max_xyz.z = std::max(max_xyz.z, points[i].z);
  }

  center_offset_xyz.x = (min_xyz.x + max_xyz.x) / 2.0;
  center_offset_xyz.y = (min_xyz.y + max_xyz.y) / 2.0;
  center_offset_xyz.z = (min_xyz.z + max_xyz.z) / 2.0;

  for (size_t i = 0; i < points.size(); i++)
  {
    points[i].x -= center_offset_xyz.x;
    points[i].y -= center_offset_xyz.y;
    points[i].z -= center_offset_xyz.z;
  }
}

/*!
 * \brief shiftPointCloudToZero Moves a pointcloud, so that its minimum coordinates are shifted to zero.
 * \param points Working cloud
 */
void PointcloudFileHandler::shiftPointCloudToZero(std::vector<Vector3f> &points)
{
  Vector3f min_xyz = points[0];

  for (size_t i = 1; i < points.size(); i++)
  {
    min_xyz.x = std::min(min_xyz.x, points[i].x);
    min_xyz.y = std::min(min_xyz.y, points[i].y);
    min_xyz.z = std::min(min_xyz.z, points[i].z);
  }

  for (size_t i = 0; i < points.size(); i++)
  {
    points[i].x -= min_xyz.x;
    points[i].y -= min_xyz.y;
    points[i].z -= min_xyz.z;
  }
}


boost::filesystem::path getGpuVoxelsPath()
{
  char const* tmp = std::getenv("GPU_VOXELS_MODEL_PATH");
  if (tmp == NULL)
  {
    LOGGING_ERROR_C(
        Gpu_voxels_helpers,
        PointcloudFileHandler,
        "The environment variable 'GPU_VOXELS_MODEL_PATH' could not be read. Did you set it?" << endl);
  }
  return boost::filesystem::path(tmp);
}

}  // end of ns
}  // end of ns
