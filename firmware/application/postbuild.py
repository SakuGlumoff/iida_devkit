#!/usr/bin/env python3

from intelhex import IntelHex
import zlib

import logging
logger = logging.getLogger('PostBuild')

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-s', '--size', type=str, help='Size of the application image in hexadecimals.')
parser.add_argument('-i', '--input', type=str, help='Path to the application binary.')
parser.add_argument('-o', '--output', type=str, help='Path to the postbuild output binary.')
parser.add_argument('-v', '--verbose', help='Show verbose output.', action='count', default=logging.NOTSET)

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

if __name__ == '__main__':
	args = parser.parse_args()

	loggerLevel = 0
	if (args.verbose >= 3):
		loggerLevel = logging.DEBUG
	elif (args.verbose == 2):
		loggerLevel = logging.INFO
	elif (args.verbose == 1):
		loggerLevel = logging.WARNING
	elif (args.verbose == 0):
		loggerLevel = logging.ERROR
	logging.basicConfig(level=loggerLevel)

	crcValue = 0
	lowestAddr = int(0xffffffff)
	size = int(args.size, 16)
	with open(args.input, 'r') as inputFile:
		intelHex = IntelHex(inputFile.name)
		intelHexDict = intelHex.todict()
		for key in intelHexDict.keys():
			if (key != 'start_addr'):
				if (key <= lowestAddr):
					lowestAddr = key
				crcValue = zlib.crc32(intelHexDict[key].to_bytes(1, 'little'), crcValue)

		logger.info('Calculated CRC: {}'.format(hex(crcValue)))
		for x in range(0, 4):
			logger.info('Adding header ID @ {}: {}'.format(hex(lowestAddr - HEADER_SIZE + HEADER_OFFSET_ID + x), hex(int((HEADER_ID >> (x * 8)) & 0xFF))))
			intelHexDict[lowestAddr - HEADER_SIZE + HEADER_OFFSET_ID + x] = int((HEADER_ID >> (x * 8)) & 0xFF)
			logger.info('Adding header size @ {}: {}'.format(hex(lowestAddr - HEADER_SIZE + HEADER_OFFSET_SIZE + x), hex(int((size >> (x * 8)) & 0xFF))))
			intelHexDict[lowestAddr - HEADER_SIZE + HEADER_OFFSET_SIZE + x] = int((size >> (x * 8)) & 0xFF)
			logger.info('Adding header crc @ {}: {}'.format(hex(lowestAddr - HEADER_SIZE + HEADER_OFFSET_CRC + x), hex(int((crcValue >> (x * 8)) & 0xFF))))
			intelHexDict[lowestAddr - HEADER_SIZE + HEADER_OFFSET_CRC + x] = int((crcValue >> (x * 8)) & 0xFF)
			logger.info('Adding header reserved @ {}: {}'.format(hex(lowestAddr - HEADER_SIZE + HEADER_OFFSET_RESERVED + x), hex(int((HEADER_RESERVED >> (x * 8)) & 0xFF))))
			intelHexDict[lowestAddr - HEADER_SIZE + HEADER_OFFSET_RESERVED + x] = int((HEADER_RESERVED >> (x * 8)) & 0xFF)

		intelHex.fromdict(intelHexDict)
		with open(args.output, '+w') as outputFile:
			intelHex.tofile(outputFile, 'hex')

	exit(0)
