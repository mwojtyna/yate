file(READ ${FILE_PATH} FILE_CONTENT)

# Escape special characters
string(REPLACE "\\" "\\\\" FILE_CONTENT "${FILE_CONTENT}")
string(REPLACE "\"" "\\\"" FILE_CONTENT "${FILE_CONTENT}")
string(REPLACE "\n" "\\n\"\n\"" FILE_CONTENT "${FILE_CONTENT}")

file(WRITE ${OUTPUT_FILE} "#include <string>\n\n")
file(APPEND ${OUTPUT_FILE} "const std::string ${VARIABLE_NAME} = \"${FILE_CONTENT}\";\n")
