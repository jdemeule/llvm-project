.. title:: clang-tidy - misc-unused-variable

misc-unused-variable
====================

Finds unused ``variable`` declarations.
Limitations:
  - global variables are excluded
  - cannot reuse the -Wunused-variable deduction, so the computation of
    unused could be slow.

Example:

.. code-block:: c++

  void f() {
    int a = 0; // Never actually used.
  }
