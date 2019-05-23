//===--- UseRangesCheck.cpp - clang-tidy ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UseRangesCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/Diagnostic.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace modernize {

void UseRangesCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("::std::copy"))),
               has(implicitCastExpr(has(declRefExpr().bind("name")))))
          .bind("match"),
      this);
}

void UseRangesCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchExpr = Result.Nodes.getNodeAs<CallExpr>("match");
  const auto *MatchDecl = Result.Nodes.getNodeAs<DeclRefExpr>("name");
  diag(MatchExpr->getExprLoc(),
       "Consider to replace 'std::copy' by 'std::ranges::copy'")
      << FixItHint::CreateReplacement(MatchDecl->getSourceRange(),
                                      "std::ranges::copy");
}

} // namespace modernize
} // namespace tidy
} // namespace clang
