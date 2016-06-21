set(HTTP_PARSER_DIR http-parser-2.7.0)
add_library(http-parser external/${HTTP_PARSER_DIR}/http_parser.c)
target_include_directories(http-parser INTERFACE external/${HTTP_PARSER_DIR})
