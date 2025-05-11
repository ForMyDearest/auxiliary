# Introduction

[![License](https://img.shields.io/github/license/ForMyDearest/auxiliary?label=license&style=flat-square)](./LICENSE)

Tools for C++

# Components

| Module            |          Description          | Reference                                                          |
|-------------------|:-----------------------------:|--------------------------------------------------------------------|
| `json`            | JsonSerde for basic data type | [SakuraEngine](https://github.com/SakuraEngine/SakuraEngine) (MIT) |
| `hash`            | Compile-time and runtime hash |                                                                    |
| `intrusive_ptr`   |      Intrusive smart ptr      |                                                                    |
| `compressed_pair` |      EBCO optimized pair      | [entt](https://github.com/skypjack/entt) (MIT)                     |

# Dependencies

- [yyjson](https://github.com/ibireme/yyjson) (MIT)
- [xxhash](https://github.com/Cyan4973/xxHash) (BSD 2-Clause License)

# TODO

* [ ] Check a bug at unit_test/json.cpp mi_free_size: pointer might not point to a valid heap region __?__
* [ ] Binary Serde
* [ ] Image coder
