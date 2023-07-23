/**
 * \file ssl.h
 *
 * \brief SSL/TLS functions.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
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
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#ifndef _SIMCOM_SYSTEM_H_
#define _SIMCOM_SYSTEM_H_

 
#define CUS_VERSION       "NULL"

typedef struct
{
	unsigned char               DeycrptNumber[16];
	unsigned char               UserData[16];
	unsigned char               EncryptNumber[16];
	unsigned long               UserKey[26];
	unsigned long               SubKey[26];
	unsigned char               ImeiEncrypt;
} IMEIExtendedDataS;

typedef struct
{
    char *manufacture_namestr;
    char *Module_modelstr;
    char *Revision;
    char *GCAP;
    char *cgmr;
    char *internal_verstr;
    char *version_tagstr;
    char *SDK_Version;
}SIMComVersion;

typedef struct
{
    char sdkversion[50];
}SDKVersion;

typedef struct
{
    char rfversion1[50];
    char rfversion2[50];
}RFVersion;

typedef struct
{
    char cusversion[50];
}CUSVersion;

typedef enum
{
    SC_SYSTEM_SLEEP_DISABLE     =   0,
    SC_SYSTEM_SLEEP_ENABLE      =   1
}SC_SYSTEM_SLEEP_FLAG;

typedef struct
{
  SC_SYSTEM_SLEEP_FLAG sleep_flag;
  unsigned char time;
}SC_SleepEx_str;
#endif
