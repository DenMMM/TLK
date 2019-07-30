/*
---------------------------------------------------------------------------
Copyright (c) 1998-2010, Brian Gladman, Worcester, UK. All rights reserved.

The redistribution and use of this software (with or without changes)
is allowed without the payment of fees or royalties provided that:

  source code distributions include the above copyright notice, this
  list of conditions and the following disclaimer;

  binary distributions include the above copyright notice, this list
  of conditions and the following disclaimer in their documentation.

This software is provided 'as is' with no explicit or implied warranties
in respect of its operation, including, but not limited to, correctness
and fitness for purpose.
---------------------------------------------------------------------------
Issue Date: 20/12/2007
*/

#include "hmac.h"

#ifdef SHA_1

#define SHA1_TESTS 11

struct
{   unsigned int  txt_len;
    unsigned int  key_len;
    unsigned int  mac_len;
    unsigned char *text;
    unsigned char key[128];
    unsigned char mac[SHA1_DIGEST_SIZE];
} t_sha1[SHA1_TESTS] =
{
    {   9, 64, 20, "Sample #1",
        {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
            0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
            0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
        },
        {   0x4f, 0x4c, 0xa3, 0xd5, 0xd6, 0x8b, 0xa7, 0xcc,
            0x0a, 0x12, 0x08, 0xc9, 0xc6, 0x1e, 0x9c, 0x5d,
            0xa0, 0x40, 0x3c, 0x0a
        }
    },
    {   9, 20, 20, "Sample #2",
        {
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
            0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
            0x40, 0x41, 0x42, 0x43
        },
        {   0x09, 0x22, 0xd3, 0x40, 0x5f, 0xaa, 0x3d, 0x19,
            0x4f, 0x82, 0xa4, 0x58, 0x30, 0x73, 0x7d, 0x5c,
            0xc6, 0xc7, 0x5d, 0x24
        }
    },
    {   9, 100, 20, "Sample #3",
        {
            0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
            0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
            0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
            0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
            0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
            0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
            0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
            0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
            0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
            0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
            0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
            0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
            0xb0, 0xb1, 0xb2, 0xb3
        },
        {   0xbc, 0xf4, 0x1e, 0xab, 0x8b, 0xb2, 0xd8, 0x02,
            0xf3, 0xd0, 0x5c, 0xaf, 0x7c, 0xb0, 0x92, 0xec,
            0xf8, 0xd1, 0xa3, 0xaa
        }
    },
    {   9, 49, 12, "Sample #4",
        {
            0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
            0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
            0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
            0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
            0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
            0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
            0xa0
        },
        {   0x9e, 0xa8, 0x86, 0xef, 0xe2, 0x68, 0xdb, 0xec,
            0xce, 0x42, 0x0c, 0x75
        }
    },
    {   8, 20, 20, "Hi There",
        {
            0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
            0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
            0x0b, 0x0b, 0x0b, 0x0b
        },
        {   0xb6, 0x17, 0x31, 0x86, 0x55, 0x05, 0x72, 0x64,
            0xe2, 0x8b, 0xc0, 0xb6, 0xfb, 0x37, 0x8c, 0x8e,
            0xf1, 0x46, 0xbe, 0x00
        }
    },
    {   28, 4, 20, "what do ya want for nothing?",
        {
            0x4a, 0x65, 0x66, 0x65
        },
        {   0xef, 0xfc, 0xdf, 0x6a, 0xe5, 0xeb, 0x2f, 0xa2,
            0xd2, 0x74, 0x16, 0xd5, 0xf1, 0x84, 0xdf, 0x9c,
            0x25, 0x9a, 0x7c, 0x79
        }
    },
    {   50, 20, 20, "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                    "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                    "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                    "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                    "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd",
        {
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa
        },
        {   0x12, 0x5d, 0x73, 0x42, 0xb9, 0xac, 0x11, 0xcd,
            0x91, 0xa3, 0x9a, 0xf4, 0x8a, 0xa1, 0x7b, 0x4f,
            0x63, 0xf1, 0x75, 0xd3
        }
    },
    {   50, 25, 20, "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                    "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                    "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                    "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                    "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd",
        {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
            0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
            0x19
        },
        {   0x4c, 0x90, 0x07, 0xf4, 0x02, 0x62, 0x50, 0xc6,
            0xbc, 0x84, 0x14, 0xf9, 0xbf, 0x50, 0xc8, 0x6c,
            0x2d, 0x72, 0x35, 0xda
        }
    },
    {   20, 20, 20, "Test With Truncation",
        {
            0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
            0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
            0x0c, 0x0c, 0x0c, 0x0c
        },
        {   0x4c, 0x1a, 0x03, 0x42, 0x4b, 0x55, 0xe0, 0x7f,
            0xe7, 0xf2, 0x7b, 0xe1, 0xd5, 0x8b, 0xb9, 0x32,
            0x4a, 0x9a, 0x5a, 0x04
        }
    },
    {   54, 80, 20, "Test Using Larger Than Block-Size Key - Hash Key First",
        {
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
        },
        {   0xaa, 0x4a, 0xe5, 0xe1, 0x52, 0x72, 0xd0, 0x0e,
            0x95, 0x70, 0x56, 0x37, 0xce, 0x8a, 0x3b, 0x55,
            0xed, 0x40, 0x21, 0x12
        }
    },
    {   73, 80, 20, "Test Using Larger Than Block-Size Key and "
                    "Larger Than One Block-Size Data",
        {
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
        },
        {   0xe8, 0xe9, 0x9d, 0x0f, 0x45, 0x23, 0x7d, 0x78, 
            0x6d, 0x6b, 0xba, 0xa7, 0x96, 0x5c, 0x78, 0x08,
            0xbb, 0xff, 0x1a, 0x91 
        }
    }
};

