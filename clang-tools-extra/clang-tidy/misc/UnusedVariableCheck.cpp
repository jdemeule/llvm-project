//===--- UnusedVariableCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UnusedVariableCheck.h"
#include "../utils/DeclRefExprUtils.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Stmt.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticIDs.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace misc {

void UnusedVariableCheck::registerMatchers(MatchFinder *Finder) {
  // Scoped variable which are not a parameter or global with static keyword.
  // Non trivial type will be exclude later.
  Finder->addMatcher(
      varDecl(unless(parmVarDecl()),
              anyOf(hasAncestor(compoundStmt()),
                    allOf(hasGlobalStorage(), isStaticStorageClass())))
          .bind("vardecl"),
      this);
  Finder->addMatcher(
      declRefExpr(to(varDecl(unless(parmVarDecl())).bind("used"))), this);
}

void UnusedVariableCheck::check(const MatchFinder::MatchResult &Result) {
  auto FindRemovalRange = [&](const VarDecl *FoundVarDecl) {
    auto *Init = FoundVarDecl->getInit();
    if (Init != nullptr && isa<CallExpr>(Init)) {
      return CharSourceRange::getCharRange(FoundVarDecl->getBeginLoc(),
                                           Init->getBeginLoc());
    }
    return CharSourceRange::getCharRange(
        FoundVarDecl->getBeginLoc(),
        Lexer::findLocationAfterToken(
            FoundVarDecl->getEndLoc(), tok::semi, *Result.SourceManager,
            getLangOpts(),
            /*SkipTrailingWhitespaceAndNewLine=*/true));
  };

  if (const auto *FoundVarDecl = Result.Nodes.getNodeAs<VarDecl>("vardecl")) {
    // Non trivial type are excluded. Their constructor/desctructor could have
    // side effect.
    if (!FoundVarDecl->getType().isTrivialType(*Result.Context))
      return;
    Contexts.push_back(
        VarDeclContext(FoundVarDecl, FindRemovalRange(FoundVarDecl)));
    return;
  }
  if (const auto *FoundVarUsage = Result.Nodes.getNodeAs<VarDecl>("used")) {
    auto FoundContext = llvm::find_if(Contexts, [&](const VarDeclContext &Ctx) {
      return Ctx.FoundVarDecl == FoundVarUsage;
    });
    if (FoundContext != Contexts.end())
      FoundContext->IsUsed = true;
  }
}

void UnusedVariableCheck::onEndOfTranslationUnit() {
  for (const auto &Context : Contexts) {
    if (Context.IsUsed)
      continue;
    diag(Context.FoundVarDecl->getLocation(), "variable %0 is probably unused")
        << Context.FoundVarDecl;
    // Emit a fix and a fix description of the check;
    diag(Context.FoundVarDecl->getLocation(), /*FixDescription=*/"removing %0",
         DiagnosticIDs::Note)
        << FixItHint::CreateRemoval(Context.VarDeclRange);
  }
}

} // namespace misc
} // namespace tidy
} // namespace clang
