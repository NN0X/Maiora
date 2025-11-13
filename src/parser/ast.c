#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"
#include "statements.h"
#include "../lexer/lexer.h"
#include "../lexer/token.h"

int splitByScope(LTok_t* tokens, uint64_t* indexes, uint64_t* numIndexes, uint64_t begin, uint64_t end)
{
        if (tokens == NULL)
        {
                fprintf(stderr, "splitByScope: tokens is NULL.\n");
                return 1;
        }
        if (indexes == NULL)
        {
                fprintf(stderr, "splitByScope: indexes is NULL.\n");
                return 1;
        }
        if (numIndexes == NULL)
        {
                fprintf(stderr, "splitByScope: numIndexes is NULL.\n");
                return 1;
        }

        uint64_t numOpenScopes = 0;
        uint64_t numCloseScopes = 0;

        indexes[*numIndexes] = begin;
        (*numIndexes)++;

        bool isInsideFormattedString = false;

        for (uint64_t i = begin; i < end; i++)
        {
                TTypes_t type = tokens[i].token;
                if (type == TOK_OP_DQUOTE)
                {
                        isInsideFormattedString = !isInsideFormattedString;
                }
                if (isInsideFormattedString)
                {
                        continue;
                }

                switch (type)
                {
                        case TOK_OP_LCURLY:
                                if (numOpenScopes == numCloseScopes)
                                {
                                        indexes[*numIndexes] = i;
                                        (*numIndexes)++;
                                }
                                numOpenScopes++;
                                break;
                        case TOK_OP_RCURLY:
                                numCloseScopes++;
                                if (numOpenScopes != 0 && numOpenScopes == numCloseScopes)
                                {
                                        indexes[*numIndexes] = i;
                                        (*numIndexes)++;
                                }
                                break;
                        default:
                                break;
                }
        }

        indexes[*numIndexes] = end;
        (*numIndexes)++;

        if (numOpenScopes != numCloseScopes)
        {
                fprintf(stderr, "Number of '{' doesn't match number of '}'.\n");
                return 1;
        }

        return 0;
}

int splitBySColon(LTok_t* tokens, uint64_t* newIndexes, uint64_t* numNewIndexes,
                  uint64_t* indexes, uint64_t numIndexes, uint64_t begin, uint64_t end)
{
        if (tokens == NULL)
        {
                fprintf(stderr, "splitBySColon: tokens is NULL.\n");
                return 1;
        }
        if (indexes == NULL)
        {
                fprintf(stderr, "splitBySColon: indexes is NULL.\n");
                return 1;
        }
        if (newIndexes == NULL)
        {
                fprintf(stderr, "splitBySColon: newIndexes is NULL.\n");
                return 1;
        }
        if (numNewIndexes == NULL)
        {
                fprintf(stderr, "splitBySColon: numNewIndexes is NULL.\n");
                return 1;
        }

        uint64_t currentIndex = 0;
        uint64_t scopeDepth = 0;
        for (uint64_t i = begin; i < end; i++)
        {
                TTypes_t token = tokens[i].token;
                if (indexes[currentIndex] == i)
                {
                        newIndexes[*numNewIndexes] = i;
                        (*numNewIndexes)++;
                        currentIndex++;
                }
                else if (token == TOK_OP_LCURLY)
                {
                        scopeDepth++;
                }
                else if (token == TOK_OP_RCURLY)
                {
                        scopeDepth--;
                }
                else if (token == TOK_OP_SEMICOLON && scopeDepth == 0)
                {
                        newIndexes[*numNewIndexes] = i;
                        (*numNewIndexes)++;
                }

        }

        return 0;
}

