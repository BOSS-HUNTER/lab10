#pragma once

#include <string>
#include <random>
#include <utility>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/unordered_map.hpp>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>
#include <PicoSHA2/picosha2.h>

#include "Utils.h"

class DbActions
{
public:
    using FamilyContainer = std::list<std::unique_ptr<rocksdb::ColumnFamilyHandle>>;
    using FamilyDescriptorContainer = std::vector<rocksdb::ColumnFamilyDescriptor>;
    using FamilyHandlerContainer = std::list<std::unique_ptr<rocksdb::ColumnFamilyHandle>>;
    using RowContainer = boost::unordered_map<std::string, std::string>;

    explicit DbActions(std::string path);

    FamilyDescriptorContainer getFamilyDescriptorList();

    FamilyHandlerContainer open(const FamilyDescriptorContainer &descriptors);

    RowContainer getRows(rocksdb::ColumnFamilyHandle *family);

    void hashRows(rocksdb::ColumnFamilyHandle *family,
                  const RowContainer::const_iterator &begin,
                  const RowContainer::const_iterator &end);

    void create();

    void randomFill();

private:
    FamilyContainer randomFillFamilies();

    void randomFillRows(const FamilyContainer &container);

    std::string path_;
    std::unique_ptr<rocksdb::DB> db_;
};