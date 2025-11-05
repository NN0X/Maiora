#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"
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

        for (uint64_t i = begin; i < end; i++)
        {
                switch (tokens[i].token)
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

        if (numOpenScopes != 0 && numOpenScopes != numCloseScopes)
        {
                fprintf(stderr, "Number of '{' doesn't match number of '}'.\n");
                return 1;
        }

        return 0;
}

int splitBySColon(LTok_t* tokens, uint64_t* newIndexes, uint64_t* numNewIndexes, uint64_t* indexes, uint64_t numIndexes,
                  uint64_t begin, uint64_t end)
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

        uint64_t currentScope = 0;
        for (uint64_t i = begin; i < end; i++)
        {
                if (currentScope != 0 && i >= indexes[2*currentScope - 2] && i < indexes[2*currentScope - 1])
                {
                        continue;
                }

                switch(tokens[i].token)
                {
                        case TOK_OP_LCURLY:
                                newIndexes[*numNewIndexes] = i;
                                (*numNewIndexes)++;
                                break;
                        case TOK_OP_RCURLY:
                                newIndexes[*numNewIndexes] = i;
                                (*numNewIndexes)++;
                                currentScope++;
                                break;
                        case TOK_OP_SEMICOLON:
                                newIndexes[*numNewIndexes] = i;
                                (*numNewIndexes)++;
                                break;
                        default:
                                break;
                }

        }

        return 0;
}

int generateNode(LTok_t* tokens, uint64_t begin, uint64_t end, ANode_t* node)
{
        return 0;
}


// TODO: add a way to bind unprocessed indexes to already processed nodes
// ex. in if(a > b){print(a)}, print(a) is bound to if(a > b)
int generateNodes(LTok_t* tokens, uint64_t* indexes, uint64_t numIndexes, ANode_t* parent)
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

        for (uint64_t i = 0; i < numIndexes; i+=2)
        {
                ANode_t* node = (ANode_t*)malloc(sizeof(ANode_t));
                if (node == NULL)
                {
                        fprintf(stderr, "malloc failed for node.\n");
                        return 1;
                }

                node->parent = parent;
                if (generateNode(tokens, indexes[i], indexes[i + 1], node) != 0)
                {
                        fprintf(stderr, "generateNode failed.\n");
                        return 1;
                }
        }

        return 0;
}

int generateAST(LData_t lexerData, ANode_t* root)
{
        uint64_t begin = 0;
        uint64_t end = lexerData.metadata.numTokens;

        uint64_t* indexesScope = (uint64_t*)malloc(end * sizeof(uint64_t));
        if (indexesScope == NULL)
        {
                fprintf(stderr, "malloc failed for indexesScope.\n");
                return 1;
        }

        uint64_t* indexesSColon = (uint64_t*)malloc(end * sizeof(uint64_t));
        if (indexesSColon == NULL)
        {
                fprintf(stderr, "malloc failed for indexesSColon.\n");
                return 1;
        }

        while (begin != end)
        {
                uint64_t numIndexesScope = 0;
                if (splitByScope(lexerData.tokens, indexesScope, &numIndexesScope, begin, end) != 0)
                {
                        fprintf(stderr, "splitByScope failed.\n");
                        return 1;
                }

                uint64_t numIndexesSColon = 0;
                if (splitBySColon(lexerData.tokens, indexesSColon, &numIndexesSColon, indexesScope, numIndexesScope, begin, end) != 0)
                {
                        fprintf(stderr, "splitBySColon failed.\n");
                        return 1;
                }

                if (generateNodes(lexerData.tokens, indexesSColon, numIndexesSColon, root) != 0)
                {
                        fprintf(stderr, "generateNodes failed.\n");
                        return 1;
                }
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
