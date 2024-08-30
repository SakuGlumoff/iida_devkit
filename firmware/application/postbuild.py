#!/usr/bin/env python3

import subprocess
import os
import datetime

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--input-binary', type=str, help='Path to the application binary.')
parser.add_argument('--input-elf', type=str, help='Path to the application ELF.')
parser.add_argument('--output', type=str, help='Path to the postbuild output ELF.')
parser.add_argument('--app-header-start', type=str, help='Application header start address')
parser.add_argument('--app-code-start', type=str, help='Application code start address')

'''
Header:
  | Offset | Type | Description                                 |
  |--------|------|---------------------------------------------|
  |                 App header                                  |
  |--------|------|---------------------------------------------|
  | 0      | U32  | Identification field (hardcoded 0xDEADBEEF) |
  | 4      | U32  | Size of the image in bytes                  |
  | 8      | U32  | 32-bit CRC value of the image               |
  |--------|------|---------------------------------------------|
  |                 Metadata                                    |
  |--------|------|---------------------------------------------|
  | 12     | STR  | Timestamp as YYYY-MM-DD HH:mm:ss UTC+X      |
  | 37     | STR  | Commit ID (hash)                            |
  | 45     | STR  | ARM GNU toolchain version                   |
'''
HEADER_SIZE = 0x100

HEADER_ID_LEN = 4
HEADER_SIZE_LEN = 4
HEADER_CRC_LEN = 4

HEADER_ID = int(0xDEADBEEF)

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
		headerBytesCount = int(args['app_code_start'], 16) - int(args['app_header_start'], 16)
		print('Header bytes count: {}'.format(hex(headerBytesCount)))
		for byte in inputFile.read()[headerBytesCount:]:
			crc = UpdateCrc16(crc, byte)
			print('CRC: {} @ {}: {}'.format(hex(crc), hex(size + headerBytesCount), byte))
			size += 1
		print('Calculated CRC: {}'.format(hex(crc)))

		subprocess.run([
			'{}/bin/arm-none-eabi-objcopy'.format(os.environ.get('ARM_GNU_PATH')),
			'-O', 'binary',
			'--only-section', '.header',
			args['input_elf'], 'header.bin'
		])

		with open('header.bin', 'r+b') as headerFile:
			print('Updating header ID: {:X}'.format(HEADER_ID))
			for x in range(0, HEADER_ID_LEN):
				headerFile.write(int((HEADER_ID >> (x * 8)) & 0xFF).to_bytes(1, 'little'))
			print('Updating size: {:X}'.format(size))
			for x in range(0, HEADER_SIZE_LEN):
				headerFile.write(int((size >> (x * 8)) & 0xFF).to_bytes(1, 'little'))
			print('Updating CRC: {:X}'.format(crc))
			for x in range(0, HEADER_CRC_LEN):
				headerFile.write(int((crc >> (x * 8)) & 0xFF).to_bytes(1, 'little'))

		print('Updating ELF with header...')
		subprocess.run(['{}/bin/arm-none-eabi-objcopy'.format(os.environ.get('ARM_GNU_PATH')), '--update-section', '.header=header.bin', args['input_elf'], args['output']])
		print('Removing header.bin...')
		os.remove('header.bin')

		print('Postbuild script completed.')

	exit(0)
