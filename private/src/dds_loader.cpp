#include "base/prerequisites.h"
#include "core/core.h"

#include "image_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Arieo
{
    static const std::uint32_t g_dds_magic_number = 0x20534444; // DDS magic number
    struct DDS_PIXELFORMAT 
    {
        std::uint32_t dwSize;
        std::uint32_t dwFlags;
        std::uint32_t dwFourCC;
        std::uint32_t dwRGBBitCount;
        std::uint32_t dwRBitMask;
        std::uint32_t dwGBitMask;
        std::uint32_t dwBBitMask;
        std::uint32_t dwABitMask;
    };

    struct DDSHeader
    {
        std::uint32_t           dwSize;
        std::uint32_t           dwFlags;
        std::uint32_t           dwHeight;
        std::uint32_t           dwWidth;
        std::uint32_t           dwPitchOrLinearSize;
        std::uint32_t           dwDepth;
        std::uint32_t           dwMipMapCount;
        std::uint32_t           dwReserved1[11];
        DDS_PIXELFORMAT         ddspf;
        std::uint32_t           dwCaps;
        std::uint32_t           dwCaps2;
        std::uint32_t           dwCaps3;
        std::uint32_t           dwCaps4;
        std::uint32_t           dwReserved2;
    };

    enum class DXGI_FORMAT : std::uint32_t
    {
        DXGI_FORMAT_UNKNOWN = 0,
        DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
        DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
        DXGI_FORMAT_R32G32B32A32_UINT = 3,
        DXGI_FORMAT_R32G32B32A32_SINT = 4,
        DXGI_FORMAT_R32G32B32_TYPELESS = 5,
        DXGI_FORMAT_R32G32B32_FLOAT = 6,
        DXGI_FORMAT_R32G32B32_UINT = 7,
        DXGI_FORMAT_R32G32B32_SINT = 8,
        DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
        DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
        DXGI_FORMAT_R16G16B16A16_UNORM = 11,
        DXGI_FORMAT_R16G16B16A16_UINT = 12,
        DXGI_FORMAT_R16G16B16A16_SNORM = 13,
        DXGI_FORMAT_R16G16B16A16_SINT = 14,
        DXGI_FORMAT_R32G32_TYPELESS = 15,
        DXGI_FORMAT_R32G32_FLOAT = 16,
        DXGI_FORMAT_R32G32_UINT = 17,
        DXGI_FORMAT_R32G32_SINT = 18,
        DXGI_FORMAT_R32G8X24_TYPELESS = 19,
        DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
        DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
        DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
        DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
        DXGI_FORMAT_R10G10B10A2_UNORM = 24,
        DXGI_FORMAT_R10G10B10A2_UINT = 25,
        DXGI_FORMAT_R11G11B10_FLOAT = 26,
        DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
        DXGI_FORMAT_R8G8B8A8_UNORM = 28,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
        DXGI_FORMAT_R8G8B8A8_UINT = 30,
        DXGI_FORMAT_R8G8B8A8_SNORM = 31,
        DXGI_FORMAT_R8G8B8A8_SINT = 32,
        DXGI_FORMAT_R16G16_TYPELESS = 33,
        DXGI_FORMAT_R16G16_FLOAT = 34,
        DXGI_FORMAT_R16G16_UNORM = 35,
        DXGI_FORMAT_R16G16_UINT = 36,
        DXGI_FORMAT_R16G16_SNORM = 37,
        DXGI_FORMAT_R16G16_SINT = 38,
        DXGI_FORMAT_R32_TYPELESS = 39,
        DXGI_FORMAT_D32_FLOAT = 40,
        DXGI_FORMAT_R32_FLOAT = 41,
        DXGI_FORMAT_R32_UINT = 42,
        DXGI_FORMAT_R32_SINT = 43,
        DXGI_FORMAT_R24G8_TYPELESS = 44,
        DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
        DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
        DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
        DXGI_FORMAT_R8G8_TYPELESS = 48,
        DXGI_FORMAT_R8G8_UNORM = 49,
        DXGI_FORMAT_R8G8_UINT = 50,
        DXGI_FORMAT_R8G8_SNORM = 51,
        DXGI_FORMAT_R8G8_SINT = 52,
        DXGI_FORMAT_R16_TYPELESS = 53,
        DXGI_FORMAT_R16_FLOAT = 54,
        DXGI_FORMAT_D16_UNORM = 55,
        DXGI_FORMAT_R16_UNORM = 56,
        DXGI_FORMAT_R16_UINT = 57,
        DXGI_FORMAT_R16_SNORM = 58,
        DXGI_FORMAT_R16_SINT = 59,
        DXGI_FORMAT_R8_TYPELESS = 60,
        DXGI_FORMAT_R8_UNORM = 61,
        DXGI_FORMAT_R8_UINT = 62,
        DXGI_FORMAT_R8_SNORM = 63,
        DXGI_FORMAT_R8_SINT = 64,
        DXGI_FORMAT_A8_UNORM = 65,
        DXGI_FORMAT_R1_UNORM = 66,
        DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
        DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
        DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
        DXGI_FORMAT_BC1_TYPELESS = 70,
        DXGI_FORMAT_BC1_UNORM = 71,
        DXGI_FORMAT_BC1_UNORM_SRGB = 72,
        DXGI_FORMAT_BC2_TYPELESS = 73,
        DXGI_FORMAT_BC2_UNORM = 74,
        DXGI_FORMAT_BC2_UNORM_SRGB = 75,
        DXGI_FORMAT_BC3_TYPELESS = 76,
        DXGI_FORMAT_BC3_UNORM = 77,
        DXGI_FORMAT_BC3_UNORM_SRGB = 78,
        DXGI_FORMAT_BC4_TYPELESS = 79,
        DXGI_FORMAT_BC4_UNORM = 80,
        DXGI_FORMAT_BC4_SNORM = 81,
        DXGI_FORMAT_BC5_TYPELESS = 82,
        DXGI_FORMAT_BC5_UNORM = 83,
        DXGI_FORMAT_BC5_SNORM = 84,
        DXGI_FORMAT_B5G6R5_UNORM = 85,
        DXGI_FORMAT_B5G5R5A1_UNORM = 86,
        DXGI_FORMAT_B8G8R8A8_UNORM = 87,
        DXGI_FORMAT_B8G8R8X8_UNORM = 88,
        DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
        DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
        DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
        DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
        DXGI_FORMAT_BC6H_TYPELESS = 94,
        DXGI_FORMAT_BC6H_UF16 = 95,
        DXGI_FORMAT_BC6H_SF16 = 96,
        DXGI_FORMAT_BC7_TYPELESS = 97,
        DXGI_FORMAT_BC7_UNORM = 98,
        DXGI_FORMAT_BC7_UNORM_SRGB = 99,
        DXGI_FORMAT_AYUV = 100,
        DXGI_FORMAT_Y410 = 101,
        DXGI_FORMAT_Y416 = 102,
        DXGI_FORMAT_NV12 = 103,
        DXGI_FORMAT_P010 = 104,
        DXGI_FORMAT_P016 = 105,
        DXGI_FORMAT_420_OPAQUE = 106,
        DXGI_FORMAT_YUY2 = 107,
        DXGI_FORMAT_Y210 = 108,
        DXGI_FORMAT_Y216 = 109,
        DXGI_FORMAT_NV11 = 110,
        DXGI_FORMAT_AI44 = 111,
        DXGI_FORMAT_IA44 = 112,
        DXGI_FORMAT_P8 = 113,
        DXGI_FORMAT_A8P8 = 114,
        DXGI_FORMAT_B4G4R4A4_UNORM = 115,
        DXGI_FORMAT_P208 = 130,
        DXGI_FORMAT_V208 = 131,
        DXGI_FORMAT_V408 = 132,
        DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
        DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
        DXGI_FORMAT_FORCE_UINT = 0xffffffff
    };

    enum class D3D10_RESOURCE_DIMENSION : std::uint32_t
    {
        D3D10_RESOURCE_DIMENSION_UNKNOWN = 0,
        D3D10_RESOURCE_DIMENSION_BUFFER = 1,
        D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2,
        D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3,
        D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4
    } ;

    struct DDS_HEADER_DXT10
    {
        DXGI_FORMAT              dxgiFormat;
        D3D10_RESOURCE_DIMENSION resourceDimension;
        std::uint32_t            miscFlag;
        std::uint32_t            arraySize;
        std::uint32_t            miscFlags2;
    } ;

    ARIEO_ENUM_MAPPER(DXGI_FORMAT, Interface::RHI::Format)
    {
        { DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, Arieo::Interface::RHI::Format::UNKNOWN },
        { DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_TYPELESS, Arieo::Interface::RHI::Format::R32G32B32A32_UINT },
        { DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, Arieo::Interface::RHI::Format::R32G32B32A32_SFLOAT },
        { DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT, Arieo::Interface::RHI::Format::R32G32B32A32_UINT },
        { DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT, Arieo::Interface::RHI::Format::R32G32B32A32_SINT },
        { DXGI_FORMAT::DXGI_FORMAT_R32G32B32_TYPELESS, Arieo::Interface::RHI::Format::R32G32B32_UINT },
        { DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, Arieo::Interface::RHI::Format::R32G32B32_SFLOAT },
        { DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT, Arieo::Interface::RHI::Format::R32G32B32_UINT },
        { DXGI_FORMAT::DXGI_FORMAT_R32G32B32_SINT, Arieo::Interface::RHI::Format::R32G32B32_SINT },
        { DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_TYPELESS, Arieo::Interface::RHI::Format::R16G16B16A16_UINT },
        { DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT , Arieo::Interface::RHI::Format::R16G16B16A16_SFLOAT  },
        { DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UNORM , Arieo::Interface::RHI::Format::R16G16B16A16_UNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UINT , Arieo::Interface::RHI::Format::R16G16B16A16_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_SNORM , Arieo::Interface::RHI::Format::R16G16B16A16_SNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_SINT , Arieo::Interface::RHI::Format::R16G16B16A16_SINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R32G32_TYPELESS , Arieo::Interface::RHI::Format::R32G32_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT , Arieo::Interface::RHI::Format::R32G32_SFLOAT  },
        { DXGI_FORMAT::DXGI_FORMAT_R32G32_UINT , Arieo::Interface::RHI::Format::R32G32_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R32G32_SINT , Arieo::Interface::RHI::Format::R32G32_SINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R32G8X24_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_X32_TYPELESS_G8X24_UINT , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_R10G10B10A2_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_R10G10B10A2_UNORM , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_R10G10B10A2_UINT , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_R11G11B10_FLOAT , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_TYPELESS , Arieo::Interface::RHI::Format::R8G8B8A8_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM , Arieo::Interface::RHI::Format::R8G8B8A8_UNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB , Arieo::Interface::RHI::Format::R8G8B8A8_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT , Arieo::Interface::RHI::Format::R8G8B8A8_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SNORM , Arieo::Interface::RHI::Format::R8G8B8A8_SNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SINT , Arieo::Interface::RHI::Format::R8G8B8A8_SINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R16G16_TYPELESS , Arieo::Interface::RHI::Format::R16G16_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R16G16_FLOAT , Arieo::Interface::RHI::Format::R16G16_SFLOAT  },
        { DXGI_FORMAT::DXGI_FORMAT_R16G16_UNORM , Arieo::Interface::RHI::Format::R16G16_UNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R16G16_UINT , Arieo::Interface::RHI::Format::R16G16_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R16G16_SNORM , Arieo::Interface::RHI::Format::R16G16_SNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R16G16_SINT , Arieo::Interface::RHI::Format::R16G16_SINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS , Arieo::Interface::RHI::Format::R32_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT , Arieo::Interface::RHI::Format::D32_SFLOAT  },
        { DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT , Arieo::Interface::RHI::Format::R32_SFLOAT  },
        { DXGI_FORMAT::DXGI_FORMAT_R32_UINT , Arieo::Interface::RHI::Format::R32_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R32_SINT , Arieo::Interface::RHI::Format::R32_SINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT , Arieo::Interface::RHI::Format::D24_UNORM_S8_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_X24_TYPELESS_G8_UINT , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_R8G8_TYPELESS , Arieo::Interface::RHI::Format::R8G8_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R8G8_UNORM , Arieo::Interface::RHI::Format::R8G8_UNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R8G8_UINT , Arieo::Interface::RHI::Format::R8G8_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R8G8_SNORM , Arieo::Interface::RHI::Format::R8G8_SNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R8G8_SINT , Arieo::Interface::RHI::Format::R8G8_SINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R16_TYPELESS , Arieo::Interface::RHI::Format::R16_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT , Arieo::Interface::RHI::Format::R16_SFLOAT  },
        { DXGI_FORMAT::DXGI_FORMAT_D16_UNORM , Arieo::Interface::RHI::Format::D16_UNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R16_UNORM , Arieo::Interface::RHI::Format::R16_UNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R16_UINT , Arieo::Interface::RHI::Format::R16_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R16_SNORM , Arieo::Interface::RHI::Format::R16_SNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R16_SINT , Arieo::Interface::RHI::Format::R16_SINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R8_TYPELESS , Arieo::Interface::RHI::Format::R8_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R8_UNORM , Arieo::Interface::RHI::Format::R8_UNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R8_UINT , Arieo::Interface::RHI::Format::R8_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_R8_SNORM , Arieo::Interface::RHI::Format::R8_SNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_R8_SINT , Arieo::Interface::RHI::Format::R8_SINT  },
        { DXGI_FORMAT::DXGI_FORMAT_A8_UNORM , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_R1_UNORM , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_R9G9B9E5_SHAREDEXP , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_R8G8_B8G8_UNORM , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_G8R8_G8B8_UNORM , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_BC1_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  }, //TODO
        { DXGI_FORMAT::DXGI_FORMAT_BC1_UNORM , Arieo::Interface::RHI::Format::BC1_RGB_UNORM_BLOCK  }, //TODO
        { DXGI_FORMAT::DXGI_FORMAT_BC1_UNORM_SRGB , Arieo::Interface::RHI::Format::BC1_RGB_SRGB_BLOCK  }, //TODO
        { DXGI_FORMAT::DXGI_FORMAT_BC2_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  }, 
        { DXGI_FORMAT::DXGI_FORMAT_BC2_UNORM , Arieo::Interface::RHI::Format::BC2_UNORM_BLOCK  }, 
        { DXGI_FORMAT::DXGI_FORMAT_BC2_UNORM_SRGB , Arieo::Interface::RHI::Format::BC2_SRGB_BLOCK  }, 
        { DXGI_FORMAT::DXGI_FORMAT_BC3_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  }, 
        { DXGI_FORMAT::DXGI_FORMAT_BC3_UNORM , Arieo::Interface::RHI::Format::BC3_UNORM_BLOCK  }, 
        { DXGI_FORMAT::DXGI_FORMAT_BC3_UNORM_SRGB , Arieo::Interface::RHI::Format::BC3_SRGB_BLOCK  }, 
        { DXGI_FORMAT::DXGI_FORMAT_BC4_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  }, 
        { DXGI_FORMAT::DXGI_FORMAT_BC4_UNORM , Arieo::Interface::RHI::Format::BC4_UNORM_BLOCK  }, 
        { DXGI_FORMAT::DXGI_FORMAT_BC4_SNORM , Arieo::Interface::RHI::Format::BC4_SNORM_BLOCK  }, 
        { DXGI_FORMAT::DXGI_FORMAT_BC5_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  }, 
        { DXGI_FORMAT::DXGI_FORMAT_BC5_UNORM , Arieo::Interface::RHI::Format::BC5_UNORM_BLOCK  }, 
        { DXGI_FORMAT::DXGI_FORMAT_BC5_SNORM , Arieo::Interface::RHI::Format::BC5_SNORM_BLOCK  }, 
        { DXGI_FORMAT::DXGI_FORMAT_B5G6R5_UNORM , Arieo::Interface::RHI::Format::B5G6R5_UNORM_PACK16  },
        { DXGI_FORMAT::DXGI_FORMAT_B5G5R5A1_UNORM , Arieo::Interface::RHI::Format::B5G5R5A1_UNORM_PACK16  },
        { DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM , Arieo::Interface::RHI::Format::B8G8R8A8_UNORM  },
        { DXGI_FORMAT::DXGI_FORMAT_B8G8R8X8_UNORM , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_TYPELESS , Arieo::Interface::RHI::Format::B8G8R8A8_UINT  },
        { DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM_SRGB , Arieo::Interface::RHI::Format::B8G8R8A8_SRGB  },
        { DXGI_FORMAT::DXGI_FORMAT_B8G8R8X8_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_B8G8R8X8_UNORM_SRGB , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_BC6H_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_BC6H_UF16 , Arieo::Interface::RHI::Format::BC6H_UFLOAT_BLOCK  },
        { DXGI_FORMAT::DXGI_FORMAT_BC6H_SF16 , Arieo::Interface::RHI::Format::BC6H_SFLOAT_BLOCK  },
        { DXGI_FORMAT::DXGI_FORMAT_BC7_TYPELESS , Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_BC7_UNORM , Arieo::Interface::RHI::Format::BC7_UNORM_BLOCK  },
        { DXGI_FORMAT::DXGI_FORMAT_BC7_UNORM_SRGB , Arieo::Interface::RHI::Format::BC7_SRGB_BLOCK  },
        { DXGI_FORMAT::DXGI_FORMAT_AYUV, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_Y410, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_Y416, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_NV12, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_P010, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_P016, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_420_OPAQUE, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_YUY2, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_Y210, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_Y216, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_NV11, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_AI44, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_IA44, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_P8, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_A8P8, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_B4G4R4A4_UNORM, Arieo::Interface::RHI::Format::B4G4R4A4_UNORM_PACK16  },
        { DXGI_FORMAT::DXGI_FORMAT_P208, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_V208, Arieo::Interface::RHI::Format::UNKNOWN  },
        { DXGI_FORMAT::DXGI_FORMAT_V408, Arieo::Interface::RHI::Format::UNKNOWN  },
        // DXGI_FORMAT_FORCE_UINT = 0xffffffff
    };

    Interface::FileLoader::ImageBuffer ImageLoader::loadDDS(void* buffer, size_t size)
    {
        int width = 0;
        int height = 0; 
        int channels = 0;

        std::uint32_t* magic_number = (std::uint32_t*)buffer;
        if(*magic_number != g_dds_magic_number)
        {
            Core::Logger::trace("dds loaded failed: magic number wrong");
        }

        DDSHeader* dds_header = (DDSHeader*)((std::byte*)buffer + sizeof(g_dds_magic_number));
        DDS_HEADER_DXT10* dds_header_dxt10 = nullptr;
        void* texture_buffer = nullptr;
        size_t texture_buffer_size = 0;
        Interface::RHI::Format texture_buffer_format = Interface::RHI::Format::UNKNOWN;
        if((dds_header->ddspf.dwFlags & 0x000000004) != 0 //DDPF_FORCC
            && dds_header->ddspf.dwFourCC == '01XD')  // "DX10" as a FourCC
        {
            dds_header_dxt10 = (DDS_HEADER_DXT10*)((std::byte*)buffer + sizeof(g_dds_magic_number) + dds_header->dwSize);
            texture_buffer = ((std::byte*)buffer + sizeof(g_dds_magic_number) + dds_header->dwSize + sizeof(DDS_HEADER_DXT10));
            texture_buffer_size = size - dds_header->dwSize - sizeof(g_dds_magic_number) - sizeof(DDS_HEADER_DXT10);
            texture_buffer_format = Base::mapEnum<Interface::RHI::Format>(dds_header_dxt10->dxgiFormat);
        }
        else
        {
            texture_buffer = ((std::byte*)buffer + sizeof(g_dds_magic_number) + dds_header->dwSize);
            texture_buffer_size = size - dds_header->dwSize - sizeof(g_dds_magic_number);

            if(dds_header->ddspf.dwRBitMask == 0x000000FF)
            {
                texture_buffer_format = Interface::RHI::Format::R8G8B8A8_UNORM;
            }
            else
            {
                texture_buffer_format = Interface::RHI::Format::B8G8R8A8_UNORM;
            }
            
        }

        Interface::FileLoader::ImageBuffer image_buffer{};
        {
            image_buffer.m_buffer = texture_buffer;
            image_buffer.m_size = texture_buffer_size;

            image_buffer.m_format = texture_buffer_format;

            image_buffer.m_width = dds_header->dwWidth;
            image_buffer.m_height = dds_header->dwHeight;
            image_buffer.m_depth = dds_header->dwDepth;
            image_buffer.m_mip_map_count = dds_header->dwMipMapCount;
        }

        return image_buffer;
    }
}