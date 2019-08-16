# kon-c
Kon is a simple programming language, aiming for embedding into other host language easily, and easy to make your own DSL. Easy to make nested data using Kon's vector, table, list, cell data container. It also can be used as a markup language with logic code embedded. Heavily inspired by Scheme, JSON, XML. This repo is a c99 implementation.

Now this project is under very early development stage, and may change syntax or feature anytime. Bugs everywhere. Just play it for fun 

# TODO list
- [x] a CPS(Continuation Passing Style) Interpreter
- [x] call-cc (call with current continuation)
- [x] tail-call optimization
- [x] eval, apply
- [x] Lexical-Scoped procedure using `lambda` keyword
- [x]  Dynamic-Scoped procedure using `func` keyword
- [x]  Dynamic-Scoped code block can be assigned to a vairable using `blk` keyword.
- [x] a Mark-Sweep GC.
- [x] a XML-like data language.
- [x] chained calls. SVOVOVO...
- [] Scheme-like Number tower
- [] pattern matching
- [] ffi
- [] jit
- [] vm
- [] coroutine
- [] asyn await
- [] multi-thread support
- [] a simplified Scheme-like marco system, but forbid nesting
- [] a code editor
- [] error handling
- [] break point, debugger
- [] module system
- [] package manager
- [] lightweight regex
- [] Perl6-like grammar syntax
- [] pointer equality call-cc in tail call. `[call-cc [lambda [cc1] [call-cc [lambda [cc2] [eq cc1 cc2]]]]] => #t;`
- [] Racket-like Continuation Marks

# build

## prepare environment
first install [xmake](https://raw.githubusercontent.com/xmake-io/xmake/master/README.md) to build this project.
Mac:
```
brew install xmake
```

## clone code and build
```
https://github.com/seerkong/kon-c.git
cd kon-c
xmake
```

## run kon script file

kli -f -q <file_path>
eg: 
```
xmake run kli -f -q ~/lang/konscript/kon-c/samples/knative/do.kl
```
remove `-q` option to show more debug info

### generate a xcode project
for better debugging, you can generator a xcode project.
```
mkdir -p xcode
```

run the command below each time after add new files
```
xmake project -k cmakelists
cd xcode
cmake .. -G "Xcode"
cd ..
```
if xcode warns missing 'tbox.config.h', copy it to src/tbox/ and add to xcode manually
```
cp ./build/macosx/x86_64/release/tbox.config.h ./src/tbox/
```

# A pamphlet about Kon
Well, here we go. You want to learn Kon? Sure, okay. But first, bear in mind that Kon isn’t done yet. And it does very little.

## An Understanding
a Kon expression is sepreated to a subject and a clause list like nature languages.

expression starts with a `[`, ends with a `]`; clauses is sepreated by `;`
```
[+ % 1 2; .+ 3; |+ 4 5 6]
```
we have a subject: `+`,
and three verb-objects clauses:
`% 1 2` , `.+ 3` , `|+ 4 5 6`

`%` is a infix means 'apply arguments to the subject'.
the subject `+` is a procedure, after `+ % 1 2;` step, the subject changed to a number 3;

`.` is a infix means 'send a message call to the subject', just like method call in  OOP languages. the message signal is `+`, the arguments are `[3]`. after `.+ 3;` step, the subject changed to a number 6;

`|` is a infix means 'pipe the subject to the next function's first argument', just like the pipe symbol in shell cmd: 'ls |grep '. after `|+ 4 5 6` step, the subject changed to `[+ 6 4 5 6] => 21`;

now, after all clauses processed, we got the expression value 21

## Comments
Lines preceded by the backquote '`' are ignored by Kon.
```
`bala bala
[+ % 1 2]
```

## Boolean
true : ` #t; `
false: ` #f; `

## nil
the end of a data structure like list, tree, graph ...
```
#nil;
```

## unknown
container empty placeholder. just like the `[NSNull null]` in objective c
```
#ukn;
```

## undefined
just like the `undefined` in javascript
```
#undef;
```

## char
```
#c,A;
#c,b;
```


## Number

### Number compare

Because '<' and '>' is used as vector start and vector end, the larger than function uses '>>' instead, the smaller than function uses '<<' instead.

```
[>> % 5 3] ` #t;
[<< % 5 3] ` #f;
[>= % 5 5] ` #t;
[<= % 2 4] ` #t;
```

## string
```
"hello world"
```

## Vector
```
< 1 2 3 4 >
```

## Table
a sequence of values.
the key is optional
```
(0 :a 1 :b 2)
```

## List
```
$[1 2 3]
```

## Cell
```
{div (:width 200 :height 100)  
  [
    {text ("hello")}
    {text ("world")}
  ]
}
```

## Define variable
```
[let a 1]
```

## Update variable
```
[set a 2]
```


## Procedure call
```
[+ % 1 2 3] `` return 6
```

## Lexical-Scoped procedure
```
[let a 2]
[lambda foo []
  a
]

[lambda bar []
  [let a 3]
  [foo %]
]

[bar %]   `` return 2
```

## Dynamic-Scoped procedure
```
[let a 2]
[func foo []
  a
]

[lambda bar []
  [let a 3]
  [foo %]
]

[bar %]   `` return 3
```

## Block
```
[let t
  [blk
    [set a 1]
    [+ % a b c]
  ]
]
[let a 5]
[let b 6]
[let c 7]
[t %]   `` return 14

```

## Condition
if
```
[if [>= % i 4]
  [displayln % "if branch expr1"]
  [displayln % "if branch expr2"]
else
  [displayln % "else branch expr1"]
  [displayln % "else branch expr2"]
]
```

cond
```
[let a 5]
[cond
  [[<< % 5 4]
    "condition 1 pass"
  ]
  [[<< % a 4]
    [writeln % "condition 2 pass"]
  ]
  [[<< % 3 4]
    [writeln % "condition 3 pass"]
    [writeln % "balabala"]
  ]
  [else
    "else branch"
  ]
]
```


## loop, break, continue
print a list data
```
[let c $[1 2 3]]

[for [let iter c] [neq % iter #nil;] [set iter [cdr % iter]]
  [writeln % [car % iter]]
]
```



```
[for [let i 0] [<< % i 8] [set i [+ % i 1]]
  [writeln % "i : " i]

  

  [for
    [let j 0] `` init
    [<< % j 4]  `` condition
    [set j [+ % j 1]] `` update after for body

    `` for body content
    [if [>> % j 2]
      [break]
    ]
    [writeln % "    j : " j]
  ]

  [if [>> % i 4]
    [break]
  ]
  
  [if [>> % i 2]
    [continue]
  ]
]
```

## Eval
```
[eval $[+ % 1 [- % 6 3]]]
```

## Apply
```
[lambda foo []
  $+
]

[lambda bar []
  $[10 10 10]
]

[apply [foo %] [bar %] ]
```

## call-cc
```
[let f
  [func [$return]
    [return % 2]
    3
  ]
]

` writes 3
[displayln %
  [f % [lambda [$x] x]]
]

` writes 2
[displayln % [call-cc f]]


## other examples
other examples are in samples/knative folder