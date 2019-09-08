// RUN: %check_clang_tidy %s misc-unused-function %t

void f() {}

static void unused_f() {} // 0
// CHECK-MESSAGES: :[[@LINE-1]]:13: warning: function 'unused_f' is probably unused [misc-unused-function]
// CHECK-FIXES: {{^}}// 0

static void fwded_unused_f(); // 1
// CHECK-MESSAGES: :[[@LINE-1]]:13: warning: function 'fwded_unused_f' is probably unused [misc-unused-function]
// CHECK-FIXES: {{^}}// 1

static void fwded_unused_f() {} // 2
// CHECK-MESSAGES: :[[@LINE-1]]:13: warning: function 'fwded_unused_f' is probably unused [misc-unused-function]
// CHECK-FIXES: {{^}}// 2

static void used_f() {}
static void fwded_used_f();
static void fwded_used_f() {}

void xyz() {
  used_f();
  fwded_used_f();
}
