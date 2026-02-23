#include <cstdint>
#include <cstring>
#include <filesystem>
#include <system_error>

#include "core/core.hpp"

#include <H5Apublic.h>
#include <H5Fpublic.h>
#include <H5Gpublic.h>
#include <H5Ipublic.h>
#include <H5Ppublic.h>
#include <H5Spublic.h>
#include <H5Tpublic.h>
#include <H5public.h>

namespace
{
constexpr const char* attr_name = "haxr_poc";
constexpr const char* g = "modified_by_core";

bool paths_equal(const char* a, const char* b)
{
    if (a == nullptr || b == nullptr)
    {
        return false;
    }

    return std::strcmp(a, b) == 0;
}

int32_t write_root_attribute(hid_t file_id)
{
    const hid_t root_id = H5Gopen2(file_id, "/", H5P_DEFAULT);
    if (root_id < 0)
    {
        return -3;
    }

    const hid_t space_id = H5Screate(H5S_SCALAR);
    if (space_id < 0)
    {
        H5Gclose(root_id);
        return -4;
    }

    const hid_t type_id = H5Tcopy(H5T_C_S1);
    if (type_id < 0)
    {
        H5Sclose(space_id);
        H5Gclose(root_id);
        return -5;
    }

    const size_t value_len = std::strlen(g);
    if (H5Tset_size(type_id, value_len) < 0)
    {
        H5Tclose(type_id);
        H5Sclose(space_id);
        H5Gclose(root_id);
        return -6;
    }

    hid_t attr_id = -1;
    if (H5Aexists(root_id, attr_name) > 0)
    {
        attr_id = H5Aopen(root_id, attr_name, H5P_DEFAULT);
    }
    else
    {
        attr_id = H5Acreate2(root_id, attr_name, type_id, space_id, H5P_DEFAULT, H5P_DEFAULT);
    }

    if (attr_id < 0)
    {
        H5Tclose(type_id);
        H5Sclose(space_id);
        H5Gclose(root_id);
        return -7;
    }

    const herr_t write_status = H5Awrite(attr_id, type_id, g);

    H5Aclose(attr_id);
    H5Tclose(type_id);
    H5Sclose(space_id);
    H5Gclose(root_id);

    return write_status < 0 ? -8 : 0;
}
}  // namespace

extern "C"
{
int32_t haxr_tag_hdf5(const char* input_path, const char* output_path)
{
    if (input_path == nullptr || output_path == nullptr)
    {
        return -1;
    }

    std::error_code copy_ec;
    if (!paths_equal(input_path, output_path))
    {
        std::filesystem::copy_file(input_path,
                                   output_path,
                                   std::filesystem::copy_options::overwrite_existing,
                                   copy_ec);
        if (copy_ec)
        {
            return -2;
        }
    }

    const hid_t file_id = H5Fopen(output_path, H5F_ACC_RDWR, H5P_DEFAULT);
    if (file_id < 0)
    {
        return -9;
    }

    const int32_t status = write_root_attribute(file_id);
    H5Fclose(file_id);

    return status;
}
}  // extern "C"
