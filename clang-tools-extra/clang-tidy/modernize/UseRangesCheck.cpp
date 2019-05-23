//===--- UseRangesCheck.cpp - clang-tidy ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UseRangesCheck.h"

#include "LoopConvertUtils.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace modernize {

void UseRangesCheck::registerMatchers(MatchFinder *Finder) {
  auto BeginExpr = callExpr(callee(functionDecl(hasName("begin"))));
  auto EndExpr = callExpr(callee(functionDecl(hasName("end"))));
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("::std::copy"))),
               has(implicitCastExpr(has(declRefExpr().bind("name")))),
               hasArgument(0, BeginExpr.bind("begin")),
               hasArgument(1, EndExpr.bind("end")))
          .bind("match"),
      this);
}

static const Expr *
findContainerExprFromBeginEndCallExpr(const CallExpr *BeginEndExpr) {
  const auto *BeginCalleeExpr = BeginEndExpr->getCallee();
  if (const auto *Member = dyn_cast<MemberExpr>(BeginCalleeExpr)) {
    return Member->getBase();
  } else {
    return BeginEndExpr->getArg(0);
  }
}

static const Expr *findContainerExpr(const MatchFinder::MatchResult &Result) {
  const auto *BeginCallExpr = Result.Nodes.getNodeAs<CallExpr>("begin");
  const auto *EndCallExpr = Result.Nodes.getNodeAs<CallExpr>("end");
  const auto *BeginCtnExpr =
      findContainerExprFromBeginEndCallExpr(BeginCallExpr);
  const auto *EndCtnExpr = findContainerExprFromBeginEndCallExpr(EndCallExpr);
  if (!areSameExpr(Result.Context, BeginCtnExpr, EndCtnExpr))
    return nullptr;
  return BeginCtnExpr;
}

void UseRangesCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchExpr = Result.Nodes.getNodeAs<CallExpr>("match");
  const auto *MatchDecl = Result.Nodes.getNodeAs<DeclRefExpr>("name");

  auto Diag = diag(MatchExpr->getExprLoc(),
                   "Consider to replace 'std::copy' by 'std::ranges::copy'")
              << FixItHint::CreateReplacement(MatchDecl->getSourceRange(),
                                              "std::ranges::copy");

  const auto *BeginCallExpr = Result.Nodes.getNodeAs<CallExpr>("begin");
  const auto *EndCallExpr = Result.Nodes.getNodeAs<CallExpr>("end");
  const auto *ContainerExpr = findContainerExpr(Result);
  if (ContainerExpr) {
    Diag << FixItHint::CreateRemoval(CharSourceRange::getTokenRange(
        BeginCallExpr->getBeginLoc(), EndCallExpr->getEndLoc()));
  }
}

} // namespace modernize
} // namespace tidy
} // namespace clang
