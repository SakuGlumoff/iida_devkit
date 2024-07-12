#!/usr/bin/env python3

import subprocess
import os

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--input-binary', type=str, help='Path to the application binary.')
parser.add_argument('--input-elf', type=str, help='Path to the application ELF.')
parser.add_argument('--output', type=str, help='Path to the postbuild output ELF.')

'''
Header:
  | Offset | Type | Description                                 |
  |--------|------|---------------------------------------------|
  | 0      | U32  | Identification field (hardcoded 0xDEADBEEF) |
  | 4      | U32  | Size of the image in bytes                  |
  | 8      | U32  | 32-bit CRC value of the image               |
  | 12     | U32  | Reserved                                    |
'''
HEADER_SIZE = 0x10

HEADER_OFFSET_ID = 0
HEADER_OFFSET_SIZE = 4
HEADER_OFFSET_CRC = 8
HEADER_OFFSET_RESERVED = 12

HEADER_ID = int(0xDEADBEEF)
HEADER_RESERVED = int(0x0)

def UpdateCrc16(crcIn: int, byte: int):
	crc = crcIn
	a = byte | 0x100

	crc <<= 1
	a <<= 1
	if (a & 0x100):
		crc += 1
	if (crc & 0x10000):
		crc ^= 0x1021

	while (not (a & 0x10000)):
		crc <<= 1
		a <<= 1
		if (a & 0x100):
			crc += 1
		if (crc & 0x10000):
			crc ^= 0x1021

	return crc & 0xFFFF

if __name__ == '__main__':
	args = parser.parse_args()
	args = vars(args)

	with open(args['input_binary'], 'rb') as inputFile:
		crc = 0
		size = 0
		# Skip header bytes
		for byte in inputFile.read()[HEADER_SIZE:]:
			crc = UpdateCrc16(crc, byte)
			print("CRC: {} @ {}: {}".format(hex(crc), hex(size), byte))
			size += 1
		print('Calculated CRC: {}'.format(hex(crc)))

		with open('app_header.bin', '+wb') as headerFile:
			for x in range(0, 4):
				headerFile.write(int((HEADER_ID >> (x * 8)) & 0xFF).to_bytes(1, 'little'))
			for x in range(0, 4):
				headerFile.write(int((size >> (x * 8)) & 0xFF).to_bytes(1, 'little'))
			for x in range(0, 4):
				headerFile.write(int((crc >> (x * 8)) & 0xFF).to_bytes(1, 'little'))
			for x in range(0, 4):
				headerFile.write(int((HEADER_RESERVED >> (x * 8)) & 0xFF).to_bytes(1, 'little'))

		print("Updating ELF with header...")
		subprocess.run(["{}/bin/arm-none-eabi-objcopy".format(os.environ.get('GNU_ARM_PATH')), "--update-section", ".app_header=app_header.bin", args['input_elf'], args['output']])
		print("Removing app_header.bin...")
		os.remove("app_header.bin")

		print("Postbuild script completed.")

	exit(0)
