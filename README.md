# kon-c
Kon is a simple programming language, aiming for embedding into other host language easily, and easy to make your own DSL. Easy to make nested data using Kon's vector, table, list, cell data container. It also can be used as a markup language with logic code embedded. Heavily inspired by Scheme, JSON, XML. This repo is a c99 implementation.

# environment
install xmake. make it easier to build c project
```
brew search xmake
```

build and install tbox, a glib-like multi-platform c library
```
git clone https://github.com/tboox/tbox
cd ./tbox
xmake f --hash=y --regex=y --object=y --charset=y --coroutine=y -c -m
xmake install

```

# generate xcode project after add custom c files

xmake project -k cmakelists

mkdir -p xcode
cd xcode
cmake .. -G "Xcode"

