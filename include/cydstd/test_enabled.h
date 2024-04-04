//
// Created by castle on 4/3/24.
//

#ifndef CYD_UI_TEST_ENABLED_H
#define CYD_UI_TEST_ENABLED_H

/// Enable all test related macros
#ifdef ENABLE_TESTING
/// [ENABLED] Make public for all tests
#define TEST_PUBLIC :public
#else
/// [DISABLED] Make public for all tests, must define `ENABLE_TESTING`
#define TEST_PUBLIC
#endif

#endif //CYD_UI_TEST_ENABLED_H
