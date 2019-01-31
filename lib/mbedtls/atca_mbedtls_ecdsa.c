/**
 * \brief Replace mbedTLS ECDSA Functions with hardware acceleration &
 * hardware key security
 *
 * \copyright (c) 2017 Microchip Technology Inc.
 *
 * \page License
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/* mbedTLS boilerplate includes */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECDSA_C)

#include "mbedtls/ecdsa.h"

/* Cryptoauthlib Includes */
#include "cryptoauthlib.h"
#include "basic/atca_basic.h"
#include <string.h>


#ifdef MBEDTLS_ECDSA_SIGN_ALT
/*
 * Compute ECDSA signature of a hashed message (SEC1 4.1.3)
 * Obviously, compared to SEC1 4.1.3, we skip step 4 (hash message)
 */
int mbedtls_ecdsa_sign(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                       const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret = 0;
    uint8_t raw_sig[ATCA_SIG_SIZE];

    ((void)f_rng);
    ((void)p_rng);

    if (ATCA_KEY_SIZE > blen || !d || !d->p)
    {
        ret = MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    }

    if (!ret)
    {
        ret = atcab_sign(*(uint16_t*)d->p, buf, raw_sig);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(r, raw_sig, ATCA_SIG_SIZE / 2);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(s, &raw_sig[ATCA_SIG_SIZE / 2], ATCA_SIG_SIZE / 2);
    }

    return ret;
}

#endif /* MBEDTLS_ECDSA_SIGN_ALT */

#endif /* MBEDTLS_ECDSA_C */