#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "M5Siv3D.h"

// ここにテストケースを追加していきます
TEST_CASE("M5Siv3D basic test", "[basic]") {
    M5Siv3D siv3d;
    REQUIRE(true);  // 基本的なテスト
} 