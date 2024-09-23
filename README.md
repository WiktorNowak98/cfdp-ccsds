# CCSDS 727.0-B-5 File Delivery Protocol (CFDP)

Build with:

- clang-tidy 18.1.8
- clang-format 18.1.8

## Coding code of conduct

Any contributions are welcome, but please adhere to our code of conduct.

### Style guide

We use custom style, inspired by `Nokia` internal guidelines:

- classes, template arguments and enum values should be named using `pascal case` (e.g. `MyClass`),

- methods and variables should be named using `camel case`, to avoid any confusion with the names used in C++ standard library (e.g. `myVariable`, `doSomethingImportant()`),

- constants should be named using lowercase `snake case` (e.g. `constant_value`),

- macros and other preprocessor related magic should utilize uppercase `snake_case` (e.g. `#define IMPORTANT_VALUE 0`).

### Conventional Commits

We utilize [conventional commits](https://gist.github.com/qoomon/5dfcdf8eec66a051ecd85625518cfd13). We do not manually maintain `changelog`. Squashed commits keeping the correct convention should be more than enough to create a release summary.

## Authors

- [Wiktor Nowak](@uncommon-nickname)
- [Rafał Górski](@Qlas)