int isScopedNode(ANode_t* node, bool* result)
{
        if (node == NULL)
        {
                fprintf(stderr, "isScopedNode: node is NULL.\n");
                return 1;
        }
        if (result == NULL)
        {
                fprintf(stderr, "isScopedNode: result is NULL.\n");
                return 1;
        }

        switch (node->type)
        {
                case AST_FUNC_DECLARE:
                        *result = true;
                        return 0;
                case AST_STATEMENT:
                        if (isStatementScoped(node, result) != 0)
                        {
                                fprintf(stderr, "isStatementScoped failed.\n");
                                return 1;
                        }
                        return 0;
                case AST_EMPTY:
                case AST_ROOT:
                case AST_VAR_DECLARE:
                case AST_EXPRESSION:
                        *result = false;
                        return 0;
                default:
                        fprintf(stderr, "isScopedNode: Unknown AST Node type.\n");
                        return 1;
        }

        return 0;
}

int generateEmptyNode(ANode_t* node)
{
        if (node == NULL)
        {
                fprintf(stderr, "generateEmptyNode: node is NULL.\n");
                return 1;
        }

        node->type = AST_EMPTY;
        node->astData = NULL;

        return 0;
}

int generateNode(LTok_t* tokens, uint64_t begin, uint64_t end, ANode_t* node)
{
        if (tokens == NULL)
        {
                fprintf(stderr, "generateNode: tokens is NULL.\n");
                return 1;
        }
        if (node == NULL)
        {
                fprintf(stderr, "generateNode: node is NULL.\n");
                return 1;
        }

        if (begin == end)
        {
                if (generateEmptyNode(node) != 0)
                {
                        fprintf(stderr, "generateEmptyNode failed.\n");
                        return 1;
                }
                printf("is empty\n");
                return 0;
        }

        bool containsEntry = false;
        bool containsVisibilitySpecifier = false;
        bool containsPars = false;
        bool containsAssign = false;
        bool containsType = false;
        bool containsStatementKeyword = false;
        bool containsID = false;

        for (uint64_t i = begin; i < end; i++)
        {
                TTypes_t type = tokens[i].token;
                if (type == TOK_KEYW_ENTRY)
                {
                        containsEntry = true;
                        break;
                }
                else if (type == TOK_KEYW_PUBLIC || type == TOK_KEYW_PRIVATE)
                {
                        containsVisibilitySpecifier = true;
                }
                else if (type == TOK_OP_LPAR)
                {
                        containsPars = true;
                }
                else if (type == TOK_OP_ASSIGN)
                {
                        containsAssign = true;
                }
                else if (type > TOK_TYPE_NONE && type <= TOK_TYPE_UTF8)
                {
                        containsType = true;
                }
                else if (type >= TOK_KEYW_WHILE && type <= TOK_KEYW_ASM)
                {
                        containsStatementKeyword = true;
                        break;
                }
                else if (type == TOK_ID)
                {
                        containsID = true;
                }
        }

        bool isFuncDecl = false;
        bool isVarDecl = false;
        bool isStmt = false;

        if (containsEntry)
        {
                isFuncDecl = true;
        }
        else if (containsStatementKeyword)
        {
                isStmt = true;
        }
        else if (containsType && containsAssign && containsID)
        {
                isVarDecl = true;
        }
        else if (containsType && containsVisibilitySpecifier && containsID && containsPars)
        {
                isFuncDecl = true;
        }

        if (containsAssign && !isVarDecl)
        {
                isStmt = true;
        }


        if (isFuncDecl)
        {
                /*if (generateFuncDeclNode() != 0)
                {
                        return 1;
                }*/
                node->type = AST_FUNC_DECLARE;
                printf("is func decl\n");
        }
        else if (isVarDecl)
        {
                /*if (generateVarDeclNode() != 0)
                {
                        return 1;
                }*/
                node->type = AST_VAR_DECLARE;
                printf("is var decl\n");
        }
        else if (isStmt)
        {
                /*if (generateStatementNode() != 0)
                {
                        return 1;
                }*/
                node->type = AST_STATEMENT;
                printf("is stmt\n");
        }
        else
        {
                /*if (generateExpressionNode() != 0)
                {
                        return 1;
                }*/
                node->type = AST_EXPRESSION;
                printf("is expr\n");
        }

        return 0;
}

