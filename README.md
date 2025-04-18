# M-Calculator Version 4

A fully featured CLI scientific calculator (which I probably should have written
this in Rust because string parsing in C is physically painful).

## Building
Requirements: `make` and a C-compiler such as `gcc` or `clang`.

```
git clone https://github.com/EHMD28/m-calc_ver.4.git
cd m-calc_ver.4
make release
```

## Usage

Start `mcalc4` by typing in `mcalc4` command with no arguments. If there
are arguments, they will be evaluted without the application actually
starting. Once the application is started, the user will be prompted to 
enter an input until they type `exit`, at which point the program will
exit.  

### Demo
```
$ mcalc4
(mcalc4) cos(pi)
cos(pi) = -1.000000
(mcalc4) set angle deg
Setting angle mode to degrees
(mcalc4) cos(pi)
cos(pi) = 0.998497
(mcalc4) cos(180)
cos(180) = -1.000000
(mcalc4) exit
$ 
```

## Expressions
All expressions are case insensitive, so there is no need to worry about
capitalization.

* Operators
    * `+`, `-`, `*`, `/`, and `^`.
* Functions
    * `sqrt`
    * `sin`, `cos`, and `tan`.
    * `arcsin`, `arccos`, and `arcctan`.
    * `log` (base 10) and `ln`.
* Constants
    * `pi` and `e`.

## Variables

Syntax: `let {VARIABLE_NAME} = {EXPRESSION}`. Note that variable name must be
one alphabetical character and expression must be a valid mathematical
expression. Also, `e` cannot be used as a variable name because it is also a
constant. 

```
(mcalc4) let x = 5
(mcalc4) x * 2 = 10.0
```

## Settings

Syntax: `set {SETTING_NAME} {VALUE}`.

* Setting Names:
    * `angle`
        * `rad` - Sets the angle to radians.
        * `deg` - Sets the angle to degrees.
