//===--- UnusedFunctionCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UnusedFunctionCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/SmallVector.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace misc {

void UnusedFunctionCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      functionDecl(isStaticStorageClass(), isDefinition()).bind("fct"), this);
  Finder->addMatcher(
      callExpr(callee(functionDecl(isStaticStorageClass()).bind("used"))),
      this);
}

void UnusedFunctionCheck::check(const MatchFinder::MatchResult &Result) {
  auto FindRemovalRange = [&](const FunctionDecl *FctDecl) {
    if (FctDecl->doesThisDeclarationHaveABody()) {
      return CharSourceRange::getTokenRange(FctDecl->getSourceRange());
    }
    return CharSourceRange::getCharRange(
        FctDecl->getBeginLoc(), Lexer::findLocationAfterToken(
                                    FctDecl->getEndLoc(), tok::semi,
                                    *Result.SourceManager, getLangOpts(),
                                    /*SkipTrailingWhitespaceAndNewLine=*/true));
  };

  if (const auto *FoundFctDecl = Result.Nodes.getNodeAs<FunctionDecl>("fct")) {
    llvm::SmallVector<FctDeclContext, 1> Contexts;
    for (const auto *ReDecl : FoundFctDecl->redecls()) {
      Contexts.emplace_back(ReDecl, FindRemovalRange(ReDecl));
    }
    FoundDecls[FoundFctDecl] = std::move(Contexts);
    return;
  }
  if (const auto *UsedFctDecl = Result.Nodes.getNodeAs<FunctionDecl>("used")) {
    FoundDecls.erase(UsedFctDecl);
    return;
  }
}

void UnusedFunctionCheck::onEndOfTranslationUnit() {
  for (const auto &Entry : FoundDecls) {
    for (const auto &Context : Entry.getSecond()) {
      diag(Context.FctDecl->getLocation(), "function %0 is probably unused")
          << Context.FctDecl;
      diag(Context.FctDecl->getLocation(), /*FixDescription=*/"removing %0",
           DiagnosticIDs::Note)
          << FixItHint::CreateRemoval(Context.SourceRange);
    }
  }
}

} // namespace misc
} // namespace tidy
} // namespace clang