#endif

#ifdef SHA_256

/*  HMAC-SHA256 test vectors from draft-ietf-ipsec-ciph-sha-256-01.txt  */

#define SHA256_TESTS 10

struct
{   unsigned int  txt_len;
    unsigned int  key_len;
    unsigned int  mac_len;
    unsigned char text[128];
    unsigned char key[128];
    unsigned char mac[SHA256_DIGEST_SIZE];
} t_sha256[SHA256_TESTS] =
{
    {   3, 32, 32, "abc",
        {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
            0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
            0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
        },
        {   0xa2, 0x1b, 0x1f, 0x5d, 0x4c, 0xf4, 0xf7, 0x3a,
            0x4d, 0xd9, 0x39, 0x75, 0x0f, 0x7a, 0x06, 0x6a,
            0x7f, 0x98, 0xcc, 0x13, 0x1c, 0xb1, 0x6a, 0x66,
            0x92, 0x75, 0x90, 0x21, 0xcf, 0xab, 0x81, 0x81
        },
    },
    {   56, 32, 32, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
            0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
            0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
        },
        {   0x10, 0x4f, 0xdc, 0x12, 0x57, 0x32, 0x8f, 0x08,
            0x18, 0x4b, 0xa7, 0x31, 0x31, 0xc5, 0x3c, 0xae,
            0xe6, 0x98, 0xe3, 0x61, 0x19, 0x42, 0x11, 0x49,
            0xea, 0x8c, 0x71, 0x24, 0x56, 0x69, 0x7d, 0x30
        }
    },
    {   112, 32, 32, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
                     "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
            0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
            0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
        },
        {   0x47, 0x03, 0x05, 0xfc, 0x7e, 0x40, 0xfe, 0x34,
            0xd3, 0xee, 0xb3, 0xe7, 0x73, 0xd9, 0x5a, 0xab,
            0x73, 0xac, 0xf0, 0xfd, 0x06, 0x04, 0x47, 0xa5,
            0xeb, 0x45, 0x95, 0xbf, 0x33, 0xa9, 0xd1, 0xa3
        }
    },
    {   8, 32, 32, "Hi There",
        {
            0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
            0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
            0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
            0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
        },
        { 0x19, 0x8a, 0x60, 0x7e, 0xb4, 0x4b, 0xfb, 0xc6,
          0x99, 0x03, 0xa0, 0xf1, 0xcf, 0x2b, 0xbd, 0xc5,
          0xba, 0x0a, 0xa3, 0xf3, 0xd9, 0xae, 0x3c, 0x1c,
          0x7a, 0x3b, 0x16, 0x96, 0xa0, 0xb6, 0x8c, 0xf7
        }
    },
    {   28, 4, 32, "what do ya want for nothing?",
        {
            "Jefe"
        },
        {   0x5b, 0xdc, 0xc1, 0x46, 0xbf, 0x60, 0x75, 0x4e,
            0x6a, 0x04, 0x24, 0x26, 0x08, 0x95, 0x75, 0xc7,
            0x5a, 0x00, 0x3f, 0x08, 0x9d, 0x27, 0x39, 0x83,
            0x9d, 0xec, 0x58, 0xb9, 0x64, 0xec, 0x38, 0x43
        }
    },
    {   50, 32, 32, "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                    "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                    "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                    "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                    "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd",
        {
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
        },
        {   0xcd, 0xcb, 0x12, 0x20, 0xd1, 0xec, 0xcc, 0xea,
            0x91, 0xe5, 0x3a, 0xba, 0x30, 0x92, 0xf9, 0x62,
            0xe5, 0x49, 0xfe, 0x6c, 0xe9, 0xed, 0x7f, 0xdc,
            0x43, 0x19, 0x1f, 0xbd, 0xe4, 0x5c, 0x30, 0xb0
        }
    },
    {   50, 37, 32, "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                    "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                    "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                    "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                    "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd",
        {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
            0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
            0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
            0x21, 0x22, 0x23, 0x24, 0x25, 0x00
        },
        {   0xd4, 0x63, 0x3c, 0x17, 0xf6, 0xfb, 0x8d, 0x74,
            0x4c, 0x66, 0xde, 0xe0, 0xf8, 0xf0, 0x74, 0x55,
            0x6e, 0xc4, 0xaf, 0x55, 0xef, 0x07, 0x99, 0x85,
            0x41, 0x46, 0x8e, 0xb4, 0x9b, 0xd2, 0xe9, 0x17
        }
    },
    {   20, 32, 32, "Test With Truncation",
        {
            0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
            0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
            0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
            0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c
        },
        {   0x75, 0x46, 0xaf, 0x01, 0x84, 0x1f, 0xc0, 0x9b,
            0x1a, 0xb9, 0xc3, 0x74, 0x9a, 0x5f, 0x1c, 0x17,
            0xd4, 0xf5, 0x89, 0x66, 0x8a, 0x58, 0x7b, 0x27,
            0x00, 0xa9, 0xc9, 0x7c, 0x11, 0x93, 0xcf, 0x42
        }
    },
    {   54, 80, 32, "Test Using Larger Than Block-Size Key - Hash Key First",
        {
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
        },
        {   0x69, 0x53, 0x02, 0x5e, 0xd9, 0x6f, 0x0c, 0x09,
            0xf8, 0x0a, 0x96, 0xf7, 0x8e, 0x65, 0x38, 0xdb,
            0xe2, 0xe7, 0xb8, 0x20, 0xe3, 0xdd, 0x97, 0x0e,
            0x7d, 0xdd, 0x39, 0x09, 0x1b, 0x32, 0x35, 0x2f
        }
    },
    {   73, 80, 32, "Test Using Larger Than Block-Size Key and Larger Than "
                    "One Block-Size Data",
        {
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
        },
        {   0x63, 0x55, 0xac, 0x22, 0xe8, 0x90, 0xd0, 0xa3,
            0xc8, 0x48, 0x1a, 0x5c, 0xa4, 0x82, 0x5b, 0xc8,
            0x84, 0xd3, 0xe7, 0xa1, 0xff, 0x98, 0xa2, 0xfc,
            0x2a, 0xc7, 0xd8, 0xe0, 0x64, 0xc3, 0xb2, 0xe6
        }
    },
 };

