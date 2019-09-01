# Kunu
Kunu is a programming language. Heavily inspired by Scheme, _why's Potion language, JSON, XML.

Note: Kunu isn’t done yet. The implement may make breaking changes and produce new bugs at anytime. Just play it for fun.

# DONE list
- [x] a CPS (Continuation Passing Style) tree-walking interpreter
- [x] call-cc (call with current continuation)
- [x] simplified marco system. a lexical-scoped macro builder and a dynamic-scoped macro builder.
- [x] eval, apply
- [x] lexical-scoped procedure using `lambda` keyword
- [x] dynamic-scoped procedure using `func` keyword
- [x] quasiquote and unquote.
- [x] a mark-sweep GC.
- [x] A non-backtracking NFA/DFA-based Perl-compatible regex engine.
- [x] a data language like XML and JSON.

# TODO list
- [ ] module system
- [ ] package manager
- [ ] pattern matching
- [ ] scheme-like Number tower
- [ ] register based bitcode interperter
- [ ] ffi
- [ ] compile to GraalVM
- [ ] coroutine
- [ ] asyn await
- [ ] multi-thread support
- [ ] IDE support
- [ ] error handling
- [ ] break point, debugger
- [ ] perl6-like grammar syntax
- [ ] pointer equality call-cc in tail call. `{call-cc {lambda (cc1) #[{call-cc {lambda (cc2) #[ [eq cc1 cc2] ] } } ] } } => #t;`

# build