int linkNodeToRoot(ANode_t* node, ANode_t* parent)
{
        if (node == NULL)
        {
                fprintf(stderr, "linkNodeToRoot: node is NULL.\n");
                return 1;
        }
        if (parent == NULL)
        {
                fprintf(stderr, "linkNodeToRoot: parent is NULL.\n");
                return 1;
        }

        ARoot_t* root = (ARoot_t*)(parent->astData);
        if (root == NULL)
        {
                fprintf(stderr, "linkNodeToRoot: root is NULL.\n");
                return 1;
        }

        root->body[root->bodyNum] = node;
        root->bodyNum++;

        return 0;
}

int linkNodeToFuncDeclare(ANode_t* node, ANode_t* parent)
{
        if (node == NULL)
        {
                fprintf(stderr, "linkNodeToFuncDeclare: node is NULL.\n");
                return 1;
        }
        if (parent == NULL)
        {
                fprintf(stderr, "linkNodeToFuncDeclare: parent is NULL.\n");
                return 1;
        }

        AStmt_t* stmt = (AStmt_t*)(node->astData);
        if (stmt == NULL)
        {
                fprintf(stderr, "linkNodeToFuncDeclare: stmt is NULL.\n");
                return 1;
        }
        if (stmt->type != STMT_SCOPE)
        {
                fprintf(stderr, "linkNodeToFuncDeclare: stmt is not a SCOPE statement.\n");
                return 1;
        }

        AFDec_t* funcDecl = (AFDec_t*)(parent->astData);
        if (funcDecl == NULL)
        {
                fprintf(stderr, "linkNodeToFuncDeclare: funcDecl is NULL.\n");
                return 1;
        }

        funcDecl->scope = node;

        return 0;
}

int linkNodeToParent(ANode_t* node)
{
        if (node == NULL)
        {
                fprintf(stderr, "linkNodeToParent: node is NULL.\n");
                return 1;
        }

        ANode_t* parent = node->parent;
        switch(parent->type)
        {
                case AST_ROOT:
                        if (linkNodeToRoot(node, parent) != 0)
                        {
                                fprintf(stderr, "linkNodeToRoot failed.\n");
                                return 1;
                        }
                        break;
                case AST_FUNC_DECLARE:
                        if (linkNodeToFuncDeclare(node, parent) != 0)
                        {
                                fprintf(stderr, "linkNodeToFuncDeclare failed.\n");
                                return 1;
                        }
                        break;
                case AST_STATEMENT:
                        if (linkNodeToStatement(node, parent) != 0)
                        {
                                fprintf(stderr, "linkNodeToStatement failed.\n");
                                return 1;
                        }
                        break;
                case AST_VAR_DECLARE:
                        fprintf(stderr, "Variable declaration should not have scoped children nodes.\n");
                        return 1;
                case AST_EXPRESSION:
                        fprintf(stderr, "Expression should not have scoped children nodes.\n");
                        return 1;
                default:
                        fprintf(stderr, "Unknown AST Node type.\n");
                        return 1;
        }

        return 0;
}

