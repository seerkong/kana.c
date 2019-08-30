# Kunu
Kunu is a programming language. Heavily inspired by Scheme, _why's Potion language, JSON, XML.

Note: Kunu isn’t done yet. The implement may make breaking changes and produce new bugs at anytime. Just play it for fun.

# DONE list
- [x] a CPS (Continuation Passing Style) tree-walking interpreter
- [x] call-cc (call with current continuation)
- [x] eval, apply
- [x] lexical-scoped procedure using `lambda` keyword
- [x] dynamic-scoped procedure using `func` keyword
- [x] dynamic-scoped code block can be assigned to a vairable using `blk` keyword.
- [x] a mark-sweep GC.
- [x] a data language like XML and JSON.
- [x] chained calls. SVOVOVO...

# TODO list
- [ ] scheme-like Number tower
- [ ] pattern matching
- [ ] ffi
- [ ] partial apply, curry
- [ ] compile to GraalVM
- [ ] coroutine
- [ ] asyn await
- [ ] multi-thread support
- [ ] a simplified scheme-like marco system
- [ ] IDE support
- [ ] error handling
- [ ] break point, debugger
- [ ] module system
- [ ] package manager
- [ ] lightweight regex
- [ ] perl6-like grammar syntax
- [ ] pointer equality call-cc in tail call. `{call-cc {lambda (cc1) #[{call-cc {lambda (cc2) #[ [eq cc1 cc2] ] } } ] } } => #t;`
- [ ] racket-like continuation marks

# build

## prepare environment
first install [xmake](https://raw.githubusercontent.com/xmake-io/xmake/master/README.md) to build this project.
Mac:
```
brew install xmake
```

## clone code and build
```
git cloen https://github.com/seerkong/kunu.c.git
cd Kunu
xmake
```

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


All special forms expressions like `cond`, `if` ... uses cell structure.
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


Object message passing expressions uses cell structre too.
```
{+ % (1 2) + (3) | (+ 4 5 6)}
```
we have a subject: `+`,
and three verb-objects clauses:
`% 1 2` , `+ 3` , `| + 4 5 6`

`%` is a infix means 'apply arguments to the object'.
`.` is a infix means 'send a signal the object'

`|` is a infix means 'pipe the object to the next function's first argument', just like the pipe symbol in shell cmd: 'ls |grep '.

The similar method call style in Kunu eg:
```
{obj set-name ("zhangsan") }
```
`(`,`)` can be omitted if method argument length is 0.
```
{obj clone}
```

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


## Number

### Number compare

```
[> 5 3] `` #t;
[< 5 3] `` #f;
[>= 5 5] `` #t;
[<= 2 4] `` #t;
```

## string
```
"hello world"
```

## Vector
```
#< 1 2 3 4 >
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
  #[
    {text ("hello") }
    {text ("world") }
  ]
}
```

## Define variable
```
{let a 1}
```

## Update variable
```
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

## Block
```
{let t
  {blk
    #[
      {set a 1}
      [+ a b c]
    ]
  }
}
{let a 5}
{let b 6}
{let c 7}
[t]   `` return 14

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

## implement a tiny prototype-based inheritance example

