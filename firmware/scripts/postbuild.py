#!/usr/bin/env python3

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-s', '--size', type=str, help='Size of the application image in hexadecimals.')
parser.add_argument('-i', '--input', type=str, help='Path to the application binary.')
parser.add_argument('-o', '--output', type=str, help='Path to the postbuild output binary.')

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

	crcValue = 0
	size = int(args.size, 16)
	with open(args.input, 'rb') as inputFile:
		offset = 0
		for byte in inputFile.read():
			crcValue = UpdateCrc16(crcValue, byte)
			print("CRC: {} @ {}: {}".format(hex(crcValue), hex(offset), byte))
			offset += 1
		print('Calculated CRC: {}'.format(hex(crcValue)))

		size = offset

		with open(args.output, '+wb') as outputFile:
			for x in range(0, 4):
				print('Adding header ID @ {}: {}'.format(hex(HEADER_OFFSET_ID + x), hex(int((HEADER_ID >> (x * 8)) & 0xFF))))
				outputFile.write(int((HEADER_ID >> (x * 8)) & 0xFF).to_bytes(1, 'little'))
			for x in range(0, 4):
				print('Adding header size @ {}: {}'.format(hex(HEADER_OFFSET_SIZE + x), hex(int((size >> (x * 8)) & 0xFF))))
				outputFile.write(int((size >> (x * 8)) & 0xFF).to_bytes(1, 'little'))
			for x in range(0, 4):
				print('Adding header crc @ {}: {}'.format(hex(HEADER_OFFSET_CRC + x), hex(int((crcValue >> (x * 8)) & 0xFF))))
				outputFile.write(int((crcValue >> (x * 8)) & 0xFF).to_bytes(1, 'little'))
			for x in range(0, 4):
				print('Adding header reserved @ {}: {}'.format(hex(HEADER_OFFSET_RESERVED + x), hex(int((HEADER_RESERVED >> (x * 8)) & 0xFF))))
				outputFile.write(int((HEADER_RESERVED >> (x * 8)) & 0xFF).to_bytes(1, 'little'))
			
			inputFile.seek(0)
			outputFile.write(inputFile.read())

	exit(0)
