.. title:: clang-tidy - misc-unused-function

misc-unused-function
====================

Finds unused ``function`` declarations.
Limitations:
  - cannot reuse the -Wunused-function deduction, so the computation of
    unused could be slow.

Example:

.. code-block:: c++

  static void f() {
  }