int generateNodes(LTok_t* tokens, uint64_t* indexes, uint64_t numIndexes, ANode_t* parent, PBound_t* boundaries)
{
        if (tokens == NULL)
        {
                fprintf(stderr, "generateNodes: tokens is NULL.\n");
                return 1;
        }
        if (indexes == NULL)
        {
                fprintf(stderr, "generateNodes: indexes is NULL.\n");
                return 1;
        }
        if (parent == NULL)
        {
                fprintf(stderr, "generateNodes: parent is NULL.\n");
                return 1;
        }
        if (boundaries == NULL)
        {
                fprintf(stderr, "generateNodes: boundaries is NULL.\n");
                return 1;
        }

        for (uint64_t i = 0; i < numIndexes - 1; i++)
        {
                ANode_t* node = (ANode_t*)malloc(sizeof(ANode_t));
                if (node == NULL)
                {
                        fprintf(stderr, "malloc failed for node.\n");
                        return 1;
                }

                uint64_t beginIndex = indexes[i];
                if (beginIndex > 0)
                {
                        beginIndex++;
                }

                if (generateNode(tokens, beginIndex, indexes[i + 1], node) != 0)
                {
                        fprintf(stderr, "generateNode failed.\n");
                        return 1;
                }

                node->parent = parent;

                if (i < numIndexes - 2 && tokens[indexes[i + 1]].token == TOK_OP_LCURLY)
                {
                        uint64_t boundaryIndex = boundaries->size;
                        boundaries->begins[boundaryIndex] = indexes[i + 1] + 1;
                        uint64_t nextRCurlyIndex = i + 2;
                        while (tokens[indexes[nextRCurlyIndex]].token != TOK_OP_RCURLY)
                        {
                                nextRCurlyIndex++;
                        }
                        boundaries->ends[boundaryIndex] = indexes[nextRCurlyIndex];

                        if (boundaries->begins[boundaryIndex] >= boundaries->ends[boundaryIndex])
                        {
                                i = nextRCurlyIndex;
                                continue;
                        }

                        bool isScoped = false;
                        if (isScopedNode(node, &isScoped) != 0)
                        {
                                fprintf(stderr, "isScopedNode failed.\n");
                                return 1;
                        }

                        if (!isScoped)
                        {
                                boundaries->parentNodes[boundaryIndex] = parent;
                        }
                        else
                        {
                                ANode_t* scopeNode = (ANode_t*)malloc(sizeof(ANode_t));
                                if (scopeNode == NULL)
                                {
                                        fprintf(stderr, "malloc failed for scopeNode.\n");
                                        return 1;
                                }

                                if (generateScopeNode(scopeNode) != 0)
                                {
                                        fprintf(stderr, "generateScopeNode failed.\n");
                                        return 1;
                                }
                                node->parent = scopeNode;
                                scopeNode->parent = parent;
                                boundaries->parentNodes[boundaryIndex] = scopeNode;
                        }

                        boundaries->size++;
                        i = nextRCurlyIndex;
                }

                if (node->parent == NULL)
                {
                        fprintf(stderr, "node->parent is NULL.\n");
                        return 1;
                }

                if (linkNodeToParent(node) != 0)
                {
                        fprintf(stderr, "linkNodeToParent failed.\n");
                        return 1;
                }
        }

        return 0;
}

