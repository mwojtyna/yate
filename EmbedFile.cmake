file(READ ${FILE_PATH} FILE_CONTENT HEX)
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," FILE_CONTENT ${FILE_CONTENT})

file(WRITE ${OUTPUT_FILE} "#include <cstddef>\n\n")
file(APPEND ${OUTPUT_FILE} "const char ${VARIABLE_NAME}[] = {${FILE_CONTENT}};\n")
file(APPEND ${OUTPUT_FILE} "const std::size_t ${VARIABLE_NAME}_size = sizeof(${VARIABLE_NAME});\n")
