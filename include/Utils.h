#pragma once

#include <boost/filesystem.hpp>
#include <random>

int programArguments(int argc, char *argv[]);

std::string createRandomString(size_t length);

void copyDirectory(const boost::filesystem::path &src, const boost::filesystem::path &dst);

void removeDirectoryIfExists(const boost::filesystem::path &path);