int generateAST(LData_t lexerData, ANode_t* root)
{
        uint64_t maxSize = lexerData.metadata.numTokens;

        if (root->type != AST_ROOT)
        {
                fprintf(stderr, "generateAST: root node is not of type AST_ROOT.\n");
                return 1;
        }
        if (root->astData == NULL)
        {
                ARoot_t* astRootData = (ARoot_t*)malloc(sizeof(ARoot_t));
                if (astRootData == NULL)
                {
                        fprintf(stderr, "malloc failed for astRootData.\n");
                        return 1;
                }
                astRootData->body = (ANode_t**)malloc(maxSize * sizeof(ANode_t*));
                if (astRootData->body == NULL)
                {
                        fprintf(stderr, "malloc failed for astRootData->body.\n");
                        return 1;
                }
                astRootData->bodyNum = 0;
                root->astData = (void*)astRootData;
        }

        uint64_t* indexesScope = (uint64_t*)malloc(maxSize * sizeof(uint64_t));
        if (indexesScope == NULL)
        {
                fprintf(stderr, "malloc failed for indexesScope.\n");
                return 1;
        }

        uint64_t* indexesSColon = (uint64_t*)malloc(maxSize * sizeof(uint64_t));
        if (indexesSColon == NULL)
        {
                fprintf(stderr, "malloc failed for indexesSColon.\n");
                return 1;
        }

        PBound_t boundaries;
        boundaries.size = 1;
        boundaries.offset = 0;
        boundaries.begins = (uint64_t*)malloc(maxSize * sizeof(uint64_t));
        if (boundaries.begins == NULL)
        {
                fprintf(stderr, "malloc failed for boundaries.begins.\n");
                return 1;
        }
        boundaries.ends = (uint64_t*)malloc(maxSize * sizeof(uint64_t));
        if (boundaries.ends == NULL)
        {
                fprintf(stderr, "malloc failed for boundaries.ends.\n");
                return 1;
        }
        boundaries.parentNodes = (ANode_t**)malloc(maxSize * sizeof(ANode_t*));
        if (boundaries.parentNodes == NULL)
        {
                fprintf(stderr, "malloc failed for boundaries.parentNodes.\n");
                return 1;
        }
        boundaries.begins[0] = 0;
        boundaries.ends[0] = lexerData.metadata.numTokens;
        boundaries.parentNodes[0] = root;

        while (boundaries.size > boundaries.offset)
        {
                uint64_t begin = boundaries.begins[boundaries.offset];
                uint64_t end = boundaries.ends[boundaries.offset];
                ANode_t* parent = boundaries.parentNodes[boundaries.offset];

                fprintf(stderr, "Processing boundary %lu: begin=%lu, end=%lu, size=%lu\n", 
                boundaries.offset, begin, end, boundaries.size);

                uint64_t numIndexesScope = 0;
                if (splitByScope(lexerData.tokens, indexesScope, &numIndexesScope,
                                 begin, end) != 0)
                {
                        fprintf(stderr, "splitByScope failed.\n");
                        return 1;
                }

                uint64_t numIndexesSColon = 0;
                if (splitBySColon(lexerData.tokens, indexesSColon, &numIndexesSColon,
                                  indexesScope, numIndexesScope, begin, end) != 0)
                {
                        fprintf(stderr, "splitBySColon failed.\n");
                        return 1;
                }

                if (generateNodes(lexerData.tokens, indexesSColon, numIndexesSColon, parent, &boundaries) != 0)
                {
                        fprintf(stderr, "generateNodes failed.\n");
                        return 1;
                }

                boundaries.offset++;
        }

        return 0;
}

// INFO: PLAN:
// 1. split tokens into groups by scopes
//      a) stuff before { has to be a function declaration or statement id(ex: if, for, while, unsafe, asm)
//      b) sequence { a;a;a; { b;b;b; } c;c;c; { d;d;d; } } { e;e;e; } is split into: [ a;a;a; { b;b;b; } c;c;c; { d;d;d; }, e;e;e; ]
// 2. split groups created in step 1 by ;
//      a) [ a;a;a; { b;b;b; } c;c;c; { d;d;d; } , e;e;e; ] => [ a, a, a, { b;b;b; }, c, c, c, { d;d;d; } ], [ e, e, e ]
// 3. parse each element that is not a scope into statements / expressions / declarations
//      a) after this [ a, a, a, { b;b;b; }, c, c, c, { d;d;d; } ] becomes [ stuff, { b;b;b; }, stuff, { d;d;d; } ]
// 4. repeat steps 1-3 until no more scopes are found

