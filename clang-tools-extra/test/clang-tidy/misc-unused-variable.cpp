// RUN: %check_clang_tidy %s misc-unused-variable %t

int get_some_external_int();

void f_with_unused() {
  int unused_var_0; // 0
  // CHECK-MESSAGES: :[[@LINE-1]]:7: warning: variable 'unused_var_0' is probably unused [misc-unused-variable]
  // CHECK-FIXES: {{^}}  // 0

  int unused_var_1 = 1; // 1
  // CHECK-MESSAGES: :[[@LINE-1]]:7: warning: variable 'unused_var_1' is probably unused [misc-unused-variable]
  // CHECK-FIXES: {{^}}  // 1

  int unused_var_2 = get_some_external_int();
  // CHECK-MESSAGES: :[[@LINE-1]]:7: warning: variable 'unused_var_2' is probably unused [misc-unused-variable]
  // CHECK-FIXES: {{^}}  get_some_external_int();{{$}}
}

void eat_0(int);
void eat_1(int &);
void eat_2(int *);

void f_with_used() {
  {
    int used = 0;
    eat_0(used);
  }
  {
    int used = 0;
    eat_1(used);
  }
  {
    int used = 0;
    eat_2(&used);
  }
  {
    int used = 0;
    int unused_var_3 = used; // 3
    // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: variable 'unused_var_3' is probably unused [misc-unused-variable]
    // CHECK-FIXES: {{^}}    // 3
    {
      int unused_var_4 = used; // 4
      // CHECK-MESSAGES: :[[@LINE-1]]:11: warning: variable 'unused_var_4' is probably unused [misc-unused-variable]
      // CHECK-FIXES: {{^}}      // 4
    }
  }
  {
    int used = 0;
    int arr[42];
    arr[used] = 1;
  }
}

struct X {
  X() {
    // simulate an external call where we have no idea of the side effect.
    eat_0(get_some_external_int());
  }
};

struct Y {
  ~Y() {
    // simulate an external call where we have no idea of the side effect.
    eat_0(get_some_external_int());
  }
};

struct Trivial {
};

struct Defaulted {
  Defaulted() = default;
  ~Defaulted() = default;
};

void f_raii() {
  X x;
  Y y;
  Trivial t; // 5
  // CHECK-MESSAGES: :[[@LINE-1]]:11: warning: variable 't' is probably unused [misc-unused-variable]
  // CHECK-FIXES: {{^}}  // 5
  Defaulted d; // 6
  // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: variable 'd' is probably unused [misc-unused-variable]
  // CHECK-FIXES: {{^}}  // 6
}

int global_0 = 0;
static int local_0 = 0; // 7
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: variable 'local_0' is probably unused [misc-unused-variable]
// CHECK-FIXES: {{^}}// 7
