syntax clear

syntax match maioraComment "//.*$"
syntax region maioraString start=+"+ skip=+\\"+ end=+"+ keepend containedin=ALLBUT,maioraComment

syntax keyword maioraKeyword public private entry instance as function containedin=ALLBUT,maioraComment,maioraString
syntax keyword maioraControl if else while for return containedin=ALLBUT,maioraComment,maioraString

syntax keyword maioraMemory heap address free at containedin=ALLBUT,maioraComment,maioraString
syntax keyword maioraType sint64 sint32 sint16 sint8 uint64 uint32 uint16 uint8 bool none ascii containedin=ALLBUT,maioraComment,maioraString

syntax match maioraImport "^#import\s\+\S\+" containedin=ALLBUT,maioraComment,maioraString
syntax match maioraModulePre "^#module\s\+\S\+" containedin=ALLBUT,maioraComment,maioraString
syntax match maioraTypePre "^#type\s\+\S\+" containedin=ALLBUT,maioraComment,maioraString
syntax match maioraModule "\<\w\+\ze::" containedin=ALLBUT,maioraComment,maioraString

syntax match maioraFunctionDef "\(int64\|uint64\|bool\|none\|ascii\)\s\+\zs\w\+\ze\s*(" containedin=ALLBUT,maioraComment,maioraString
syntax match maioraFunctionCall "\<\w\+\ze\s*(" containedin=ALLBUT,maioraFunctionDef,maioraControl,maioraComment,maioraString

syntax match maioraCharacter "'[^']\{0,1}'" containedin=ALLBUT,maioraString,maioraComment
syntax match maioraNumber "\<-\=\d\+\(s\|u\)\>" containedin=ALLBUT,maioraString,maioraComment,maioraCharacter

syntax match maioraSemicolon ";" containedin=ALLBUT,maioraComment,maioraString,maioraCharacter
syntax match maioraColon ":\s*" containedin=ALLBUT,maioraComment,maioraString,maioraCharacter

highlight link maioraKeyword Identifier
highlight link maioraControl Identifier
highlight link maioraMemory Keyword
highlight link maioraType Type
highlight link maioraImport PreProc
highlight link maioraModulePre PreProc
highlight link maioraTypePre Function
highlight link maioraFunctionDef Function
highlight link maioraFunctionCall Function
highlight link maioraModule Structure
highlight link maioraString String
highlight link maioraCharacter Character
highlight link maioraNumber Number
highlight link maioraComment Comment
highlight link maioraSemicolon PreProc
highlight link maioraColon Statement
