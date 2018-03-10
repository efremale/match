# Match

Pattern matching-like functionality for `std::visit` on `std::variant`.

## Syntax

```
auto result =
  match ([variant]) (
    case_<[sequence_of_types_0]> (function_0),
    case_<[sequence_of_types_1]> (function_1),
    ...
    case_<[sequence_of_types_n]> (function_n),
    [optional] default_ (function_default)
  );
```

If `variant` contains type `T` and `T` belongs to the `[sequence_of_types_i]`, the result equivalent to calling
```
std::visit (function_i, [variant]);
```
Type sequences are allowed to overlap, in that case the first matching sequence is used.

Default case is optional. When default case is not present, the union of type sequences is required to cover all possible type of the variant, otherwise compilation error will occur.

All functions must return the same type (requirement of `std::visit`);

## Example
We can write
```
auto v = std::variant<int, double, std::string, void *> { "123" };
auto number = 
  match (v) (
    case_<int, double> ([] (auto val) -> int { return val; }),
    case_<std::string> ([] (auto && str) -> int { return std::stoi (str); }),
    default_ ([] (auto) -> int { throw "not a number"; })
  );
  ```
instead of
```
struct to_int_visitor {
  int operator () (int val) const { return val; }
  int operator () (double val) const { return val; }
  int operator () (const std::string & str) const { return std::stoi (str); }
  template <typename Default>
  int operator () (Default) const { throw "not a number"; }
};

...

auto number = std::visit (to_int_visitor {}, v);
```