#endif

#define SHA2_TESTS   7

 struct
{   unsigned int key_len;
    unsigned int txt_len;
    unsigned int mac_len[4];
    unsigned char key[140];
    unsigned char text[160];
    unsigned char r224[28];
    unsigned char r256[32];
    unsigned char r384[48];
    unsigned char r512[64];
} t_s2[SHA2_TESTS] =
{
    {   20, 8, { 28, 32, 48, 64 },
    { 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
      0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
      0x0b, 0x0b, 0x0b, 0x0b },
    { 0x48, 0x69, 0x20, 0x54, 0x68, 0x65, 0x72, 0x65 },
    { 0x89, 0x6f, 0xb1, 0x12, 0x8a, 0xbb, 0xdf, 0x19,
      0x68, 0x32, 0x10, 0x7c, 0xd4, 0x9d, 0xf3, 0x3f,
      0x47, 0xb4, 0xb1, 0x16, 0x99, 0x12, 0xba, 0x4f,
      0x53, 0x68, 0x4b, 0x22 },
    { 0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53,
      0x5c, 0xa8, 0xaf, 0xce, 0xaf, 0x0b, 0xf1, 0x2b,
      0x88, 0x1d, 0xc2, 0x00, 0xc9, 0x83, 0x3d, 0xa7,
      0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32, 0xcf, 0xf7 },
    { 0xaf, 0xd0, 0x39, 0x44, 0xd8, 0x48, 0x95, 0x62,
      0x6b, 0x08, 0x25, 0xf4, 0xab, 0x46, 0x90, 0x7f,
      0x15, 0xf9, 0xda, 0xdb, 0xe4, 0x10, 0x1e, 0xc6,
      0x82, 0xaa, 0x03, 0x4c, 0x7c, 0xeb, 0xc5, 0x9c,
      0xfa, 0xea, 0x9e, 0xa9, 0x07, 0x6e, 0xde, 0x7f,
      0x4a, 0xf1, 0x52, 0xe8, 0xb2, 0xfa, 0x9c, 0xb6 },
    { 0x87, 0xaa, 0x7c, 0xde, 0xa5, 0xef, 0x61, 0x9d,
      0x4f, 0xf0, 0xb4, 0x24, 0x1a, 0x1d, 0x6c, 0xb0,
      0x23, 0x79, 0xf4, 0xe2, 0xce, 0x4e, 0xc2, 0x78,
      0x7a, 0xd0, 0xb3, 0x05, 0x45, 0xe1, 0x7c, 0xde,
      0xda, 0xa8, 0x33, 0xb7, 0xd6, 0xb8, 0xa7, 0x02,
      0x03, 0x8b, 0x27, 0x4e, 0xae, 0xa3, 0xf4, 0xe4,
      0xbe, 0x9d, 0x91, 0x4e, 0xeb, 0x61, 0xf1, 0x70,
      0x2e, 0x69, 0x6c, 0x20, 0x3a, 0x12, 0x68, 0x54}
  },
    {   4, 28, { 28, 32, 48, 64 },
    { 0x4a, 0x65, 0x66, 0x65 },
    { 0x77, 0x68, 0x61, 0x74, 0x20, 0x64, 0x6f, 0x20,
      0x79, 0x61, 0x20, 0x77, 0x61, 0x6e, 0x74, 0x20,
      0x66, 0x6f, 0x72, 0x20, 0x6e, 0x6f, 0x74, 0x68,
      0x69, 0x6e, 0x67, 0x3f },
    { 0xa3, 0x0e, 0x01, 0x09, 0x8b, 0xc6, 0xdb, 0xbf,
      0x45, 0x69, 0x0f, 0x3a, 0x7e, 0x9e, 0x6d, 0x0f,
      0x8b, 0xbe, 0xa2, 0xa3, 0x9e, 0x61, 0x48, 0x00,
      0x8f, 0xd0, 0x5e, 0x44 },
    { 0x5b, 0xdc, 0xc1, 0x46, 0xbf, 0x60, 0x75, 0x4e,
      0x6a, 0x04, 0x24, 0x26, 0x08, 0x95, 0x75, 0xc7,
      0x5a, 0x00, 0x3f, 0x08, 0x9d, 0x27, 0x39, 0x83,
      0x9d, 0xec, 0x58, 0xb9, 0x64, 0xec, 0x38, 0x43 },
    { 0xaf, 0x45, 0xd2, 0xe3, 0x76, 0x48, 0x40, 0x31,
      0x61, 0x7f, 0x78, 0xd2, 0xb5, 0x8a, 0x6b, 0x1b,
      0x9c, 0x7e, 0xf4, 0x64, 0xf5, 0xa0, 0x1b, 0x47,
      0xe4, 0x2e, 0xc3, 0x73, 0x63, 0x22, 0x44, 0x5e,
      0x8e, 0x22, 0x40, 0xca, 0x5e, 0x69, 0xe2, 0xc7,
      0x8b, 0x32, 0x39, 0xec, 0xfa, 0xb2, 0x16, 0x49 },
    { 0x16, 0x4b, 0x7a, 0x7b, 0xfc, 0xf8, 0x19, 0xe2,
      0xe3, 0x95, 0xfb, 0xe7, 0x3b, 0x56, 0xe0, 0xa3,
      0x87, 0xbd, 0x64, 0x22, 0x2e, 0x83, 0x1f, 0xd6,
      0x10, 0x27, 0x0c, 0xd7, 0xea, 0x25, 0x05, 0x54,
      0x97, 0x58, 0xbf, 0x75, 0xc0, 0x5a, 0x99, 0x4a,
      0x6d, 0x03, 0x4f, 0x65, 0xf8, 0xf0, 0xe6, 0xfd,
      0xca, 0xea, 0xb1, 0xa3, 0x4d, 0x4a, 0x6b, 0x4b,
      0x63, 0x6e, 0x07, 0x0a, 0x38, 0xbc, 0xe7, 0x37 }
  },
  {   20, 50, { 28, 32, 48, 64 },
    { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa },
    { 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
      0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
      0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
      0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
      0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
      0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
      0xdd, 0xdd },
    { 0x7f, 0xb3, 0xcb, 0x35, 0x88, 0xc6, 0xc1, 0xf6,
      0xff, 0xa9, 0x69, 0x4d, 0x7d, 0x6a, 0xd2, 0x64,
      0x93, 0x65, 0xb0, 0xc1, 0xf6, 0x5d, 0x69, 0xd1,
      0xec, 0x83, 0x33, 0xea },
    { 0x77, 0x3e, 0xa9, 0x1e, 0x36, 0x80, 0x0e, 0x46,
      0x85, 0x4d, 0xb8, 0xeb, 0xd0, 0x91, 0x81, 0xa7,
      0x29, 0x59, 0x09, 0x8b, 0x3e, 0xf8, 0xc1, 0x22,
      0xd9, 0x63, 0x55, 0x14, 0xce, 0xd5, 0x65, 0xfe },
    { 0x88, 0x06, 0x26, 0x08, 0xd3, 0xe6, 0xad, 0x8a,
    0x0a, 0xa2, 0xac, 0xe0, 0x14, 0xc8, 0xa8, 0x6f,
      0x0a, 0xa6, 0x35, 0xd9, 0x47, 0xac, 0x9f, 0xeb,
      0xe8, 0x3e, 0xf4, 0xe5, 0x59, 0x66, 0x14, 0x4b,
      0x2a, 0x5a, 0xb3, 0x9d, 0xc1, 0x38, 0x14, 0xb9,
      0x4e, 0x3a, 0xb6, 0xe1, 0x01, 0xa3, 0x4f, 0x27 },
    { 0xfa, 0x73, 0xb0, 0x08, 0x9d, 0x56, 0xa2, 0x84,
      0xef, 0xb0, 0xf0, 0x75, 0x6c, 0x89, 0x0b, 0xe9,
      0xb1, 0xb5, 0xdb, 0xdd, 0x8e, 0xe8, 0x1a, 0x36,
      0x55, 0xf8, 0x3e, 0x33, 0xb2, 0x27, 0x9d, 0x39,
      0xbf, 0x3e, 0x84, 0x82, 0x79, 0xa7, 0x22, 0xc8,
      0x06, 0xb4, 0x85, 0xa4, 0x7e, 0x67, 0xc8, 0x07,
      0xb9, 0x46, 0xa3, 0x37, 0xbe, 0xe8, 0x94, 0x26,
      0x74, 0x27, 0x88, 0x59, 0xe1, 0x32, 0x92, 0xfb }
  },
  {   25, 50, { 28, 32, 48, 64 },
    { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
      0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
      0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
      0x19 },
    { 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
      0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
      0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
      0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
      0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
      0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
      0xcd, 0xcd },
    { 0x6c, 0x11, 0x50, 0x68, 0x74, 0x01, 0x3c, 0xac,
      0x6a, 0x2a, 0xbc, 0x1b, 0xb3, 0x82, 0x62, 0x7c,
      0xec, 0x6a, 0x90, 0xd8, 0x6e, 0xfc, 0x01, 0x2d,
      0xe7, 0xaf, 0xec, 0x5a },
    { 0x82, 0x55, 0x8a, 0x38, 0x9a, 0x44, 0x3c, 0x0e,
      0xa4, 0xcc, 0x81, 0x98, 0x99, 0xf2, 0x08, 0x3a,
      0x85, 0xf0, 0xfa, 0xa3, 0xe5, 0x78, 0xf8, 0x07,
      0x7a, 0x2e, 0x3f, 0xf4, 0x67, 0x29, 0x66, 0x5b },
    { 0x3e, 0x8a, 0x69, 0xb7, 0x78, 0x3c, 0x25, 0x85,
      0x19, 0x33, 0xab, 0x62, 0x90, 0xaf, 0x6c, 0xa7,
      0x7a, 0x99, 0x81, 0x48, 0x08, 0x50, 0x00, 0x9c,
      0xc5, 0x57, 0x7c, 0x6e, 0x1f, 0x57, 0x3b, 0x4e,
      0x68, 0x01, 0xdd, 0x23, 0xc4, 0xa7, 0xd6, 0x79,
      0xcc, 0xf8, 0xa3, 0x86, 0xc6, 0x74, 0xcf, 0xfb },
    { 0xb0, 0xba, 0x46, 0x56, 0x37, 0x45, 0x8c, 0x69,
      0x90, 0xe5, 0xa8, 0xc5, 0xf6, 0x1d, 0x4a, 0xf7,
      0xe5, 0x76, 0xd9, 0x7f, 0xf9, 0x4b, 0x87, 0x2d,
      0xe7, 0x6f, 0x80, 0x50, 0x36, 0x1e, 0xe3, 0xdb,
      0xa9, 0x1c, 0xa5, 0xc1, 0x1a, 0xa2, 0x5e, 0xb4,
      0xd6, 0x79, 0x27, 0x5c, 0xc5, 0x78, 0x80, 0x63,
      0xa5, 0xf1, 0x97, 0x41, 0x12, 0x0c, 0x4f, 0x2d,
      0xe2, 0xad, 0xeb, 0xeb, 0x10, 0xa2, 0x98, 0xdd }
  },
  {   20, 20, { 16, 16, 16, 16 },
    { 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
      0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
      0x0c, 0x0c, 0x0c, 0x0c },
    { 0x54, 0x65, 0x73, 0x74, 0x20, 0x57, 0x69, 0x74,
      0x68, 0x20, 0x54, 0x72, 0x75, 0x6e, 0x63, 0x61,
      0x74, 0x69, 0x6f, 0x6e },
    { 0x0e, 0x2a, 0xea, 0x68, 0xa9, 0x0c, 0x8d, 0x37,
      0xc9, 0x88, 0xbc, 0xdb, 0x9f, 0xca, 0x6f, 0xa8 },
    { 0xa3, 0xb6, 0x16, 0x74, 0x73, 0x10, 0x0e, 0xe0,
      0x6e, 0x0c, 0x79, 0x6c, 0x29, 0x55, 0x55, 0x2b },
    { 0x3a, 0xbf, 0x34, 0xc3, 0x50, 0x3b, 0x2a, 0x23,
      0xa4, 0x6e, 0xfc, 0x61, 0x9b, 0xae, 0xf8, 0x97 },
    { 0x41, 0x5f, 0xad, 0x62, 0x71, 0x58, 0x0a, 0x53,
      0x1d, 0x41, 0x79, 0xbc, 0x89, 0x1d, 0x87, 0xa6 }
  },
  {   131, 54, { 28, 32, 48, 64 },
    { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa },
    { 0x54, 0x65, 0x73, 0x74, 0x20, 0x55, 0x73, 0x69,
      0x6e, 0x67, 0x20, 0x4c, 0x61, 0x72, 0x67, 0x65,
      0x72, 0x20, 0x54, 0x68, 0x61, 0x6e, 0x20, 0x42,
      0x6c, 0x6f, 0x63, 0x6b, 0x2d, 0x53, 0x69, 0x7a,
      0x65, 0x20, 0x4b, 0x65, 0x79, 0x20, 0x2d, 0x20,
      0x48, 0x61, 0x73, 0x68, 0x20, 0x4b, 0x65, 0x79,
      0x20, 0x46, 0x69, 0x72, 0x73, 0x74 },
    { 0x95, 0xe9, 0xa0, 0xdb, 0x96, 0x20, 0x95, 0xad,
      0xae, 0xbe, 0x9b, 0x2d, 0x6f, 0x0d, 0xbc, 0xe2,
      0xd4, 0x99, 0xf1, 0x12, 0xf2, 0xd2, 0xb7, 0x27,
      0x3f, 0xa6, 0x87, 0x0e },
    { 0x60, 0xe4, 0x31, 0x59, 0x1e, 0xe0, 0xb6, 0x7f,
      0x0d, 0x8a, 0x26, 0xaa, 0xcb, 0xf5, 0xb7, 0x7f,
      0x8e, 0x0b, 0xc6, 0x21, 0x37, 0x28, 0xc5, 0x14,
      0x05, 0x46, 0x04, 0x0f, 0x0e, 0xe3, 0x7f, 0x54 },
    { 0x4e, 0xce, 0x08, 0x44, 0x85, 0x81, 0x3e, 0x90,
      0x88, 0xd2, 0xc6, 0x3a, 0x04, 0x1b, 0xc5, 0xb4,
      0x4f, 0x9e, 0xf1, 0x01, 0x2a, 0x2b, 0x58, 0x8f,
      0x3c, 0xd1, 0x1f, 0x05, 0x03, 0x3a, 0xc4, 0xc6,
      0x0c, 0x2e, 0xf6, 0xab, 0x40, 0x30, 0xfe, 0x82,
      0x96, 0x24, 0x8d, 0xf1, 0x63, 0xf4, 0x49, 0x52 },
    { 0x80, 0xb2, 0x42, 0x63, 0xc7, 0xc1, 0xa3, 0xeb,
      0xb7, 0x14, 0x93, 0xc1, 0xdd, 0x7b, 0xe8, 0xb4,
      0x9b, 0x46, 0xd1, 0xf4, 0x1b, 0x4a, 0xee, 0xc1,
      0x12, 0x1b, 0x01, 0x37, 0x83, 0xf8, 0xf3, 0x52,
      0x6b, 0x56, 0xd0, 0x37, 0xe0, 0x5f, 0x25, 0x98,
      0xbd, 0x0f, 0xd2, 0x21, 0x5d, 0x6a, 0x1e, 0x52,
      0x95, 0xe6, 0x4f, 0x73, 0xf6, 0x3f, 0x0a, 0xec,
      0x8b, 0x91, 0x5a, 0x98, 0x5d, 0x78, 0x65, 0x98 }
  },
  {   131, 152, { 28, 32, 48, 64 },
    { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      0xaa, 0xaa, 0xaa },
    { 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20,
      0x61, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x75,
      0x73, 0x69, 0x6e, 0x67, 0x20, 0x61, 0x20, 0x6c,
      0x61, 0x72, 0x67, 0x65, 0x72, 0x20, 0x74, 0x68,
      0x61, 0x6e, 0x20, 0x62, 0x6c, 0x6f, 0x63, 0x6b,
      0x2d, 0x73, 0x69, 0x7a, 0x65, 0x20, 0x6b, 0x65,
      0x79, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x61, 0x20,
      0x6c, 0x61, 0x72, 0x67, 0x65, 0x72, 0x20, 0x74,
      0x68, 0x61, 0x6e, 0x20, 0x62, 0x6c, 0x6f, 0x63,
      0x6b, 0x2d, 0x73, 0x69, 0x7a, 0x65, 0x20, 0x64,
      0x61, 0x74, 0x61, 0x2e, 0x20, 0x54, 0x68, 0x65,
      0x20, 0x6b, 0x65, 0x79, 0x20, 0x6e, 0x65, 0x65,
      0x64, 0x73, 0x20, 0x74, 0x6f, 0x20, 0x62, 0x65,
      0x20, 0x68, 0x61, 0x73, 0x68, 0x65, 0x64, 0x20,
      0x62, 0x65, 0x66, 0x6f, 0x72, 0x65, 0x20, 0x62,
      0x65, 0x69, 0x6e, 0x67, 0x20, 0x75, 0x73, 0x65,
      0x64, 0x20, 0x62, 0x79, 0x20, 0x74, 0x68, 0x65,
      0x20, 0x48, 0x4d, 0x41, 0x43, 0x20, 0x61, 0x6c,
      0x67, 0x6f, 0x72, 0x69, 0x74, 0x68, 0x6d, 0x2e },
    { 0x3a, 0x85, 0x41, 0x66, 0xac, 0x5d, 0x9f, 0x02,
      0x3f, 0x54, 0xd5, 0x17, 0xd0, 0xb3, 0x9d, 0xbd,
      0x94, 0x67, 0x70, 0xdb, 0x9c, 0x2b, 0x95, 0xc9,
      0xf6, 0xf5, 0x65, 0xd1 },
    { 0x9b, 0x09, 0xff, 0xa7, 0x1b, 0x94, 0x2f, 0xcb,
      0x27, 0x63, 0x5f, 0xbc, 0xd5, 0xb0, 0xe9, 0x44,
      0xbf, 0xdc, 0x63, 0x64, 0x4f, 0x07, 0x13, 0x93,
      0x8a, 0x7f, 0x51, 0x53, 0x5c, 0x3a, 0x35, 0xe2 },
    { 0x66, 0x17, 0x17, 0x8e, 0x94, 0x1f, 0x02, 0x0d,
      0x35, 0x1e, 0x2f, 0x25, 0x4e, 0x8f, 0xd3, 0x2c,
      0x60, 0x24, 0x20, 0xfe, 0xb0, 0xb8, 0xfb, 0x9a,
      0xdc, 0xce, 0xbb, 0x82, 0x46, 0x1e, 0x99, 0xc5,
      0xa6, 0x78, 0xcc, 0x31, 0xe7, 0x99, 0x17, 0x6d,
      0x38, 0x60, 0xe6, 0x11, 0x0c, 0x46, 0x52, 0x3e },
    { 0xe3, 0x7b, 0x6a, 0x77, 0x5d, 0xc8, 0x7d, 0xba,
      0xa4, 0xdf, 0xa9, 0xf9, 0x6e, 0x5e, 0x3f, 0xfd,
      0xde, 0xbd, 0x71, 0xf8, 0x86, 0x72, 0x89, 0x86,
      0x5d, 0xf5, 0xa3, 0x2d, 0x20, 0xcd, 0xc9, 0x44,
      0xb6, 0x02, 0x2c, 0xac, 0x3c, 0x49, 0x82, 0xb1,
      0x0d, 0x5e, 0xeb, 0x55, 0xc3, 0xe4, 0xde, 0x15,
      0x13, 0x46, 0x76, 0xfb, 0x6d, 0xe0, 0x44, 0x60,
      0x65, 0xc9, 0x74, 0x40, 0xfa, 0x8c, 0x6a, 0x58 }
  }
};