```

{let ProtoObjDispatcher
  {def-dispatcher
    #(
    )
  }
}

{lambda make-proto-obj-setter (obj key)
  #[
    {lambda (accessor new-val)
      #[
        ``[writeln "update in obj key " key " to val : " new-val]
        {kn/accessor/put-key-val %
          (obj key new-val)
        }
      ]
    }
  ]
}

`` if not found, return #undef;
{lambda proto-obj_lookup-key (obj key)
  #[
    {if {kn/accessor/has-key % (obj key) }
      #[
        {kn/accessor/at-key % (obj key) }
      ]
      else
      #[
        {let proto-obj {kn/accessor/at-key % (obj "__proto") } }

        {if [neq #undef; proto-obj]
          #[
            {set proto-obj {kn/accessor/unbox % (proto-obj) } }
          ]
        }
        {if [neq #nil; proto-obj ]
          #[
            {set proto-obj {kn/accessor/unbox % (proto-obj)} }
            [proto-obj_lookup-key proto-obj key]
          ]
          else
          #[
            #undef;
          ]
        }
      ]
    }
  ]

}

{set-dispatcher ProtoObjDispatcher
  #(
  :on-symbol
    {lambda (this sym)
      #[
        {let find-res [proto-obj_lookup-key this sym] }
        {if [neq find-res #undef;]
          #[
            {kn/accessor/init-prop %
              (find-res $r [make-proto-obj-setter this sym] )
            }
          ]
          `` make a new symbol
          else
          #[
            {kn/accessor/init-prop %
              (#ukn; $r [make-proto-obj-setter this sym] )
            }
          ]
        }
      ]
    }
  :on-method-call
    {lambda (self msg args)
      #[
        {lambda eval-method (method)
          #[
            {let this self}
            {apply method args}
          ]
        }

        {let find-res [proto-obj_lookup-key self msg] }

        `` check if is a procedure
        {if
          {and
            [neq find-res #undef;]
            [is-procedure {kn/accessor/unbox % (find-res) } ]
          }
          #[
            [eval-method find-res]
          ]
          else
          #[
            [writeln "method " msg " not found"]
          ]
        }
      ]
    }
  )
}


{let ProtoObjBuilderDispatcher
  {def-dispatcher
    #(
    :on-method-call
      {lambda (subj msg method-args)
        #[
          {lambda clone-proto-obj ()
            #[
              {set subj [unbox-quote subj] }
              `` create a new attr slot
              {let new-instance {kn/accessor/init-dir %} }
              `` set proto attr
              {let proto-name {kn/cell/get-core % (subj) } }
              {let obj-init-table {kn/cell/get-table % (subj) } }
              `` object structure layout version
              {kn/accessor/put-key-val % (new-instance "__scheme" $ProtoClass) }

              {if [eq proto-name #ukn; ]
                #[
                  {kn/accessor/put-key-val % (new-instance "__proto" #nil;) }
                ]
                else
                #[
                  {kn/accessor/put-key-val % (new-instance "__proto" {eval [to-variable proto-name] } ) }
                ]
              }
              {for
                (
                  {let iter {kn/table/iter-head % (obj-init-table) } }
                  [neq iter #nil;]
                  {set iter {kn/table/iter-next % (obj-init-table iter) } }
                )
                #[
                  {let init-key {kn/table/iter-key % (obj-init-table iter) } }
                  {let init-val {kn/table/iter-val % (obj-init-table iter) } }
                  ``[writeln "init key: " init-key  " val: " init-val ]

                  `` evaluate each val
                  {set init-val {eval init-val} }
                  
                  {kn/accessor/put-key-val %
                    (new-instance init-key init-val)
                  }
                  
                ]
              }

              [set-dispatcher-id new-instance ProtoObjDispatcher]
              new-instance
            ]
          }

          {cond
            [eq $clone msg]
            #[
              [clone-proto-obj]
            ]
            else
            `` TODO handle other class level message
            #ukn;
          }
        ]
      }
    )
  }
}

{def-builder
  ProtoObj
  ProtoObjBuilderDispatcher
  ${
    (
    :default-attr1 5
    :default-attr2 "absddd"
    )
  }
}


{let my-obj {ProtoObj clone} }
my-obj

{my-obj .__scheme}
{my-obj .default-attr1 := 8}
{my-obj .default-attr1}

`` test setter 1
{my-obj .my-new-key := "my new key value 1"}
[writeln {my-obj .my-new-key} ]

`` test setter 2
{my-obj .my-new-key := "my new key value 2"}
[writeln {my-obj .my-new-key} ]


{def-builder
  Person
  ProtoObjBuilderDispatcher
  ${my-obj `` parent proto name
    (
    :name "Undefined"
    :age 0
    :sex #ukn;
    :set-sex
      {func (sex)
        #[
          {this .sex := sex}
        ]
      }
    :print
      {func ()
        #[
          [writeln
            "My name is  " {this /name} ", "
            "I am " {this /age} " years old, "
            "my sex is " {this /sex}
          ]
        ]
      }
    )
  }
}


{let p {Person clone} }
`` overwrite my-new-key
{p .my-new-key := "my new key value 3"}
`` show object structure
``[writeln p]

{p .name := "Loki"}
{p .age :=  1500}
{p set-sex ($male)}
[writeln p]
{p print}

```