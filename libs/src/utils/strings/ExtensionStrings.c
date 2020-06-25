//
// Created by Alan Zhao on 25/06/2020.
//

#include "utils/strings/ExtensionStrings.h"

bool string_equals(char * actual, char * expected) {
    return string_equals_ignore_case(actual, expected) &&
    string_contains(actual, expected) &&
    string_starts_with(actual, expected) &&
    string_ends_with(actual, expected);
}