#include <stdio.h>

int main()
{   unsigned int    i, j, k, key_len = 64;
    unsigned char   mac[HMAC_MAX_OUTPUT_SIZE];

#ifdef SHA_1
    for(i = 0; i < SHA1_TESTS; ++i)
    {
        hmac_sha(HMAC_SHA1, t_sha1[i].key, t_sha1[i].key_len, t_sha1[i].text, t_sha1[i].txt_len, mac, t_sha1[i].mac_len);

        printf("\nHMAC-SHA1 test %i, ", i + 1);
        printf("key %s", memcmp(t_sha1[i].mac, mac, t_sha1[i].mac_len) ? "is bad" : "is good");
        printf("\n");
        for(j = 0; j < t_sha1[i].mac_len; j += 4)
            printf("0x%02x%02x%02x%02x ", mac[j], mac[j + 1], mac[j + 2], mac[j + 3]);
        printf("\n");
    }

    for(i = 0; i < SHA1_TESTS; ++i)
    {   hmac_ctx    cx[1];

        hmac_sha_begin(HMAC_SHA1, cx);
        hmac_sha_key(t_sha1[i].key, t_sha1[i].key_len / 2, cx);
        hmac_sha_key(t_sha1[i].key + t_sha1[i].key_len / 2, t_sha1[i].key_len - t_sha1[i].key_len / 2, cx);

        hmac_sha_data(t_sha1[i].text, t_sha1[i].txt_len / 2, cx);
        hmac_sha_data(t_sha1[i].text + t_sha1[i].txt_len / 2, t_sha1[i].txt_len - t_sha1[i].txt_len / 2, cx);

        hmac_sha_end(mac, t_sha1[i].mac_len, cx);

        printf("\nHMAC-SHA1 test %i, ", i + 1);
        printf("mac %s", memcmp(t_sha1[i].mac, mac, t_sha1[i].mac_len) ? "is bad" : "is good");
        printf("\n");
        for(j = 0; j < t_sha1[i].mac_len; j += 4)
            printf("0x%02x%02x%02x%02x ", mac[j], mac[j + 1], mac[j + 2], mac[j + 3]);
        printf("\n");
    }
#endif

#ifdef SHA_256
    for(i = 0; i < SHA256_TESTS; ++i)
    {
        hmac_sha(HMAC_SHA256, t_sha256[i].key, t_sha256[i].key_len, t_sha256[i].text,
                                t_sha256[i].txt_len, mac,t_sha256[i].mac_len);

        printf("\nHMAC-SHA256 test %i, ", i + 1);
        printf("mac %s", memcmp(t_sha256[i].mac, mac, t_sha256[i].mac_len)
                                                ? "is bad" : "is good");

        for(j = 0; j < t_sha256[i].mac_len; j += 4)
        {
            if(j % 16 == 0)
                printf("\n");
            printf("0x%02x%02x%02x%02x ", mac[j], mac[j + 1], mac[j + 2], mac[j + 3]);
        }
        printf("\n");
    }

    for(i = 0; i < SHA256_TESTS; ++i)
    {   hmac_ctx    cx[1];

        hmac_sha_begin(HMAC_SHA256, cx);
        hmac_sha_key(t_sha256[i].key, t_sha256[i].key_len / 2, cx);
        hmac_sha_key(t_sha256[i].key + t_sha256[i].key_len / 2,
                        t_sha256[i].key_len - t_sha256[i].key_len / 2, cx);

        hmac_sha_data(t_sha256[i].text, t_sha256[i].txt_len / 2, cx);
        hmac_sha_data(t_sha256[i].text + t_sha256[i].txt_len / 2,
                        t_sha256[i].txt_len - t_sha256[i].txt_len / 2, cx);

        hmac_sha_end(mac, t_sha256[i].mac_len, cx);

        printf("\nHMAC-SHA256 test %i, ", i + 1);
        printf("mac %s", memcmp(t_sha256[i].mac, mac, t_sha256[i].mac_len)
                                                ? "is bad" : "is good");

        for(j = 0; j < t_sha256[i].mac_len; j += 4)
        {
            if(j % 16 == 0)
                printf("\n");
            printf("0x%02x%02x%02x%02x ", mac[j], mac[j + 1], mac[j + 2], mac[j + 3]);
        }
        printf("\n");
    }
#endif

#ifdef SHA_224
    for(i = 0; i < SHA2_TESTS; ++i)
    {   hmac_ctx cx[1];
        hmac_sha_begin(HMAC_SHA224, cx);
        hmac_sha_key(t_s2[i].key, t_s2[i].key_len / 2, cx);
        hmac_sha_key(t_s2[i].key + t_s2[i].key_len / 2,
                    t_s2[i].key_len - t_s2[i].key_len / 2, cx);

        hmac_sha_data(t_s2[i].text, t_s2[i].txt_len / 2, cx);
        hmac_sha_data(t_s2[i].text + t_s2[i].txt_len / 2,
                        t_s2[i].txt_len - t_s2[i].txt_len / 2, cx);

        hmac_sha_end(mac, t_s2[i].mac_len[0], cx);

        printf("\nHMAC-SHA224 test %i, ", i + 1);
        printf("mac %s", memcmp(t_s2[i].r224, mac, t_s2[i].mac_len[0])
                                                ? "is bad" : "is good");
        for(k = 0; k < t_s2[i].mac_len[0]; k += 4)
        {
            if(k % 16 == 0)
                printf("\n");
            printf("0x%02x%02x%02x%02x ", mac[k], mac[k + 1], mac[k + 2], mac[k + 3]);
        }
        printf("\n");
    }
#endif

#ifdef SHA_256
    for(i = 0; i < SHA2_TESTS; ++i)
    {   hmac_ctx cx[1];
        hmac_sha_begin(HMAC_SHA256, cx);
        hmac_sha_key(t_s2[i].key, t_s2[i].key_len / 2, cx);
        hmac_sha_key(t_s2[i].key + t_s2[i].key_len / 2,
                    t_s2[i].key_len - t_s2[i].key_len / 2, cx);

        hmac_sha_data(t_s2[i].text, t_s2[i].txt_len / 2, cx);
        hmac_sha_data(t_s2[i].text + t_s2[i].txt_len / 2,
                        t_s2[i].txt_len - t_s2[i].txt_len / 2, cx);

        hmac_sha_end(mac, t_s2[i].mac_len[1], cx);

        printf("\nHMAC-SHA256 test %i, ", i + 1);
        printf("mac %s", memcmp(t_s2[i].r256, mac, t_s2[i].mac_len[1])
                                                ? "is bad" : "is good");
        for(k = 0; k < t_s2[i].mac_len[1]; k += 4)
        {
            if(k % 16 == 0)
                printf("\n");
            printf("0x%02x%02x%02x%02x ", mac[k], mac[k + 1], mac[k + 2], mac[k + 3]);
        }
        printf("\n");
    }
#endif

#ifdef SHA_384
    for(i = 0; i < SHA2_TESTS; ++i)
    {   hmac_ctx cx[1];
        hmac_sha_begin(HMAC_SHA384, cx);
        hmac_sha_key(t_s2[i].key, t_s2[i].key_len / 2, cx);
        hmac_sha_key(t_s2[i].key + t_s2[i].key_len / 2,
                    t_s2[i].key_len - t_s2[i].key_len / 2, cx);

        hmac_sha_data(t_s2[i].text, t_s2[i].txt_len / 2, cx);
        hmac_sha_data(t_s2[i].text + t_s2[i].txt_len / 2,
                        t_s2[i].txt_len - t_s2[i].txt_len / 2, cx);

        hmac_sha_end(mac, t_s2[i].mac_len[2], cx);

        printf("\nHMAC-SHA384 test %i, ", i + 1);
        printf("mac %s", memcmp(t_s2[i].r384, mac, t_s2[i].mac_len[2])
                                                ? "is bad" : "is good");
        for(k = 0; k < t_s2[i].mac_len[2]; k += 4)
        {
            if(k % 16 == 0)
                printf("\n");
            printf("0x%02x%02x%02x%02x ", mac[k], mac[k + 1], mac[k + 2], mac[k + 3]);
        }
        printf("\n");
    }
#endif

#ifdef SHA_512
    for(i = 0; i < SHA2_TESTS; ++i)
    {   hmac_ctx cx[1];
        hmac_sha_begin(HMAC_SHA512, cx);
        hmac_sha_key(t_s2[i].key, t_s2[i].key_len / 2, cx);
        hmac_sha_key(t_s2[i].key + t_s2[i].key_len / 2,
                    t_s2[i].key_len - t_s2[i].key_len / 2, cx);

        hmac_sha_data(t_s2[i].text, t_s2[i].txt_len / 2, cx);
        hmac_sha_data(t_s2[i].text + t_s2[i].txt_len / 2,
                        t_s2[i].txt_len - t_s2[i].txt_len / 2, cx);

        hmac_sha_end(mac, t_s2[i].mac_len[3], cx);

        printf("\nHMAC-SHA512 test %i, ", i + 1);
        printf("mac %s", memcmp(t_s2[i].r512, mac, t_s2[i].mac_len[3])
                                                ? "is bad" : "is good");
        for(k = 0; k < t_s2[i].mac_len[3]; k += 4)
        {
            if(k % 16 == 0)
                printf("\n");
            printf("0x%02x%02x%02x%02x ", mac[k], mac[k + 1], mac[k + 2], mac[k + 3]);
        }
        printf("\n");
    }
#endif

    printf("\n\n");
    return 0;
}
