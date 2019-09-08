set(LLVM_LINK_COMPONENTS support)

add_clang_library(clangTidyMiscModule
  DefinitionsInHeadersCheck.cpp
  MiscTidyModule.cpp
  MisplacedConstCheck.cpp
  NewDeleteOverloadsCheck.cpp
  NonCopyableObjects.cpp
  NonPrivateMemberVariablesInClassesCheck.cpp
  RedundantExpressionCheck.cpp
  StaticAssertCheck.cpp
  ThrowByValueCatchByReferenceCheck.cpp
  UnconventionalAssignOperatorCheck.cpp
  UniqueptrResetReleaseCheck.cpp
  UnusedAliasDeclsCheck.cpp
  UnusedFunctionCheck.cpp
  UnusedParametersCheck.cpp
  UnusedUsingDeclsCheck.cpp
  UnusedVariableCheck.cpp

  LINK_LIBS
  clangAnalysis
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangSerialization
  clangTidy
  clangTidyUtils
  clangTooling
  )
