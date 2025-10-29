syntax clear

syntax match maioraComment "//.*$"
syntax region maioraString start=+"+ skip=+\\"+ end=+"+ keepend contains=maioraInterpolation,@maioraStringSpecials containedin=ALLBUT,maioraComment
syntax region maiorainterpolation start=+{+ end=+}+ containedin=maioraString contains=ALLBUT,maioraString,maioraComment

syntax keyword maioraKeyword public private entry unsafe asm containedin=ALLBUT,maioraComment,maioraString
syntax keyword maioraControl if else while for return switch case containedin=ALLBUT,maioraComment,maioraString

syntax keyword maioraFunction instance function containedin=ALLBUT,maioraComment,maioraString
syntax keyword maioraMemory heap address reference ref at containedin=ALLBUT,maioraComment,maioraString
syntax keyword maioraType sint64 sint32 sint16 sint8 uint64 uint32 uint16 uint8 float8 float16 float32 float64 bool none ascii utf8 containedin=ALLBUT,maioraComment,maioraString

syntax match maioraImport "^#import\s\+\S\+" containedin=ALLBUT,maioraComment,maioraString
syntax match maioraModulePre "^#module\s\+\S\+" containedin=ALLBUT,maioraComment,maioraString
syntax match maioraTypePre "^#type\s\+\S\+" containedin=ALLBUT,maioraComment,maioraString
syntax match maioraModule "\<\w\+\ze::" containedin=ALLBUT,maioraComment,maioraString

syntax match maioraFunctionDef "\(sint64\|sint32\|sint16\|sint8\|uint8\|uint16\|uint32\|uint64\|float8\|float16\|float32\|float64\|bool\|none\|ascii\|utf8\)\s\+\zs\w\+\ze\s*(" containedin=ALLBUT,maioraComment,maioraString
syntax match maioraFunctionCall "\<\w\+\ze\s*(" containedin=ALLBUT,maioraFunctionDef,maioraControl,maioraComment,maioraString

syntax match maioraCharacter "'[^']\{0,1}'" containedin=ALLBUT,maioraString,maioraComment
syntax match maioraNumber "\<-\=\d\+\(s\|u\)\>" containedin=ALLBUT,maioraString,maioraComment,maioraCharacter
syntax match maioraFloating "\<-\=\(\d\+\.\d*\|\.\d\+\)\(f\|d\)\=\>" containedin=ALLBUT,maioraString,maioraComment,maioraCharacter
syntax match maioraInteger "\<-\=\d\+\>" containedin=ALLBUT,maioraString,maioraComment,maioraCharacter

syntax match maioraSemicolon ";" containedin=ALLBUT,maioraComment,maioraString,maioraCharacter

highlight link maioraKeyword Identifier
highlight link maioraControl Identifier
highlight link maioraFunction Keyword
highlight link maioraMemory Keyword
highlight link maioraType Type
highlight link maioraImport PreProc
highlight link maioraModulePre PreProc
highlight link maioraTypePre Function
highlight link maioraFunctionDef Function
highlight link maioraFunctionCall Function
highlight link maioraModule Structure
highlight link maioraString String
highlight link maioraInterpolation Normal
highlight link maioraCharacter Character
highlight link maioraNumber Number
highlight link maioraFloating Number
highlight link maioraInteger Number
highlight link maioraComment Comment
highlight link maioraSemicolon PreProc
