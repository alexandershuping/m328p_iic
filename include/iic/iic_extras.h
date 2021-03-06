/* 
   Copyright 2018 Alexander Shuping

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 
 * iic_extras.h
 * Extra IIC functions (e.g. automatic address negotiation.
 */

#pragma once
#include <iic/common.h>

typedef uint8_t IIC_COMMAND_t;

//===========================================================================//
//== Dynamic-address-allocation commands                                   ==//
//===========================================================================//

/* IIC_COMMAND_REQUEST_ADDRESS
 * target address: 0x01 (address server) ONLY
 * length: 0
 * purpose: request an address from the address server
 */
#define IIC_COMMAND_REQUEST_ADDRESS 0xA0

/* IIC_COMMAND_ADDRESS_ALLOCATION
 * target address: 0x00 (general-call) ONLY
 * length: 1
 * syntax: [ command | NEW_ADDRESS ]
 * purpose: inform the device which last sent IIC_COMMAND_REQUEST_ADDRESS that
 *          the address "NEW_ADDRESS" has been allocated for it.
 */
#define IIC_COMMAND_ADDRESS_ALLOCATION 0xA1

/* IIC_COMMAND_NO_ROOM_ON_BUS
 * target address: 0x00 (general-call) ONLY
 * length: 0
 * purpose: inform the device which last sent IIC_COMMAND_REQUEST_ADDRESS that
 *          an address could not be allocated for it, as the iic bus is
 *          currently full.
 * note: if you *really* need an address, try RELEASE_REQUEST'ing addresses
 *       until one of them fails to dispute.
 */
#define IIC_COMMAND_NO_ROOM_ON_BUS 0xA2

/* IIC_COMMAND_RELEASE_ADDRESS
 * target address: 0x01 (address server) ONLY
 * length: 1
 * syntax: [ command | ADDRESS_TO_RELEASE ]
 * purpose: request that the address "ADDRESS_TO_RELEASE" be un-allocated. Used
 *          by slaves that are about to disconnect from the bus, and by the
 *          address server when it wants to release an inactive address.
 */
#define IIC_COMMAND_RELEASE_REQUEST 0xA9

/* IIC_COMMAND_RELEASE_ACKNOWLEDGE
 * target address: 0x00 (general-call) ONLY
 * length: 1
 * syntax: [ command | RELEASED_ADDRESS ]
 * purpose: inform the device at RELEASED_ADDRESS that its address has been
 *          un-allocated, and it should immediately stop listening at that
 *          address. Also inform other devices that the address is now free.
 */
#define IIC_COMMAND_RELEASE_ACKNOWLEDGE 0xAA
#define IIC_COMMAND_RELEASE_DISPUTED 0xAB
#define IIC_COMMAND_RELEASE_FORCE 0xAC
#define IIC_COMMAND_RELEASE_NOT_ALLOCATED 0xAD // sent when we have no record of a slave at the requested address


//===========================================================================//
//== LED control commands                                                  ==//
//===========================================================================//
/* IIC_COMMAND_LED_WRITE_WORD
 * target address: any
 * length: 3
 * syntax: [ command | CHANNEL | LOW_BYTE | HIGH_BYTE ]
 * purpose: write the RGB channel at CHANNEL to the value represented by the
 *          16-bit integer (HIGH_BYTE << 8) | (LOW_BYTE)
 *
 * note: channel values are:
 *       * 0 = RED
 *       * 1 = GREEN
 *       * 2 = BLUE
 */
#define IIC_COMMAND_LED_WRITE_WORD 0x20

/* IIC_COMMAND_LED_SET_PATTERN
 * target address: any
 * length: 1
 * syntax: [ command | PATTERN_NUMBER ]
 * purpose: set the pattern for an LED device to use.
 */
#define IIC_COMMAND_LED_SET_PATTERN 0x21

/* IIC_COMMAND_LED_INCLUDE_DEVICE
 * target address: any except general-call
 * length: 1
 * syntax: [ command | PHASE ]
 * purpose: inform a device that, at the next SYNCHRONIZE command, it
 *          should immediately jump to the position "`PHASE`" in its
 *          current waveform.
 */
#define IIC_COMMAND_LED_INCLUDE_DEVICE 0x26

/* IIC_COMMAND_LED_EXCLUDE_DEVICE
 * target address: any except general-call
 * length: 0
 * syntax: [ command ]
 * purpose: inform a device that it should ignore the next SYNCHRONIZE
 *          command, even if it is an INCLUSIVE_SYNCHRONIZE.
 */
#define IIC_COMMAND_LED_EXCLUDE_DEVICE 0x27

/* IIC_COMMAND_LED_INCLUDE_GROUP
 * target address: 0x00 (general-call) ONLY
 * length: 2
 * syntax: [ command | GROUP_ID | PHASE ]
 * purpose: as IIC_COMMAND_LED_INCLUDE_DEVICE, except it targets all 
 *          devices in the group `GROUP_ID`, rather than a single device.
 */
#define IIC_COMMAND_LED_INCLUDE_GROUP 0x28

/* IIC_COMMAND_LED_EXCLUDE_GROUP
 * target address: 0x00 (general-call) ONLY
 * length: 1
 * syntax: [ command | GROUP_ID ]
 * purpose: as IIC_COMMAND_LED_EXCLUDE_DEVICE, except it targets all
 *          devices in the group `GROUP_ID`, rather than a single device.
 */
#define IIC_COMMAND_LED_EXCLUDE_GROUP 0x29

/* IIC_COMMAND_LED_INCLUSIVE_SYNCRONIZE
 * target address: 0x00 (general-call) ONLY
 * length: 1
 * syntax: [ command | PHASE ]
 * purpose: informs all devices that they should immediately jump to the
 *          offset `PHASE` in their waveforms, unless they have been
 *          specifically excluded with IIC_COMMAND_LED_EXCLUDE_DEVICE or
 *          IIC_COMMAND_LED_EXCLUDE_GROUP.
 * note: if a device was included with IIC_COMMAND_LED_INCLUDE_DEVICE or
 *       IIC_COMMAND_LED_INCLUDE_GROUP, and the `PHASE` specified in that
 *       command is different from the `PHASE` specified in this command,
 *       the `PHASE` from the INCLUDE command will be used instead of
 *       the `PHASE` from the SYNCHRONIZE command.
 */
#define IIC_COMMAND_LED_INCLUSIVE_SYNCHRONIZE 0x2A

/* IIC_COMMAND_LED_EXCLUSIVE_SYNCHRONIZE
 * target address: 0x00 (general-call) ONLY
 * length: 0
 * syntax: [ command ]
 * purpose: informs devices which were previously included with the 
 *          IIC_COMMAND_LED_INCLUDE_DEVICE or IIC_COMMAND_LED_INCLUDE_GROUP
 *          commands that they should immediately jump to an offset in 
 *          their waveform as specified by the INCLUDE command.
 */
#define IIC_COMMAND_LED_EXCLUSIVE_SYNCHRONIZE 0x2B

#ifdef ADDRESS_SERVER
bool handle_address_negotiation(uint8_t command);
bool handle_address_release(uint8_t command, uint8_t slave_address, uint8_t dispute_byte);
void do_address_response(uint8_t allocated_address);
#endif
