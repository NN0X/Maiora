syntax clear

" -----------------------
" Keywords & Control Flow
" -----------------------
syntax keyword maioraKeyword public private entry declare implement
syntax keyword maioraControl if else while for return

" -----------------------
" Type Definitions (add more here)
" -----------------------
syntax keyword maioraType sint64 sint32 sint16 sint8 uint64 uint32 uint16 uint8 bool none ascii

" -----------------------
" Import Statements
" -----------------------
syntax match maioraImport "^#import\s\+\S\+"

" -----------------------
" Function Definitions
" Highlights only the function name in definitions like: int64 myFunc(...)
" -----------------------
syntax match maioraFunctionDef "\(int64\|uint64\|bool\|none\|ascii\)\s\+\zs\w\+\ze\s*("

" -----------------------
" Function Calls (e.g., print(...) or myFunc(...), excluding control keywords)
" -----------------------
syntax match maioraFunctionCall "\<\w\+\ze\s*(" containedin=ALLBUT,maioraFunctionDef,maioraControl

" -----------------------
" Module Access (e.g., IO::print)
" Highlights `IO` as a module
" -----------------------
syntax match maioraModule "\<\w\+\ze::"

" -----------------------
" String Literals ("Hello")
" -----------------------
syntax region maioraString start=+"+ skip=+\\"+ end=+"+ keepend

" -----------------------
" Number Literals: signed/unsigned like -12s, 0u
" -----------------------
syntax match maioraNumber "\<-\=\d\+\(s\|u\)\>"

" -----------------------
" Comments
" -----------------------
syntax match maioraComment "//.*$"

" -----------------------
" Semicolons
" -----------------------
syntax match maioraSemicolon ";"

" -----------------------
" Highlight Links
" -----------------------
highlight link maioraKeyword Keyword
highlight link maioraControl Identifier
highlight link maioraType Type
highlight link maioraImport PreProc
highlight link maioraFunctionDef Function
highlight link maioraFunctionCall Function
highlight link maioraModule Structure
highlight link maioraString String
highlight link maioraNumber Number
highlight link maioraComment Comment
highlight link maioraSemicolon Statement
