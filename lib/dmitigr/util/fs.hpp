// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or util.hpp

#ifndef DMITIGR_UTIL_FS_HPP
#define DMITIGR_UTIL_FS_HPP

#include "dmitigr/util/dll.hpp"
#include "dmitigr/util/filesystem.hpp"

#include <fstream>
#include <string>
#include <vector>

namespace dmitigr::fs {

/**
 * @returns The vector of the paths.
 *
 * @param root - the search root;
 * @param extension - the extension of files to be included into the result;
 * @param recursive - if `true` then do the recursive search;
 * @param include_heading - if `true` then include the "heading file" (see
 * remarks) into the result.
 *
 * @remarks The "heading file" - is a regular file with the given `extension`
 * which has the same parent directory as the `root`.
 */
DMITIGR_UTIL_API std::vector<std::filesystem::path> files_by_extension(const std::filesystem::path& root,
  const std::filesystem::path& extension, bool recursive, bool include_heading = false);

/**
 * @brief Searches for the `dir` directory in the current working directory and
 * in the parent directories.
 *
 * @returns The path to the `dir` directory.
 */
DMITIGR_UTIL_API std::filesystem::path relative_root_path(const std::filesystem::path& dir);

/**
 * @brief Copies lines of the given file into the vector of strings.
 *
 * This function calls the the callback `pred(line)`, where line - is a line
 * that has been read from a file, and the `line` is stored into the result
 * vector only if the callback returns `true`.
 */
template<typename Pred>
std::vector<std::string> read_lines_to_vector_if(const std::filesystem::path& path, Pred pred)
{
  std::vector<std::string> result;
  std::string line;
  std::ifstream lines{path, std::ios_base::in | std::ios_base::binary};
  while (getline(lines, line)) {
    if (pred(line))
      result.push_back(line);
  }
  return result;
}

/**
 * @returns `read_lines_to_vector_if(path, true)`.
 *
 * @see read_lines_to_vector_if().
 */
DMITIGR_UTIL_API std::vector<std::string> read_lines_to_vector(const std::filesystem::path& path);

/**
 * @brief Reads an entire file into an instance of `std::string`.
 *
 * @returns The string with the content read from the specified `path`.
 */
DMITIGR_UTIL_API std::string read_to_string(const std::filesystem::path& path);

} // namespace dmitigr::fs

#ifdef DMITIGR_UTIL_HEADER_ONLY
#include "dmitigr/util/fs.cpp"
#endif

#endif  // DMITIGR_UTIL_FS_HPP