// EXAMPLE:
//
// entry sint32 main(none)
// {
//      sint32 a = 5s;
//
//      unsafe
//      {
//          sint16 b = a;
//      }
//
//      if (a > b)
//      {
//          uint16 c = 0;
//          while (c < 10)
//          {
//              print(a);
//              c++;
//          }
//      }
//
//      return 0s;
// }
//
// INPUT:
// entry sint32 main(none){ sint32 a = 5s; unsafe{ sint16 b = a; } if(a > b){ uint16 c = 0; while(c < 10){ print(a); c++; } } return 0s; }
// NODE: [ AST_ROOT: [ ] ]
//                    ^ HERE PASS 1
//
// PASS 1:
// 1. [ entry sint32 main(none),
//      { sint32 a = 5s; unsafe{ sint16 b = a; } if(a > b){ uint16 c = 0; while(c < 10){ print(a); c++; } } return 0s; }
//    ]
// 2. [ entry sint32 main(none),
//      { sint32 a = 5s; unsafe{ sint16 b = a; } if(a > b){ uint16 c = 0; while(c < 10){ print(a); c++; } } return 0s; }
//    ]
// 3. [ FUNC_DECL,
//      { sint32 a = 5s; unsafe{ sint16 b = a; } if(a > b){ uint16 c = 0; while(c < 10){ print(a); c++; } } return 0s; }
//    ]
// NODE: [ AST_ROOT: [ FUNC_DECL: [ ] ] ]
//                                ^ HERE PASS 2
//
// PASS 2:
// 1. [ sint32 a = 5s; unsafe{ sint16 b = a; } if(a > b){ uint16 c = 0; while(c < 10){ print(a); c++; } } return 0s; ]
// 2. [ sint32 a = 5s,
//      unsafe{ sint16 b = a; } if(a > b){ uint16 c = 0; while(c < 10){ print(a); c++; } },
//      return 0s
//    ]
// 3. [ VAR_DECL,
//      unsafe{ sint16 b = a; } if(a > b){ uint16 c = 0; while(c < 10){ print(a); c++; } },
//      STMT
//    ]
// NODE: [ AST_ROOT: [ FUNC_DECL: [ VAR_DECL, , STMT ] ]
//                                           ^ HERE PASS 3
//
// PASS 3:
// 1. [ unsafe,
//      { sint16 b = a; },
//      if(a > b),
//      { uint16 c = 0; while(c < 10){ print(a); c++; }
//    ]
// 2. [ unsafe,
//      { sint16 b = a; },
//      if(a > b),
//      { uint16 c = 0; while(c < 10){ print(a); c++; }
//    ]
// 3. [ STMT,
//      { sint16 b = a; },
//      STMT,
//      { uint16 c = 0; while(c < 10){ print(a); c++; }
//    ]
// NODE: [ AST_ROOT: [ FUNC_DECL: [ VAR_DECL, STMT: [ ], STMT: [ ], STMT ] ]
//                                     HERE PASS 4.1 ^          ^ HERE_PASS 4.2
//
// PASS 4.1 (unsafe stmt):
// 1. [ sint16 b = a;]
// 2. [ sint16 b = a ]
// 3. [ VAR_DECL ]
// NODE: [ AST_ROOT: [ FUNC_DECL: [ VAR_DECL, STMT: [ VAR_DECL ], STMT: [ ], STMT ] ] ]
//                                                                       ^ HERE PASS 4.2
//
// PASS 4.2 (if stmt):
// 1. [ uint16 c = 0; while(c < 10){ print(a); c++; } ]
// 2. [ uint16 c = 0,
//      while(c < 10){ print(a); c++; }
//    ]
// 3. [ VAR_DECL,
//      while(c < 10){ print(a); c++; }
//    ]
// NODE: [ AST_ROOT: [ FUNC_DECL: [ VAR_DECL, STMT: [ VAR_DECL ], STMT: [ VAR_DECL, ], STMT ] ] ]
//                                                                                 ^ HERE PASS 5
//
// PASS 5:
// 1. [ while(c < 10),
//      { print(a); c++; }
//    ]
// 2. [ while(c < 10),
//      { print(a); c++; }
//    ]
// 3. [ STMT,
//      { print(a); c++; }
//    ]
// NODE: [ AST_ROOT: [ FUNC_DECL: [ VAR_DECL, STMT: [ VAR_DECL ], STMT: [ VAR_DECL, STMT: [ ] ], STMT ] ] ]
//                                                                                         ^ HERE PASS 6
//
// PASS 6:
// 1. [ print(a); c++; ]
// 2. [ print(a),
//      c++
//    ]
// 3. [ EXPR,
//      EXPR,
//    ]
// NODE: [ AST_ROOT: [ FUNC_DECL: [ VAR_DECL, STMT: [ VAR_DECL ], STMT: [ VAR_DECL, STMT: [ EXPR, EXPR ] ], STMT ] ] ]
//
//
// PASS 7:
// STOP
//
// FINAL TREE:
//
//  AST_ROOT:
//      FUNC_DECL:
//          VAR_DECL
//          STMT:
//              VAR_DECL
//          STMT:
//              VAR_DECL
//              STMT:
//                  EXPR
//                  EXPR
//          STMT
