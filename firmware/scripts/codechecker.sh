#!/usr/bin/env bash

FILES_TO_CHECK=$(
	find . \
	-type f \
	-not -path './ptsw*' \
	-not -path './drivers/include/stm32l552xx.h' \
	-not -path './lib*' \
	-not -path './build*' \
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
