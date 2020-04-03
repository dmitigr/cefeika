// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or img.hpp

#include "dmitigr/img/graphicsmagick.hpp"

#include <iostream>
#include <memory>

#include <GraphicsMagick/Magick++.h>

namespace dmitigr::img::graphicsmagick {

DMITIGR_IMG_INLINE void init(const char* const app_path)
{
  ::Magick::InitializeMagick(app_path);
}

DMITIGR_IMG_INLINE const char* file_type_c_str(const File_type file_type)
{
  switch (file_type) {
  case File_type::gif:  { return "GIF"; }
  case File_type::jpeg: { return "JPEG"; }
  case File_type::png:  { return "PNG"; }
  }
}

DMITIGR_IMG_INLINE void resize(std::istream& input,
  std::ostream& output,
  const unsigned int output_x,
  const unsigned int output_y,
  const File_type output_type,
  const unsigned int output_quality)
{
  input.seekg(0, std::ios::end);
  const auto input_size = input.tellg();
  input.seekg(0, std::ios::beg);

  const std::unique_ptr<char[]> input_data{new char[input_size]};
  input.read(input_data.get(), input_size);

  // The input_data will be owned by the input_blob
  Magick::Blob input_blob;
  input_blob.updateNoCopy(input_data.release(), input_size);

  // Resize and write the result to the output
  {
    Magick::Blob output_blob;

    // The actual resizing goes here
    {
      Magick::Image img(input_blob);
      img.strip();
      img.quality(output_quality);
      img.magick(file_type_c_str(output_type));
      img.zoom(Magick::Geometry{output_x, output_y});
      img.write(&output_blob);
    }

    output.write(static_cast<const char*>(output_blob.data()), output_blob.length());
  }
}

} // namespace dmitigr::img::graphicsmagick