## prepare environment
first install [xmake](https://raw.githubusercontent.com/xmake-io/xmake/master/README.md) to build this project.
Mac:
```
brew install xmake
```
centos:
```
wget https://raw.githubusercontent.com/xmake-io/xmake/master/scripts/get.sh
chmod +x ./get.sh
XMAKE_ROOT=y ./get.sh
source ~/.xmake/profile
```

## clone code and build
```
git clone https://github.com/seerkong/kunu.c.git
cd kunu.c
xmake
```
use `xmake --root` if is root

## run kon script file

kli <file_path>
eg: 
```
./kli <the path to your awesome script>/xx.kl
```

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
if xcode warns missing 'tbox.config.h', copy it to lib/tbox/ and add to xcode manually
```
cp ./build/macosx/x86_64/release/tbox.config.h ./lib/tbox/
```

# A pamphlet about Kunu
Well, here we go. You want to learn Kunu? Sure, okay. But first, bear in mind that Kunu isn’t done yet. And it does very little.

The examples can be found in ./examples folder

## An Understanding
There are two expression syntax.
One uses list, like Lisp, eg: `[+ 1 2]`
The other uses cell structure.

### Kunu's cell structure
Every cell structure can have a core data, a map of attributes, a sequence of items stored in tuple, a list of children, and a reference to next cell.

eg:
```
{subj1
  :a
  :b=1
  (1 :a=2 :b="zxc" 3)
  #[
    1
    "abc"
  ]
  subj2 :t :f=2 (1 df)
  #[
    ff
  ]
^subj1}
```

similar to xml:
```
<subj1 a b="1">
  <item>1</item>
  <item key="a">2</item>
  <item key="b">zxc</item>
  <item>3</item>
  <list>
    <item>1</item>
    <item>abc</item>
  </list>
  <next>
    <subj2 t f="2">
      <item>1</item>
      <item>df</item>
      <list>
        <item>ff</item>
      </list>
    </subj2>
  </next>
</subj1>

```


similar to json:
```
{
  "core": "subj1",
  "attr": {
    "a": true,
    "b": 1
  },
  "table": [
    {"val": 1},
    {"key": "a", "val": 2},
    {"key": "b", "val": "zxc"},
    {"val": 3}
  ],
  "list": [
    1,
    "abc"
  ],
  "next": {
    "core": "subj2",
    "attr": {
      "t": true,
      "f": 2
    },
    "table": [
      {"val": 1},
      {"val": "df"}
    ],
    "list": [
      "ff"
    ],
    "next": null
  }
}
```

### keywords uses cell structure
All special forms expressions like `cond`, `if` ... uses cell structure.
in cell structure

eg:
```
{if [> a 1]
  #[
    `` if true
  ]
  else
  #[
    `` if false
  ]
}
```

### message passing part
Object message passing expressions uses cell structre too.
A sentence is seperated to a subject and clause list

eg:
```
{kn/str/length % "abc"; | + 4 5 6}
{Person clone}
{p .name := "Loki"}
{p set-sex $male}
```
in Kunu internal, the sentences above will splited as below:

```
[kn [
  [/ str]
  [/ length]
  [% "abc"]
  [| + 4 5 6]
] ]

[Person [
  [clone]
] ]

[p [
  [. name]
  [:= "Loki"]
] ]

[p [
  [set-sex $male]
] ]
```


in message passing style expression:
`/` is a infix means 'get the left value'.
`%` is a infix means 'apply arguments to the object'.
`|` is a infix means 'pipe the object to the next function's first argument', just like the pipe symbol in shell cmd: 'ls |grep '.
`.` is a infix means 'send a signal the object'.
`:=` is a infix means 'assign a value to a left value'.
`;` is a syntax marker, means 'the end of arguments'.

if the first word in a clause is a word, means 'do method call to an object', eg: `{p set-sex $male}`

## Comments
Lines preceded by the two backquotes '``' are ignored by Kunu.
```
``bala bala
[+ 1 2]
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

## string
string
```
"hello world\n"
```
raw string
```
'hello world\n'
```

## Vector
```
#<1 2 3 4 >
```

## Table
a sequence of values.
the key is optional
the `=` is optional
```
#(0 :a 1 :b = 2)
```

## List
quoted list
```
$[1 2 3]
```
quasiquoted list
```
{let a 1}
@[$.a 2 $.[+ a 2] ] `` => [1 2 3 ]
```


## Cell
the data in `()` will be stored in cell table field
the data in `#[]` will be stored in cell list field
the key value pair will be stored in cell map field

eg:
```
${div :width =200 :height = 100 :disabled
  (
    {text ("hello") }
    {text ("world") }
  )  
  #[
    [writeln "script here"]
  ]
}
```

## declare and set variable
```
{let a 1}
{set a 2}
```

## Procedure call
there a two ways to make a procedure call
first is a lisp-like style, using '[' ']'
```
[+ 1 2 3] `` return 6
```
the second is a message passing style, using '{' '}'
the '%' meas apply arguments to the object
```
{+ % 1 2}
```

## Lexical-Scoped procedure
```
{let a 2}
{lambda foo ()
  #[
    a
  ]
}

{lambda bar ()
  #[
    {let a 3}
    [foo]
  ]
}

[bar]   `` return 2
```

## Dynamic-Scoped procedure
```
{let a 2}
{func foo ()
  #[
    a
  ]
}

{lambda bar ()
  #[
    [let a 3]
    [foo]
  ]
}

[bar]   `` return 3
```

## Condition
if
```
{if [>= i 4]
  #[
    [displayln "if branch expr1"]
    [displayln "if branch expr2"]
  ]
  else
  #[
    [displayln "else branch expr1"]
    [displayln "else branch expr2"]
  ]
}
```

cond
```
[let a 5]
{cond
  [< 5 4]
  #[
    "condition 1"
  ]
  [< a 4]
  #[
    [writeln "condition 2"]
  ]
  [< 6 4]
  #[
    [writeln "condition 3"]
    [writeln "do something more here"]
  ]
  else
  #[
    [writeln "else branch"]
  ]
}
```


## loop, break, continue
print a list data
```
{let c $[a b c ] }

{for
  (
    {let iter c}
    [neq iter #nil;]
    {set iter [cdr iter] }
  )
  #[
    [writeln [car iter] ]
  ]
}
```

break and continue
```
{for ( {let i 0} [< i 8] {set i [+ i 1] } )
  #[
    [writeln "i : " i]

    {for ( {let j 0} [< j 4] {set j [+ j 1] } )
      #[
        {if [> j 2]
          #[
            {break}
          ]
        }
        [writeln "    j : " j]
      ]
    }

    {if [> i 4]
      #[
        {break}
      ]
    }
    
    {if [> i 2]
      #[
        {continue}
      ]
    }
  ]
  
}
```

## Eval
```
{eval $[+ 1 [- 6 3] ] }
```

## Apply
```
{lambda foo ()
  #[
    [writeln "code in foo"]
    $+
  ]
}

{lambda bar ()
  #[
    [writeln "code in bar"]
    $[10 10 10]
  ]
}

{apply [foo] [bar] }
```

## call-cc
```
{let f
  {func (return)
    #[
      [return 2]
      3
    ]
  }
}

`` writes 3
[displayln
  [f {lambda (x) #[x] } ]
]

`` writes 2
[displayln {call-cc f} ]
```

## fib example
```
{lambda fib (n)
  #[
    {if {or [eq n 1] [eq n 2]}
      #[1]
      else
      #[
        [+ [fib [- n 1 ] ] [fib [- n 2] ] ]
      ]
    }
  ]
}
[writeln  "fib result " [fib 20] ]
```

## quasiquote and unquote
```
{let a 1}
{let b $[+ a 3]}

{lambda c ()
  #[
    5
  ]
}

`` return [1 2 3 $[+ a 3 ] 5 ]
[writeln @[$.a 2 $.[+ a 2] $.b $.[c]]]
```

## macro
receive ast tree. should add a `!` before macro function

```
{let m
  {macro-lambda (ast)
    #[
      [writeln ast]
    ]
  }
}

`` outputs:  {!m tt :'a'= 1 :'b' (1 2 3 ) #[t f t] m}
{!m tt :a = 1 :b (1 2 3) #[t f t] m}

`` outputs: [1 3 adsf adiop ]
[!m 1 3 adsf adiop ]
```


## accessor
Kunu's accessor is similar to file system which can store directory and file
```
{let a 1}
{let b {kn/accessor/init-prop % a} }
{b := 2}
[writeln "after =, a " a ", b " b]

{let folder {kn/accessor/init-dir %} }
{kn/accessor/put-key-val % folder "alice" 1}
{kn/accessor/put-key-val % folder "bob" 2}

```

## regex
```
{let a "abcdefgcd"}
{let regex-pattern '(ab.*?)d.*(fg)' }
{let matches {kn/regex/match % regex-pattern a} }
[writeln matches] `` outputs #<#<0 7 > #<0 3 > #<5 7 > >
```

## build in syntax sugar to call shell cmds
```
{let a "ls"}
{sh @a -al './'}

{lambda foo ()
  #[ "-h" ]
}

{sh "ps" [foo] }
```