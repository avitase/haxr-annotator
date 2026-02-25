#include <cstdint>
#include <cstring>
#include <new>

#include "core/core.hpp"

#include <H5Apublic.h>
#include <H5Fpublic.h>
#include <H5FDcore.h>
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
uint8_t* last_output = nullptr;
size_t last_output_size = 0;

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
int32_t haxr_tag_hdf5_image(const uint8_t* input_data,
                            size_t input_size)
{
    if (input_data == nullptr || input_size == 0)
    {
        return -10;
    }

    if (last_output != nullptr)
    {
        delete[] last_output;
        last_output = nullptr;
        last_output_size = 0;
    }

    const hid_t fapl_id = H5Pcreate(H5P_FILE_ACCESS);
    if (fapl_id < 0)
    {
        return -11;
    }

    if (H5Pset_fapl_core(fapl_id, static_cast<size_t>(64) * 1024, false) < 0)
    {
        H5Pclose(fapl_id);
        return -12;
    }

    if (H5Pset_file_image(fapl_id, const_cast<uint8_t*>(input_data), input_size) < 0)
    {
        H5Pclose(fapl_id);
        return -13;
    }

    const hid_t file_id = H5Fopen("mem.h5", H5F_ACC_RDWR, fapl_id);
    H5Pclose(fapl_id);
    if (file_id < 0)
    {
        return -14;
    }

    const int32_t status = write_root_attribute(file_id);
    if (status != 0)
    {
        H5Fclose(file_id);
        return status;
    }

    H5Fflush(file_id, H5F_SCOPE_GLOBAL);
    const hssize_t image_size = H5Fget_file_image(file_id, nullptr, 0);
    if (image_size <= 0)
    {
        H5Fclose(file_id);
        return -15;
    }

    auto* buffer = new (std::nothrow) uint8_t[static_cast<size_t>(image_size)];
    if (buffer == nullptr)
    {
        H5Fclose(file_id);
        return -16;
    }

    const hssize_t written = H5Fget_file_image(file_id, buffer, static_cast<size_t>(image_size));
    H5Fclose(file_id);
    if (written <= 0)
    {
        delete[] buffer;
        return -17;
    }

    last_output = buffer;
    last_output_size = static_cast<size_t>(written);
    return 0;
}

uint8_t* haxr_get_output_ptr()
{
    return last_output;
}

size_t haxr_get_output_size()
{
    return last_output_size;
}

void haxr_clear_output()
{
    delete[] last_output;
    last_output = nullptr;
    last_output_size = 0;
}
}  // extern "C"
