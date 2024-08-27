#!/usr/bin/env bash

SCRIPT_DIR=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)
FIRMWARE_DIR=$SCRIPT_DIR/..

FILES_TO_CHECK=$(
	find ${FIRMWARE_DIR} \
	-type f \
	-not -path "${FIRMWARE_DIR}/ptsw*" \
	-not -path "${FIRMWARE_DIR}/drivers/include/stm32l552xx.h" \
	-not -path "${FIRMWARE_DIR}/lib*" \
	-not -path "${FIRMWARE_DIR}/build*" \
	\( -name '*.h' -or -name '*.hpp' -or -name '*.c' -or -name '*.cpp' \)
)

for FILE_TO_CHECK in $FILES_TO_CHECK
do
	echo "Checking $FILE_TO_CHECK..."
	clang-format $FILE_TO_CHECK --dry-run --Werror
	ERR=$?
	if [ $ERR -ne 0 ]; then
		echo "$FILE_TO_CHECK not formatted properly!"
		break
	fi
done

exit $ERR
