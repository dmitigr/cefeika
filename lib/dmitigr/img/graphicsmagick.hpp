// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or img.hpp

#ifndef DMITIGR_IMG_GRAPHICSMAGICK_HPP
#define DMITIGR_IMG_GRAPHICSMAGICK_HPP

#include "dmitigr/img/dll.hpp"

#include <iosfwd>

namespace dmitigr::img::graphicsmagick {

enum class File_type { gif = 1, jpeg, png };

/**
 * @brief The GraphicsMagick initialization.
 */
DMITIGR_IMG_API void init(const char* app_path);

/**
 * @returns MIME in the GraphicsMagick notation.
 */
DMITIGR_IMG_API const char* file_type_c_str(File_type file_type);

/**
 * @brief Resizes the image read from `input` and writes the result to `output`.
 *
 * @param output_quality - JPEG/MIFF/PNG compression level.
 */
DMITIGR_IMG_API void resize(std::istream& input,
  std::ostream& output,
  unsigned int output_x,
  unsigned int output_y,
  File_type output_type,
  unsigned int output_quality = 75);

} // namespace dmitigr::img::graphicsmagick

#ifdef DMITIGR_IMG_HEADER_ONLY
#include "dmitigr/img/graphicsmagick.cpp"
#endif

#endif  // DMITIGR_IMG_GRAPHICSMAGICK_HPP
